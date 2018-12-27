/*! 
/brief This file contains all needed algorithms to operate with in-build STM32 bootloaders.
*/
#include "stm32_boot_client.hpp"
#include "stm32_io_pc.hpp"
const Stm32BootClient::McuDescription_t Stm32BootClient::m_mcuDescription[] = {
    {   /// Stm32F05xxx_F030x8
        0x20000800,
        0x20001fff,
        0x1fffec00,
        0x1ffff7ff,
        0x20000000,
        0x00002000,
        0x08000000,
        0x1ffff7cc,
        1024,
        true
    },
    {   /// Stm32F09xxx
        0x0,
        0x0,
        0x1fffd800,
        0x1ffff7ff,
        0x20000000,
        0x00008000,
        0x08000000,
        0x1ffff7cc,
        2048, 
        false
    },
};
/*!
 * Function: init 
 * Initializes client serial port and other things.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32BootClient::init() {
    ErrorCode result = Stm32Io::init();
    if (result == ErrorCode::OK) {
        Stm32Io::delayMs(BOOT_READY_DELAY);
    }
    return result;
}
/*!
 * Function: checkMcuPresence 
 * Sends 0x7f to MCU that must have been connected to serial port, then waits for ACK
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32BootClient::checkMcuPresence() {
    ErrorCode result;
    uint8_t txbuff[] = {ACK_ASK_CODE};
    size_t writtern;
    Stm32Io::setBootLine(true);
    ResetMCU();
    Stm32Io::setBootLine(false);
    result = Stm32Io::write(txbuff, sizeof( txbuff ), &writtern);
    if (result == ErrorCode::OK) {
        result = ( writtern == sizeof( txbuff ) ) ? ErrorCode::OK : ErrorCode::FAILED;
        if (result == ErrorCode::OK) {
            uint8_t rxbuff[1] = {0};
            size_t count;
            result = Stm32Io::read(rxbuff, sizeof( rxbuff ), &count);
            if (result == ErrorCode::OK) {
                result = ( count == sizeof( rxbuff ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                if (result == ErrorCode::OK) {
                    result = ( rxbuff[0] == ACK_RESP_CODE ) ? ErrorCode::ACK_OK : ErrorCode::ACK_FAILED;
                }
            }
        }
    }
    return result;
}
std::string Stm32BootClient::errorCode2String( ErrorCode _errcode ) {
    std::string msgs[] = {
        "OK",
        "Generic Error",
        "ACK has been received",
        "No ACK received",
        "Can't open serial port",
        "Debug code",
        "Unknown MCU type",
        "Can't write N bytes to serial port"
    };
    size_t idx = static_cast<int>(_errcode);
    configASSERT(idx < ARRAY_SIZE(msgs));
    return msgs[idx];
}
std::string Stm32BootClient::mcuType2String( McuType _type ) {
    static const std::string name[] = {
        "STM32F05xxx or STM32F030x8",
        "STM32F09xxx"
    };
    std::string result = "Unknown";
    size_t idx = static_cast<size_t>(_type);
    if (idx != 0xff) {
        configASSERT(idx < ARRAY_SIZE(name));
        result = name[idx];
    }
    return result;
}
Stm32BootClient::McuDescription_t Stm32BootClient::mcuType2Description( McuType _type ) {
    McuDescription_t result;
    memset(&result, 0xff, sizeof( result ));
    if (_type != McuType::Unknown) {
        size_t idx = static_cast<size_t>(_type);
        configASSERT(idx < ARRAY_SIZE(m_mcuDescription));
        result = m_mcuDescription[idx];
    }
    return result;
}
Stm32BootClient::McuType Stm32BootClient::chipId2McuType( uint16_t _chipid ) {
    Stm32BootClient::McuType result = McuType::Unknown;
    if (_chipid == 0x0440)
        result = McuType::Stm32F05xxx_F030x8;
    else if (_chipid == 0x0442) {
        result = McuType::Stm32F09xxx;
    }
    return result;

}
void Stm32BootClient::ResetMCU() {
    Stm32Io::setResetLine(false);
    Stm32Io::delayMs(100);
    Stm32Io::setResetLine(true);
}
/*!
 * Function: commandGet 
 * Get Command function
 * 
 * @param _resp 
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32BootClient::commandGet( CommandGetResponse_t &_resp ) {
    ErrorCode err;
    size_t written;
    static const uint8_t cmd = static_cast<uint8_t>(Command::Get);
    uint8_t txBuff[] = { cmd, static_cast<uint8_t>(~cmd)};
    err = Stm32Io::write(txBuff, sizeof( txBuff ), &written);
    if (err == ErrorCode::OK) {
        err = ( written == sizeof( txBuff ) ) ? ErrorCode::OK : ErrorCode::FAILED;
        if (err == ErrorCode::OK) {
            size_t rd;
            uint8_t ackCode;
            err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
            if (err == ErrorCode::OK) {
                err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                if (err == ErrorCode::OK) {
                    err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::ACK_OK : ErrorCode::ACK_FAILED;
                    if (err == ErrorCode::ACK_OK) {
                        err = Stm32Io::read(&_resp, sizeof( CommandGetResponse_t ), &rd);
                        if (err == ErrorCode::OK) {
                            err = ( rd == sizeof( _resp ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                            if (err == ErrorCode::OK) {
                                err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
                                if (err == ErrorCode::OK) {
                                    err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                                    if (err == ErrorCode::OK) {
                                        err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::OK : ErrorCode::ACK_FAILED;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return err;
}
/*!
 * Function: commandGvRps 
 * Get Version & Read Protection Status Command function.
 * 
 * @param _resp 
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32BootClient::commandGvRps( CommandGvRpsResponse_t &_resp ) {
    ErrorCode err;
    size_t written;
    static const uint8_t cmd = static_cast<uint8_t>(Command::GvRps);
    uint8_t txBuff[] = { cmd, static_cast<uint8_t>(~cmd)};
    err = Stm32Io::write(txBuff, sizeof( txBuff ), &written);
    if (err == ErrorCode::OK) {
        err = ( written == sizeof( txBuff ) ) ? ErrorCode::OK : ErrorCode::FAILED;
        if (err == ErrorCode::OK) {
            size_t rd;
            uint8_t ackCode;
            err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
            if (err == ErrorCode::OK) {
                err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                if (err == ErrorCode::OK) {
                    err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::ACK_OK : ErrorCode::ACK_FAILED;
                    if (err == ErrorCode::ACK_OK) {
                        err = Stm32Io::read(&_resp, sizeof( _resp ), &rd);
                        if (err == ErrorCode::OK) {
                            err = ( rd == sizeof( _resp ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                            if (err == ErrorCode::OK) {
                                err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
                                if (err == ErrorCode::OK) {
                                    err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                                    if (err == ErrorCode::OK) {
                                        err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::OK : ErrorCode::ACK_FAILED;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::commandGetId( CommandGetIdResponse_t &_resp ) {
    ErrorCode err;
    size_t written;
    static const uint8_t cmd = static_cast<uint8_t>(Command::Getid);
    uint8_t txBuff[] = { cmd, static_cast<uint8_t>(~cmd)};
    err = Stm32Io::write(txBuff, sizeof( txBuff ), &written);
    if (err == ErrorCode::OK) {
        err = ( written == sizeof( txBuff ) ) ? ErrorCode::OK : ErrorCode::FAILED;
        if (err == ErrorCode::OK) {
            size_t rd;
            uint8_t ackCode;
            err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
            if (err == ErrorCode::OK) {
                err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                if (err == ErrorCode::OK) {
                    err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::ACK_OK : ErrorCode::ACK_FAILED;
                    if (err == ErrorCode::ACK_OK) {
                        err = Stm32Io::read(&_resp, sizeof( _resp ), &rd);
                        if (err == ErrorCode::OK) {
                            err = ( rd == sizeof( _resp ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                            if (err == ErrorCode::OK) {
                                err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
                                if (err == ErrorCode::OK) {
                                    err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                                    if (err == ErrorCode::OK) {
                                        err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::OK : ErrorCode::ACK_FAILED;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return err;
}
// TODO Check with RDP is active, must be twon NACKS
Stm32BootClient::ErrorCode Stm32BootClient::commandReadMemory( void * _dst, uint32_t _addr, size_t _size ) {
    configASSERT(_dst);
    configASSERT(_size && _size <= 0x100);
    ErrorCode err;
    size_t written;
    err = commandGenericSend(Command::ReadMemory);
    if (err == ErrorCode::ACK_OK) {
        err = genericSendAddr(_addr);
        if (err == ErrorCode::ACK_OK) {
            uint8_t sz = static_cast<uint8_t>(_size - 1);
            uint8_t numarr[] = {sz, static_cast<uint8_t>(~sz)};
            err = Stm32Io::write(numarr, sizeof( numarr ), &written);
            if (err == ErrorCode::OK) {
                err = ( written == sizeof( numarr ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                if (err == ErrorCode::OK) {
                    uint8_t ackCode;
                    size_t rd;
                    err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
                    if (err == ErrorCode::OK) {
                        err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                        if (err == ErrorCode::OK) {
                            err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::ACK_OK : ErrorCode::ACK_FAILED;
                            if (err == ErrorCode::ACK_OK) {
                                err = Stm32Io::read(_dst, _size, &rd);
                                if (err == ErrorCode::OK && rd != _size)
                                    err = ErrorCode::FAILED;
                            }
                        }
                    }
                }
            }
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::commandGo( uint32_t _addr ) {
    ErrorCode err = commandGenericSend(Command::Go);
    if (err == ErrorCode::ACK_OK) {
        err = genericSendAddr(_addr);
    }
    return err;
}
// TODO check out of range memory address
Stm32BootClient::ErrorCode Stm32BootClient::commandWriteMemory( const void * _src, uint32_t _addr, size_t _size ) {
    configASSERT(_src);
    configASSERT(_size && _size <= 0x100 && !( _size % 4 ));
    ErrorCode err = commandGenericSend(Command::WriteMem);
    if (err == ErrorCode::ACK_OK) {
        err = genericSendAddr(_addr);
        if (err == ErrorCode::ACK_OK) {
            uint8_t sz = static_cast<uint8_t>(_size - 1);
            uint8_t xor_cs = calculateXor(static_cast<const uint8_t *>(_src), _size) ^ sz;
            size_t written;
            err = Stm32Io::write(&sz, sizeof( sz ), &written);
            if (err == ErrorCode::OK) {
                err = ( written == sizeof( sz ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                if (err == ErrorCode::OK) {
                    err = Stm32Io::write(_src, _size, &written);
                    if (err == ErrorCode::OK) {
                        err = ( written == _size ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                        if (err == ErrorCode::OK) {
                            err = Stm32Io::write(&xor_cs, sizeof( xor_cs ), &written);
                            if (err == ErrorCode::OK) {
                                err = ( written == sizeof( xor_cs ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                                if (err == ErrorCode::OK) {
                                    uint8_t ackCode;
                                    size_t rd;
                                    err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
                                    if (err == ErrorCode::OK) {
                                        err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                                        if (err == ErrorCode::OK) {
                                            err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::OK : ErrorCode::ACK_FAILED;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return err;
}
// if _pagenumarray == nullptr then we do global erase
Stm32BootClient::ErrorCode Stm32BootClient::commandErase( const uint8_t * _pagenumarray, size_t _count ) {
    auto err = commandGenericSend(Command::Erase);
    if (err == ErrorCode::ACK_OK) {
        size_t written;
        if (_pagenumarray == nullptr) {
            uint8_t txarr[] = {0xff, 0};
            err = Stm32Io::write(txarr, sizeof( txarr ), &written);
            if (err == ErrorCode::OK) {
                err = ( written == sizeof( txarr ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
            }
        } else {
            uint8_t sz = static_cast<uint8_t>(_count);
            uint8_t xor_cs = calculateXor(_pagenumarray, _count) ^ sz;
            err = Stm32Io::write(&sz, sizeof( sz ), &written);
            if (err == ErrorCode::OK) {
                err = ( written == sizeof( sz ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                if (err == ErrorCode::OK) {
                    err = Stm32Io::write(_pagenumarray, _count, &written);
                    if (err == ErrorCode::OK) {
                        err = ( written == _count ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                        if (err == ErrorCode::OK) {
                            err = Stm32Io::write(&xor_cs, sizeof( xor_cs ), &written);
                            if (err == ErrorCode::OK) {
                                err = ( written == sizeof( xor_cs ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                            }
                        }
                    }
                }
            }
        }
        if (err == ErrorCode::OK) {
            uint8_t ackCode;
            size_t rd;
            err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
            if (err == ErrorCode::OK) {
                err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                if (err == ErrorCode::OK) {
                    err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::OK : ErrorCode::ACK_FAILED;
                }
            }
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::commandExtendedErase( const uint16_t * _pagenumarray, uint16_t _count ) {
    auto err = commandGenericSend(Command::ExtErase);
    if (err == ErrorCode::ACK_OK) {
        size_t written;
        if (_count == EXT_MASS_ERASE || _count == EXT_BANK1_ERASE || _count == EXT_BANK2_ERASE) {
            uint16_t txarray[] = {_count, static_cast<uint16_t>(~_count)};
            err = Stm32Io::write(txarray, sizeof( txarray ), &written);
            if (err == ErrorCode::OK) {
                err = ( written == sizeof( txarray ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
            }
        } else {
            uint16_t sz = _count;
            sz--;
            uint8_t xor_cs = calculateXor(reinterpret_cast<const uint8_t *>(_pagenumarray), 2 * _count);
            xor_cs = xor_cs ^ calculateXor(reinterpret_cast<const uint8_t *>(&sz), sizeof( sz ));
            err = serialWrite16(&_count, 1, &written);
            if (err == ErrorCode::OK) {
                err = ( written == 1 ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                if (err == ErrorCode::OK) {
                    err = serialWrite16(_pagenumarray, _count, &written);
                    if (err == ErrorCode::OK) {
                        err = ( written == _count ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                        if (err == ErrorCode::OK) {
                            err = Stm32Io::write(&xor_cs, sizeof( xor_cs ), &written);
                            if (err == ErrorCode::OK) {
                                err = ( written == sizeof( xor_cs ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
                            }
                        }
                    }
                }
            }
        }
        if (err == ErrorCode::OK) {
            uint8_t ackCode;
            size_t rd;
            err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
            if (err == ErrorCode::OK) {
                err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::SERIAL_RD_SIZE;
                if (err == ErrorCode::OK) {
                    err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::OK : ErrorCode::ACK_FAILED;
                }
            }
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::genericSendAddr( uint32_t _addr ) {
    ErrorCode err;
    uint8_t addr_array[4];
    addr32_to_byte(_addr, addr_array);
    //std::cout << std::hex << (int)addr_array[0] << " " << (int)addr_array[1] << " " << (int)addr_array[2] << " " << (int)addr_array[3];
    size_t written;
    err = Stm32Io::write(addr_array, sizeof( addr_array ), &written);
    if (err == ErrorCode::OK && written == sizeof( addr_array )) {
        uint8_t xor_cs = calculateXor(reinterpret_cast<uint8_t *>(&_addr), sizeof( _addr ));
        err = Stm32Io::write(&xor_cs, sizeof( xor_cs ), &written);
        if (err == ErrorCode::OK) {
            err = ( written == sizeof( xor_cs ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
            if (err == ErrorCode::OK) {
                uint8_t ackCode;
                size_t rd;
                err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
                if (err == ErrorCode::OK) {
                    err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::SERIAL_RD_SIZE;
                    if (err == ErrorCode::OK) {
                        /// TODO Maby two NACKs
                        err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::ACK_OK : ErrorCode::ACK_FAILED;
                    }
                }
            }
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::readMcuSpecificInfo( uint16_t _chipid, McuSpecificInfo_t &_info ) {
    ErrorCode err;
    McuType mcu = chipId2McuType(_chipid);
    if (mcu == McuType::Unknown)
        err = ErrorCode::UNKNOWN_MCU;
    else {
        McuDescription_t descr = mcuType2Description(mcu);
        if (descr.blRamBegin == 0xffffffff)
            err = ErrorCode::UNKNOWN_MCU;
        else {
            err = commandReadMemory(&_info.flashSize, descr.flashSizeReg, 2);
            if (err == ErrorCode::OK) {
                _info.flashSize *= 1024; /// as size of the device expressed in Kbytes
            }
        }
    }
    return err;
}
uint8_t Stm32BootClient::calculateXor( const uint8_t * _src, size_t _size ) {
    configASSERT(_src);
    configASSERT(_size);
    uint8_t result = *_src++;
    _size--;
    while (_size--) {
        result ^= *_src;
        _src++;
    }
    return result;
    ;
}
Stm32BootClient::ErrorCode Stm32BootClient::commandGenericSend( Command _cmd ) {
    ErrorCode err;
    size_t written;
    uint8_t cmd = static_cast<uint8_t>(_cmd);
    uint8_t txBuff[] = { cmd, static_cast<uint8_t>(~cmd)};
    err = Stm32Io::write(txBuff, sizeof( txBuff ), &written);
    if (err == ErrorCode::OK) {
        err = ( written == sizeof( txBuff ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
        if (err == ErrorCode::OK) {
            size_t rd;
            uint8_t ackCode;
            err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
            if (err == ErrorCode::OK) {
                err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                if (err == ErrorCode::OK) {
                    err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::ACK_OK : ErrorCode::ACK_FAILED;
                }
            }
        }
    }
    return err;
}
void Stm32BootClient::addr32_to_byte( uint32_t _addr, uint8_t * _array ) {
    configASSERT(_array);
    for ( size_t addr_b_count = 0; addr_b_count < sizeof( _addr ) && _array; addr_b_count++ ) {
        *_array++ = static_cast<uint8_t>(_addr >> ( 24 - addr_b_count * 8 ));
    }
}
/*!
 * Function: serialWrite16 
 * Sends an array of uint16_t, MSB first.
 * 
 * @param _src pointer to data.
 * @param _size size in half-words (2 bytes).
 * @param _written a number of half-words written to serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32BootClient::serialWrite16( const uint16_t * _src, size_t _size, size_t * _written ) {
    auto err = ErrorCode::OK;
    configASSERT(_src);
    if (_written)
        *_written = 0;
    while (_size-- && err == ErrorCode::OK) {
        uint8_t txarr[] = {static_cast<uint8_t>(( *_src >> 8 ) & 0xff), static_cast<uint8_t>(*_src & 0xff)};
        _src++;
        size_t written;
        err = Stm32Io::write(txarr, sizeof( txarr ), &written);
        if (err == ErrorCode::OK) {
            err = ( written == sizeof( txarr ) ) ? ErrorCode::OK : ErrorCode::SERIAL_WR_SIZE;
            if (err == ErrorCode::OK)
                ( *_written )++;
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::readMemory( void * _dst, uint32_t _addr, size_t _size ) {
    configASSERT(_dst);
    auto err = ErrorCode::OK;
    while (_size && err == ErrorCode::OK) {
        size_t bytes_to_send = ( _size > 256 ) ? 256 : _size;
        _size -= bytes_to_send;
        err = commandReadMemory(_dst, _addr, bytes_to_send);
        if (err == ErrorCode::OK) {
            uint8_t * pArithm = static_cast<uint8_t *>(_dst);
            pArithm += bytes_to_send;
            _dst = pArithm;
            _addr += static_cast<uint32_t>(bytes_to_send);
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::writeMemory( const void * _src, uint32_t _addr, size_t _size ) {
    configASSERT(_src);
    auto err = ErrorCode::OK;
    while (_size && err == ErrorCode::OK) {
        size_t bytes_to_send = ( _size > 256 ) ? 256 : _size;
        _size -= bytes_to_send;
        err = commandWriteMemory(_src, _addr, bytes_to_send);
        if (err == ErrorCode::OK) {
            const uint8_t * pArithm = static_cast<const uint8_t *>(_src);
            pArithm += bytes_to_send;
            _src = pArithm;
            _addr += static_cast<uint32_t>(bytes_to_send);
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::eraseAllMemory() {
    CommandGetResponse_t getresp;
    auto err = commandGet(getresp);
    if (err == ErrorCode::OK) {
		bool isExtendedSupport = false;
        for ( size_t cmd = 0; cmd < getresp.getCommandListSize(); cmd++ ) {
            if (getresp.getCommand(cmd) == Command::ExtErase)
                isExtendedSupport = true;
        }
        err = isExtendedSupport ? commandExtendedErase(nullptr, EXT_MASS_ERASE) : commandErase();
    }
    return err;
}

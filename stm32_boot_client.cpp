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
        true
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
    configASSERT(_size && _size <= 0xff);
    ErrorCode err;
    size_t written;
    err = commandGenericSend(Command::ReadMemory);
    if (err == ErrorCode::ACK_OK) {
        err = ErrorCode::OK;
        uint8_t addr_array[4];
        addr32_to_byte(_addr, addr_array);
        err = Stm32Io::write(addr_array, sizeof( addr_array ), &written);
        if (err == ErrorCode::OK && written == sizeof( addr_array )) {
            uint8_t xor_cs = calculateXor(reinterpret_cast<uint8_t *>(&_addr), sizeof( _addr ));
            err = Stm32Io::write(&xor_cs, sizeof( xor_cs ), &written);
            if (err == ErrorCode::OK) {
                err = ( written == sizeof( xor_cs ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                if (err == ErrorCode::OK) {
                    uint8_t ackCode;
                    size_t rd;
                    err = Stm32Io::read(&ackCode, sizeof( ackCode ), &rd);
                    if (err == ErrorCode::OK) {
                        err = ( rd == sizeof( ackCode ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                        if (err == ErrorCode::OK) {
                            err = ( ackCode == ACK_RESP_CODE ) ? ErrorCode::ACK_OK : ErrorCode::ACK_FAILED;
                            if (err == ErrorCode::ACK_OK) {
                                uint8_t sz = static_cast<uint8_t>(_size - 1);
                                uint8_t numarr[] = {sz, static_cast<uint8_t>(~sz)};
                                err = Stm32Io::write(numarr, sizeof( numarr ), &written);
                                if (err == ErrorCode::OK) {
                                    err = ( written == sizeof( numarr ) ) ? ErrorCode::OK : ErrorCode::FAILED;
                                    if (err == ErrorCode::OK) {
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
                    }
                }
            }
        }
    }
    return err;
}
Stm32BootClient::ErrorCode Stm32BootClient::commandGo( uint32_t _addr ) {
    (void)_addr;
    return ErrorCode::OK;
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

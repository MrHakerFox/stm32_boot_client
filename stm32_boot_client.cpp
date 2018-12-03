/*! 
  /brief This file contains all needed algorithms to operate with in-build STM32 bootloaders.
  */
#include "stm32_boot_client.hpp"
#include "stm32_io_pc.hpp"
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
        "Debug code"
    };
    size_t idx = static_cast<int>(_errcode);
    configASSERT(idx < ARRAY_SIZE(msgs));
    return msgs[idx];
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
    uint8_t txBuff[] = { static_cast<uint8_t>(Command::Get), static_cast<uint8_t>(~( static_cast<uint8_t>(Command::Get) ))};
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


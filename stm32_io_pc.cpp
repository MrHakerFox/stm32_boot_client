/*! 
  /brief Platform-dependent function to handle serial port.
  */
#include "stm32_io.hpp"
#include <windows.h>
#include <stdio.h>
#include <iostream>
static HANDLE s_serialHandle;
/*!
 * Function: init 
 * Initializes serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32Io::init() {
    Stm32BootClient::ErrorCode result;
    s_serialHandle = CreateFile(
        "\\\\.\\COM3",
        GENERIC_READ | GENERIC_WRITE,
        0, NULL,
        OPEN_EXISTING,
        0,
        NULL
        );
    result = ( s_serialHandle == INVALID_HANDLE_VALUE ) ? Stm32BootClient::ErrorCode::SERIAL_CANT_OPEN :
        Stm32BootClient::ErrorCode::OK;
    if (result == Stm32BootClient::ErrorCode::OK) {
        DCB dcbSerialConfig;
        dcbSerialConfig.DCBlength = sizeof( dcbSerialConfig );
        BOOL status = GetCommState(s_serialHandle, &dcbSerialConfig);
        result = ( status == 0 ) ? Stm32BootClient::ErrorCode::FAILED : Stm32BootClient::ErrorCode::OK;
        if (result == Stm32BootClient::ErrorCode::OK) {
            dcbSerialConfig.BaudRate = CBR_115200;
            dcbSerialConfig.ByteSize = 8;
            dcbSerialConfig.StopBits = ONESTOPBIT;
            dcbSerialConfig.Parity = EVENPARITY;
            status = SetCommState(s_serialHandle, &dcbSerialConfig);
            result = ( status == 0 ) ? Stm32BootClient::ErrorCode::FAILED : Stm32BootClient::ErrorCode::OK;
            if (result == Stm32BootClient::ErrorCode::OK) {
                COMMTIMEOUTS timeouts = {};
                timeouts.ReadIntervalTimeout = 50; // in milliseconds
                timeouts.ReadTotalTimeoutConstant = 50; // in milliseconds
                timeouts.ReadTotalTimeoutMultiplier = 10; // in milliseconds
                timeouts.WriteTotalTimeoutConstant = 50; // in milliseconds
                timeouts.WriteTotalTimeoutMultiplier = 10; // in milliseconds
                status = SetCommTimeouts(s_serialHandle, &timeouts);
                result = ( status == 0 ) ? Stm32BootClient::ErrorCode::FAILED : Stm32BootClient::ErrorCode::OK;
            }
        }
    }
    return result;
}
/*!
 * Function: write 
 * Write data to serial port.
 * 
 * @param _src a pointer to the source buffer.
 * @param _size number of bytes to be written.
 * @param _written how many bytes were actually written.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32Io::write( const void * _src, size_t _size, size_t * _written ) {
    DWORD wr;
    BOOL status = WriteFile(
        s_serialHandle,
        _src,
        static_cast<DWORD>(_size),
        reinterpret_cast<LPDWORD>(&wr),
        NULL
        );
    if (_written)
        *_written = wr;

    Stm32BootClient::ErrorCode result = ( status == 0 ) ? Stm32BootClient::ErrorCode::FAILED : Stm32BootClient::ErrorCode::OK;
    return result;
}
/*!
 * Function: read 
 * Read data from serial port.
 * 
 * @param _dst a pointer to the destanation buffer.
 * @param _size number of bytes to be read.
 * @param _read how many bytes were actually read.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32Io::read( void * _dst, size_t _size, size_t * _read ) {
    DWORD rd;
    BOOL status = ReadFile(
        s_serialHandle,
        _dst,
        static_cast<DWORD>(_size),
        reinterpret_cast<LPDWORD>(&rd),
        NULL);
    if (_read) {
        *_read = rd;
    }
    Stm32BootClient::ErrorCode result = ( status == 0 ) ? Stm32BootClient::ErrorCode::FAILED : Stm32BootClient::ErrorCode::OK;
    return result;
}
/*!
 * Function: deinit 
 * Deinitializes serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32Io::deinit() {
    Stm32BootClient::ErrorCode result;
    result = CloseHandle(s_serialHandle) ? Stm32BootClient::ErrorCode::OK : Stm32BootClient::ErrorCode::FAILED;
    return result;
}
/*!
 * Function: setResetLine 
 * Control reset MCU line.
 * 
 * @param _level true - hight level, false - low level
 */
void Stm32Io::setResetLine( bool _level ) {
    if (!_level) {
        std::cout << "Reset MCU, press ENTER...";
        std::cin.get();
    }
}
/*!
 * Function: setBootLine 
 * Control boot line; 
 * 
 * 
 * @param _level true - high level, false - low level;
 */
void Stm32Io::setBootLine( bool _level ) {
    if (_level) {
        std::cout << "Set BOOT0 to high, press ENTER...";
    } else {
        std::cout << "Set BOOT0 to low, press ENTER...";
    }
    std::cin.get();
}
/*!
 * Function: delayMs 
 * Performs delay.
 * 
 * @param _delay how many ms to wait.
 */
void Stm32Io::delayMs( uint32_t _delay ) {
    Sleep(_delay);
}


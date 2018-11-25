/*! 
  /brief Platform-dependent function to handle serial port.
  */
#include "stm32_io_pc.hpp"
#include <iostream>
HANDLE Stm32Io::m_handle;
/*!
 * Function: init 
 * Initializes serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32Io::init() {
    Stm32BootClient::ErrorCode result;
    m_handle = CreateFile(
        "\\\\.\\COM1",
        GENERIC_READ | GENERIC_WRITE,
        0, NULL,
        OPEN_EXISTING,
        0,
        NULL
        );
    result = ( m_handle == INVALID_HANDLE_VALUE ) ? Stm32BootClient::ErrorCode::SERIAL_OPEN :
        Stm32BootClient::ErrorCode::OK;
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
Stm32BootClient::ErrorCode Stm32Io::write( const void * _src, size_t _size, size_t * _written ) {}
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
Stm32BootClient::ErrorCode Stm32Io::read( void * _dst, size_t _size, size_t * _read ) {}
/*!
 * Function: deinit 
 * Deinitializes serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32Io::deinit() {}
/*!
 * Function: setResetLine 
 * Control reset MCU line.
 * 
 * @param _level true - hight level, false - low level
 */
void Stm32Io::setResetLine( bool _level ) {
    std::cout << "Set BOOT Line in high state, then reset MCU, press ENTER....";
    std::cin.get();
}
/*!
 * Function: setBootLine 
 * Control boot line; 
 * 
 * 
 * @param _level true - high level, false - low level;
 */
void Stm32Io::setBootLine( bool _level ) {
    /// For pc do nothing
}
/*!
 * Function: delayMs 
 * Performs delay.
 * 
 * @param _delay how many ms to wait.
 */
void Stm32Io::delayMs( size_t _delay ) {}


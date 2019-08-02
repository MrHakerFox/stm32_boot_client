/*! 
  /brief Platform-dependent function to handle serial port.
  */
#include "stm32_io.hpp" 
#include <stdio.h>
/*!
 * Function: init 
 * Initializes serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32Io::init() {
  Stm32BootClient::ErrorCode result = Stm32BootClient::ErrorCode::FAILED;
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
  Stm32BootClient::ErrorCode result = Stm32BootClient::ErrorCode::FAILED;
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
    Stm32BootClient::ErrorCode result = Stm32BootClient::ErrorCode::FAILED;
    return result;
}
/*!
 * Function: deinit 
 * Deinitializes serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32Io::deinit() {
    Stm32BootClient::ErrorCode result = Stm32BootClient::ErrorCode::FAILED;
    return result;
}
/*!
 * Function: setResetLine 
 * Control reset MCU line.
 * 
 * @param _level true - hight level, false - low level
 */
void Stm32Io::setResetLine( bool _level ) {
}
/*!
 * Function: setBootLine 
 * Control boot line; 
 * 
 * 
 * @param _level true - high level, false - low level;
 */
void Stm32Io::setBootLine( bool _level ) {
}
/*!
 * Function: delay 
 * Performs delay.
 * 
 * @param _delay how many ms to wait.
 */
void Stm32Io::delay( uint32_t _delay ) {
    delayMs(_delay);
}


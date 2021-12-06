/*! 
  /brief Platform-dependent function to handle serial port.
  */
#include "stm32_io.hpp"
#include "lpc43xx_gpio.h"
#include "lpc43xx_scu.h"
#include "drivers\serial\lpc43xx_serial.hpp"
#include "modules\measurements\meas_core_calc.hpp"
#include <stdio.h>

static const GpioSetup_t s_busGpio[] = {
    { /// Microohmmeter RESET
        1, 3, FUNC0, 0, 10, DIR_OUT,
    },
    { /// Microohmmeter BOOT
        4, 9, FUNC4, 5, 13, DIR_OUT,
    },
    { /// Switch RESET
        6, 2, FUNC0, 3, 1, DIR_OUT,
    },
    { /// Switch BOOT
        6, 1, FUNC0, 3, 0, DIR_OUT,
    },
};
static Lpc43xxSerialDriver * s_busSerial[] = {
    Lpc43xxSerialDriver::instance(EXT_MICROOHMMETER_UART_NUM),
    Lpc43xxSerialDriver::instance(EXT_SWTICH_UART_NUM),
};

Stm32BootLowIo::Bus Stm32BootLowIo::m_bus = Stm32BootLowIo::Bus::Undefined;

int Stm32BootLowIo::getCurrentBusIdx() {
    auto busIdx = static_cast<int>(m_bus);
    configASSERT(busIdx > -1 && busIdx < ARRAY_SIZE(s_busGpio));
    return busIdx;
}
static Lpc43xxSerialDriver * getCurrentSerial() {
    auto p = s_busSerial[Stm32BootLowIo::getCurrentBusIdx()];
    configASSERT(p);
    return p;
}
/*!
 * Function: init 
 * Initializes serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32BootLowIo::init() {
    configASSERT(ARRAY_SIZE(s_busSerial) == MeasCore::Calc::instance()->getUpdatableUnitCount());
    configASSERT(ARRAY_SIZE(s_busSerial) == MeasCore::Calc::instance()->getUpdatableUnitCount());
    for ( auto i = 0; i < ARRAY_SIZE(s_busSerial); i++ ) {
        auto p = s_busSerial[i];
        configASSERT(p);
        AbstractSerialDriver::TSettings settings;
        settings.parameters.parity = AbstractSerialDriver::pEVEN;
        p->close();
        configASSERT(p->open(&settings) == rvOK);
    }
    return Stm32BootClient::ErrorCode::OK;
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
Stm32BootClient::ErrorCode Stm32BootLowIo::write( const void * _src, size_t _size, size_t * _written ) {
    configASSERT(_src);
    auto result = getCurrentSerial()->write(static_cast<const uint8_t *>(_src), _size, 100);
    if (_written)
        *_written = _size;
    return result == rvOK ? Stm32BootClient::ErrorCode::OK : Stm32BootClient::ErrorCode::SERIAL_WR_FAILED;
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
Stm32BootClient::ErrorCode Stm32BootLowIo::read( void * _dst, size_t _size, size_t * _read ) {
    auto result = getCurrentSerial()->read(static_cast<uint8_t *>(_dst), _size, *_read, 100);
    return result == rvOK ? Stm32BootClient::ErrorCode::OK : Stm32BootClient::ErrorCode::SERIAL_RD_FAILED;
}
/*!
 * Function: deinit 
 * Deinitializes serial port.
 * 
 * @return Stm32BootClient::ErrorCode 
 */
Stm32BootClient::ErrorCode Stm32BootLowIo::deinit() {
    Stm32BootClient::ErrorCode result = Stm32BootClient::ErrorCode::FAILED;
    return result;
}
Stm32BootClient::ErrorCode Stm32BootLowIo::flush() {
    getCurrentSerial()->flush();
    return Stm32BootClient::ErrorCode::OK;
}
/*!
 * Function: setResetLine 
 * Control reset MCU line.
 * 
 * @param _level true - hight level, false - low level
 */
void Stm32BootLowIo::setResetLine( bool _level ) {
    GPIO_SetUpArray(s_busGpio, ARRAY_SIZE(s_busGpio));
    GPIO_WritePinValue(s_busGpio[getCurrentBusIdx() * 2], _level ? 1 : 0);
}
/*!
 * Function: setBootLine 
 * Control boot line; 
 * 
 * 
 * @param _level true - high level, false - low level;
 */
void Stm32BootLowIo::setBootLine( bool _level ) {
    GPIO_SetUpArray(s_busGpio, ARRAY_SIZE(s_busGpio));
    GPIO_WritePinValue(s_busGpio[getCurrentBusIdx() * 2 + 1], _level ? 1 : 0);
}
/*!
 * Function: delay 
 * Performs delay.
 * 
 * @param _delay how many ms to wait.
 */
void Stm32BootLowIo::delay( uint32_t _delay ) {
    delayMs(_delay);
}
void Stm32BootLowIo::setSerialBus( Bus _code ) {
    configASSERT(_code != Bus::Undefined);
    m_bus = _code;
}

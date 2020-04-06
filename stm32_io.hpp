#pragma once
#ifdef __cplusplus
#include "stm32_boot_client.hpp"
#include "included_macro.hpp"
#include <inttypes.h>

class Stm32BootLowIo {
public:
    enum class Bus : int8_t {
        Undefined = -1,
        Bus0,
        Bus1
    };
    static Stm32BootClient::ErrorCode init();
    static Stm32BootClient::ErrorCode write( const void * _src, size_t _size, size_t * _written = nullptr );
    static Stm32BootClient::ErrorCode read( void * _dst, size_t _size, size_t * _read = nullptr );
    static Stm32BootClient::ErrorCode deinit();
    static Stm32BootClient::ErrorCode flush();
    static void setResetLine( bool _level );
    static void setBootLine( bool _level );
    static void delay( uint32_t _delay );
    static void setSerialBus( Bus _code );
    static int getCurrentBusIdx();
protected:
private:
    static Bus m_bus;
};
#endif

#pragma once
#ifdef __cplusplus
#include "stm32_boot_client.hpp"
#include "included_macro.hpp"
#include <inttypes.h>

class Stm32Io {
public:
    static Stm32BootClient::ErrorCode init();
    static Stm32BootClient::ErrorCode write( const void * _src, size_t _size, size_t * _written = nullptr );
    static Stm32BootClient::ErrorCode read( void * _dst, size_t _size, size_t * _read = nullptr );
    static Stm32BootClient::ErrorCode deinit();
    static void setResetLine( bool _level );
    static void setBootLine( bool _level );
    static void delayMs( uint32_t _delay );
protected:
private:
};
#endif

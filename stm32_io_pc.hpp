#pragma once
#ifdef __cplusplus
#include "stm32_boot_client.hpp"
#include <windows.h>
#include <stdio.h>

#define configASSERT(x) if((x)==0) { std::cout << std::endl << "configASSERT @ " << __FUNCTION__ << ":" << __LINE__ << std::endl; std::cin.get(); }
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof(x[0]))

class Stm32Io {
public:
    static Stm32BootClient::ErrorCode init();
    static Stm32BootClient::ErrorCode write( const void * _src, size_t _size, size_t * _written = nullptr );
    static Stm32BootClient::ErrorCode read( void * _dst, size_t _size, size_t * _read = nullptr );
    static Stm32BootClient::ErrorCode deinit();
    static void setResetLine( bool _level );
    static void setBootLine( bool _level );
    static void delayMs( size_t _delay );
protected:
private:
    static HANDLE m_handle;
};
#endif

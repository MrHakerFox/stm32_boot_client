#pragma once
#ifdef __cplusplus
#include "stm32_boot_client.hpp"
typedef struct Settings_t {
    Stm32BootClient::McuType mcuType;
    Settings_t() :
        mcuType(Stm32BootClient::McuType::Auto) {}
}Settings_t;
int tryDetectMcu( Stm32BootClient::McuType &_mcy );
int main( int argc, char * argv[] );
Settings_t parseCommandLine( int argc, char * argv[] );
#endif

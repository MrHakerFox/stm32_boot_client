#pragma once
#ifdef __cplusplus
#include "stm32_boot_client.hpp"
typedef struct Settings_t {
    Stm32BootClient::McuType mcuType;
    bool program : 1;
    bool read : 1;
    bool erase : 1;
    std::string fname;
    Settings_t()
        : mcuType(Stm32BootClient::McuType::Unknown)
        , program(false)
        , read(false)
        , erase(false) {}
}Settings_t;
int tryDetectMcu( Stm32BootClient::McuType &_mcy );
int main( int argc, char * argv[] );
Settings_t parseCommandLine( int argc, char * argv[] );
#endif

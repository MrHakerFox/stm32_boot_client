#include "stm32bootpc.hpp"
#include <iostream>
Settings_t parseCommandLine( int argc, char * argv[] ) {
    /// TODO Add code
    (void)argc;
    (void)( argv );
    return Settings_t();
}
int tryDetectMcu( Stm32BootClient::McuType &_mcy ) {
    /// TODO Fixe this
    (void)_mcy;
    std::cout << "Try to detect MCU.\n";
    std::cout << "Try to find any MCU at first...";
    Stm32BootClient::ErrorCode err = Stm32BootClient::instance()->checkMcuPresence();
    std::cout << Stm32BootClient::errorCode2String(err) << std::endl;
    if (err != Stm32BootClient::ErrorCode::ACK_OK) {
        std::cout << "No MCU found!" << std::endl;
    } else {
        std::cout << "OK! Some MCU found, try to found out type." << std::endl;
    }
    return ( err == Stm32BootClient::ErrorCode::OK ) ? 0 : -1;
}
int initBootLoader() {
    std::cout << "Initializing bootloader module...";
    Stm32BootClient::ErrorCode err = Stm32BootClient::instance()->init();
    std::cout << Stm32BootClient::errorCode2String(err) << std::endl;
    return ( err == Stm32BootClient::ErrorCode::OK ) ? 0 : -1;
}
int main( int argc, char * argv[] ) {
    Settings_t settings;
    int result = 0;
    std::cout << "STM32F0(1,2,3,4) bootloader client software.\n";

    result = initBootLoader();
    if (result == 0) {
        if (argc > 1) {
            settings = parseCommandLine(argc, argv);
        }
        if (settings.mcuType == Stm32BootClient::McuType::Auto) {
            result = tryDetectMcu(settings.mcuType);
        }
    }
    return result;
}

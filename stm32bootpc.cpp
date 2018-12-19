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
        std::cout << "Get MCU's bootloader info...";
        Stm32BootClient::CommandGetResponse_t getresp;
        err = Stm32BootClient::commandGet(getresp);
        std::cout << Stm32BootClient::errorCode2String(err) << std::endl;
        if (err == Stm32BootClient::ErrorCode::OK) {
            std::cout << "Boot ver.: " << getresp.getBootVerStr() << std::endl;
            std::cout << "List of supported commands: ";
            for ( size_t i = 0; i < getresp.getCommandListSize(); i++ ) {
                std::cout << std::hex << "0x" << static_cast<int>(getresp.getCommand(i)) << " " << std::dec;
            }
            std::cout << std::endl << "Get Chip Id...";
            Stm32BootClient::CommandGetIdResponse_t chipid;
            err = Stm32BootClient::commandGetId(chipid);
            std::cout << Stm32BootClient::errorCode2String(err) << std::endl;
            if (err == Stm32BootClient::ErrorCode::OK) {
                std::cout << "0x" << std::hex << chipid.getId() << std::dec << std::endl;
                Stm32BootClient::McuType mcutype = Stm32BootClient::chipId2McuType(chipid.getId());
                std::cout << "MCU Type: " << Stm32BootClient::mcuType2String(mcutype) << std::endl;
                std::cout << "Read MCU Specs...";
                Stm32BootClient::McuSpecificInfo_t spec;
                err = Stm32BootClient::readMcuSpecificInfo(chipid.getId(), spec);
                std::cout << Stm32BootClient::errorCode2String(err) << std::endl;
                if (err == Stm32BootClient::ErrorCode::OK) {
                    std::cout << "\tFlash size: " << spec.flashSize << " bytes." << std::endl;

                    std::cout << "Try Go...";
                    err = Stm32BootClient::commandGo(0x08000000);
                    std::cout << Stm32BootClient::errorCode2String(err) << std::endl;
                }
            }
        }
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
        if (settings.mcuType == Stm32BootClient::McuType::Unknown) {
            result = tryDetectMcu(settings.mcuType);
        }
    }
    return result;
}

#pragma once
#ifdef __cplusplus
#include <inttypes.h>
#include <string>
class Stm32BootClient {
public:
    enum class McuType : uint8_t {
        Auto = 0,
        Stm32F051 = 1
    };
    enum class ErrorCode : uint8_t {
        OK = 0x00,                  /// No errors
        FAILED = 0x01,              /// General error without clarification
        ACK_OK = 0x02,              /// ACK has been received
        ACK_FAILED = 0x03,          /// ACK has not been received, no MCU connected???
        SERIAL_CANT_OPEN = 0x04,    /// Can't open serial port
    };
    static Stm32BootClient * instance() {
        static Stm32BootClient * __self = new Stm32BootClient;
        return __self;
    }
    static ErrorCode init();
    static ErrorCode checkMcuPresence();
    static std::string errorCode2String( ErrorCode _errcode );
protected:
private:
    static const uint8_t ACK_CODE = 0x7f;
    static const uint8_t ACK_RESP_CODE = 0x79;
    static const size_t BOOT_READY_DELAY = 777;
};
#endif

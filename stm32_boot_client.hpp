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
        DBG_CODE = 0x05,            /// Debug code
    };
    enum class Command : uint8_t {
        Get = 0x00,                 /// Get the version and allowed commands
        GvRps = 0x01,               /// Get Version & Read protection Status command
        Getid = 0x02,               /// Get the chip Id
    };
    typedef struct __packed CommandGetResponse_t {
    public:
        void getBootVer( uint8_t &_high, uint8_t &_low ) {
            _high = static_cast<uint8_t>(bootver >> 4);
            _low = bootver & 0x0f;
        }
        std::string getBootVerStr() {
            uint8_t high, low;
            getBootVer(high, low);
            return std::to_string(high) + "." + std::to_string(low);
        }
        size_t getCommandListSize() const {
            return sizeof( supportedCommands );
        }
        Command getCommand( size_t _idx ) {
            //    configASSERT(_idx < sizeof( supportedCommands ));
            Command result = Command::Get;
            if (_idx < sizeof( supportedCommands )) {
                result = static_cast<Command>(supportedCommands[_idx]);
            }
            return result;
        }
    private:
        uint8_t bytenum;
        uint8_t bootver;
        uint8_t supportedCommands[11];
    }CommandGetResponse_t;
    typedef struct __packed CommandGvRpsResponse_t {
    public:
        void getBootVer( uint8_t &_high, uint8_t &_low ) {
            _high = static_cast<uint8_t>(bootver >> 4);
            _low = bootver & 0x0f;
        }
        std::string getBootVerStr() {
            uint8_t high, low;
            getBootVer(high, low);
            return std::to_string(high) + "." + std::to_string(low);
        }
    protected:
        uint8_t bootver;
        uint8_t opt1;
        uint8_t opt2;
    }CommandGvRpsResponse;
    static Stm32BootClient * instance() {
        static Stm32BootClient * __self = new Stm32BootClient;
        return __self;
    }
    static ErrorCode init();
    static ErrorCode checkMcuPresence();
    static std::string errorCode2String( ErrorCode _errcode );
    static ErrorCode commandGet( CommandGetResponse_t &_resp );
    static ErrorCode commandGvRps( CommandGvRpsResponse_t &_resp );
    static ErrorCode commandGetId( uint16_t &_id );
protected:
private:
    static const uint8_t ACK_ASK_CODE = 0x7f;
    static const uint8_t ACK_RESP_CODE = 0x79;
    static const uint8_t NACK_RESP_CODE = 0x1f;
    static const size_t BOOT_READY_DELAY = 777;

    static void ResetMCU();
};
#endif

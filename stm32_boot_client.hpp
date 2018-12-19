#pragma once
#ifdef __cplusplus
#include <inttypes.h>
#include <string>
class Stm32BootClient {
public:
    typedef struct __packed McuDescription_t {
        uint32_t blRamBegin;
        uint32_t blRamEnd;
        uint32_t blSysMemBegin;
        uint32_t blSysMemEnd;
        uint32_t ramBegin;
        uint32_t ramSize;
        uint32_t flashBegin;
        uint32_t flashSizeReg;
        bool rdpActive2Nack;    /// true if two nacks are sent when RDP is active
    }McuDescription_t;
    enum class McuType : uint8_t {
        Unknown = 0xff,
        Stm32F05xxx_F030x8 = 0x00,
    };
    enum class ErrorCode : uint8_t {
        OK = 0x00,                  /// No errors
        FAILED = 0x01,              /// General error without clarification
        ACK_OK = 0x02,              /// ACK has been received
        ACK_FAILED = 0x03,          /// ACK has not been received, no MCU connected???
        SERIAL_CANT_OPEN = 0x04,    /// Can't open serial port
        DBG_CODE = 0x05,            /// Debug code
        UNKNOWN_MCU = 0x05,         /// Unknown MCU
        SERIAL_WR_SIZE = 0x06,      /// Cant write N bytes
        SERIAL_RD_SIZE = 0x07,      /// Cant read N bytes
    };
    enum class Command : uint8_t {
        Get = 0x00,                 /// Get the version and allowed commands
        GvRps = 0x01,               /// Get Version & Read protection Status command
        Getid = 0x02,               /// Get the chip Id
        ReadMemory = 0x11,          /// Read memory up to 256 bytes
        Go = 0x21,                    /// Execute the downloaded code777
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
    typedef struct __packed CommandGetIdResponse_t {
    public:
        uint16_t getId() const {
            return static_cast<uint16_t>(( pid1 << 8 ) | pid2);
        }
    private:
        uint8_t numbytes;
        uint8_t pid1;
        uint8_t pid2;
    }CommandGetIdResponse_t;
    typedef struct __packed McuSpecificInfo_t {
        uint32_t flashSize;     /// in bytes
    }McuSpecificInfo_t;
    static Stm32BootClient * instance() {
        static Stm32BootClient * __self = new Stm32BootClient;
        return __self;
    }
    static ErrorCode init();
    static ErrorCode checkMcuPresence();
    static std::string errorCode2String( ErrorCode _errcode );
    static std::string mcuType2String( McuType _type );
    static McuType chipId2McuType( uint16_t _chipid );
    static McuDescription_t mcuType2Description( McuType _type );
    static ErrorCode commandGet( CommandGetResponse_t &_resp );
    static ErrorCode commandGvRps( CommandGvRpsResponse_t &_resp );
    static ErrorCode commandGetId( CommandGetIdResponse_t &_resp );
    static ErrorCode commandReadMemory( void * _dst, uint32_t _addr, size_t _size );
    static ErrorCode commandGo( uint32_t _addr );
    static ErrorCode readMcuSpecificInfo( uint16_t _chipid, McuSpecificInfo_t &_info );
protected:
private:
    static const McuDescription_t m_mcuDescription[];
    static const uint8_t ACK_ASK_CODE = 0x7f;
    static const uint8_t ACK_RESP_CODE = 0x79;
    static const uint8_t NACK_RESP_CODE = 0x1f;
    static const size_t BOOT_READY_DELAY = 777;

    static void ResetMCU();
    static uint8_t calculateXor( const uint8_t * _src, size_t _size );
    static ErrorCode commandGenericSend( Command _cmd );
    static ErrorCode genericSendAddr( uint32_t _addr );
    static void addr32_to_byte( uint32_t _addr, uint8_t * _array );
};
#endif

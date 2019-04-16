/*
 * @author: GWX
 * @date: 20181229
 * @descirption: TS数据格式定义
 * @history:
 */

#ifndef __XXtsType_h
#define __XXtsType_h

#include <stdbool.h>
#include <stdint.h>

// Header
typedef struct XXtsHeader
{
    uint8_t syncByte;
    bool transportErrorIndicator;
    bool payloadUnitStartIndicator;
    bool transportPriority;

    uint16_t pid;
    uint8_t transportScramblingControl;
    uint8_t adaptationFieldControl;
    uint8_t continuityCounter;

    // 自适应段
    uint8_t adaptationFieldLength;
    bool discontinuityIndicator;
    bool randomAccessIndicator;
    bool elementaryStreamPriorityIndicator;
    bool pcrFlag;

    bool opcrFlag;
    bool splicingPointFlag;
    bool transportPrivateDataFlag;
    bool adaptationFieldExtansionFlag;

    // PCR_flag == 1
    uint64_t programClockReferenceBase;
    uint8_t reserved1;
    uint16_t programClockReferenceExtension;

    // OPCR_flag == 1
    uint64_t originalProgramClockReferenceBase;
    uint8_t reserved2;
    uint16_t originalProgramClockReferenceExtension;

    // splicing_point_flag == 1
    uint8_t spliceCountdown;

    // transport_private_data_flag == 1
    uint8_t transportPrivateDataLength;
    char transportPrivateData[188];

    // adaptation_field_extension_flag == 1
    uint8_t adaptationFieldExtensionLength;
    bool ltwFlag;
    bool piecewiseRateFlag;
    bool seamlessSpliceFlag;
    uint8_t reserved3;

    // ltw_flag == 1
    bool ltwValidFlag;
    uint16_t ltwOffset;

    // piecewise_rate_flag == 1
    uint8_t reserved4;
    uint32_t piecewiseRate;

    // seamless_splice_flag
    uint8_t spliceType;
    uint64_t dtsNextAu;
}XXtsHeader;

// PAT的节目条目
typedef struct XXtsPATItem
{
    uint16_t number;
    uint8_t reserved;
    uint16_t pid;
}XXtsPATItem;

// PAT
typedef struct XXtsPAT
{
    uint8_t tableId;
    bool sectionSyntaxIndicator;
    bool zero;
    uint8_t reserved1;
    uint16_t sectionLength;

    uint16_t transportStreamId;
    uint8_t reserved2;
    uint8_t versionNumber;
    bool currentNextIndicator;

    uint8_t sectionNumber;
    uint8_t lastSectionNumber;

    uint16_t networkPid;

    //XXtsPATItem patItem;
    uint32_t crc32;
}XXtsPAT;

// PMT条目信息
typedef struct XXtsPMTItem
{
    uint8_t streamType;
    uint8_t reserved1;
    uint16_t elementaryPid;
    uint8_t reserved2;
    uint16_t esInfoLength;
}XXtsPMTItem;

// PMT
typedef struct XXtsPMT
{
    uint8_t tableId;
    bool sectionSyctaxIndicator;
    bool zero;
    uint8_t reserved1;
    uint16_t sectionLength;

    uint16_t programNumber;
    uint8_t reserved2;
    uint8_t versionNumber;
    bool currentNextIndicator;

    uint8_t sectionNumber;
    uint8_t lastSectionNumber;

    uint8_t reserved3;
    uint16_t pcrPid;
    uint8_t reserved4;
    uint16_t programInfoLength;

    //XXtsPMTItem* pmtItems[10];
    uint32_t crc32;
}XXtsPMT;

// DSM Trick mode
typedef struct FastForward
{
    uint8_t fieldId;
    uint8_t intraSliceRefresh;
    uint8_t frquencyTruncation;
}FastForward;
typedef struct SlowMotion
{
    uint8_t repCntrl;
}SlowMotion;
typedef struct FreezeFrame
{
    uint8_t fieldId;
    uint8_t reserved;
}FreezeFrame;
typedef struct FastReverse
{
    uint8_t fieldId;
    uint8_t intraSliceRefresh;
    uint8_t frquencyTruncation;
}FastReverse;
typedef struct SlowReverse
{
    uint8_t repCntrl;
}SlowReverse;
typedef union DSMTrickModeControlUnion
{
    FastForward fastForward;
    SlowMotion slowMotion;
    FreezeFrame freezeFrame;
    FastReverse fastReverse;
    SlowReverse slowReverse;
}DSMTrickModeControlUnion;

// PES define
typedef enum PESStreamType
{
    Unknown = 0,
    ProgramStreamMap = 1,
    PrivateStream1,
    PaddingStream,
    PrivateStream2,

    AudioStream,
    VideoStream,
    ECMStream,
    EMMStream,

    H2220_AnnexA_DSMCC_Stream,
    ISO_13522_Stream,
    H2221_TypeA,
    H2221_TypeB,

    H2221_TypeC,
    H2221_TypeD,
    H2221_TypeE,
    AncillaryStream,

    ReservedDataStream,
    ProgramStreamDirectory,
}PESStreamType;
PESStreamType pesStreamType(uint8_t streamId)
{
    if (0xBC == streamId)         return ProgramStreamMap;
    else if (0xBD == streamId)     return PrivateStream1;
    else if (0xBE == streamId)    return PaddingStream;
    else if (0xBF == streamId)    return PrivateStream2;
    //
    else if (0x60 <= streamId && streamId <= 0xDF)  return AudioStream;
    else if (0xE0 <= streamId && streamId <= 0xEF)  return VideoStream;
    else if (0xF0 == streamId)                return ECMStream;
    else if (0xF1 == streamId)                return EMMStream;
    //
    else if (0xF2 == streamId)    return H2220_AnnexA_DSMCC_Stream;
    else if (0xF3 == streamId)    return ISO_13522_Stream;
    else if (0xF4 == streamId)    return H2221_TypeA;
    else if (0xF5 == streamId)    return H2221_TypeB;
    //
    else if (0xF6 == streamId)    return H2221_TypeB;
    else if (0xF7 == streamId)    return H2221_TypeC;
    else if (0xF8 == streamId)    return H2221_TypeD;
    else if (0xF9 == streamId)    return AncillaryStream;
    // 
    else if (0xF5 <= streamId && streamId <= 0xFE)  return ReservedDataStream;
    else if (0xFF == streamId)                return ProgramStreamDirectory;
    else                                return Unknown;
}
typedef enum PES_DSM_TrickType
{
    FastForwardType = 1,
    SlowMotionType,
    FreezeFrameType,
    FastReverseType,
    SlowReverseType,
    ReservedType,
}PES_DSM_TrickType;
PES_DSM_TrickType dsmTrickType(unsigned char mode)
{
    if (0x00 == mode)       return FastForwardType;
    else if (0x01 == mode)  return SlowMotionType;
    else if (0x02 == mode)  return FreezeFrameType;
    else if (0x03 == mode)  return FastReverseType;
    else if (0x04 == mode)  return SlowReverseType;
    else                    return ReservedType;
}

// PES Header
typedef struct XXtsPESHeader
{
    uint32_t startCode;
    uint8_t streamId;
    uint16_t pesPacketLength;

    /*
    Stream_id != program_stream_map
    && Stream_id != padding_stream
    && Stream_id != private_stream_2
    && Stream_id != ECM
    && Stream_id != EMM
    && Stream_id != program_stream_directory
    && Stream_id != DSMCC_stream
    && stream_id != ITU-T Rec. H.222.1 type E_stream
    */
        uint8_t two;
        uint8_t pesScramblingControl;
        bool pesPriority;
        bool dataAlignmentIndicator;
        bool copyright;
        bool originalOrCopy;

        uint8_t ptsDtsFlags;
        bool escrFlag;
        bool esRateFlag;
        bool dsmTrickModeFlag;
        bool additionalCopyInfoFlag;
        bool pesCrcFlag;
        bool pesExtensionFlag;
        uint8_t pesHeaderDataLength;

        // PTS_DTS_flags
        uint64_t pts;
        uint64_t dts;

        // ESCR_flag
        uint64_t escrBase;
        uint16_t escrExtension;

        // ES_rate_flag
        uint32_t esRate;

        // DSM_trick_mode_flag
        uint8_t trickModeControl;
        DSMTrickModeControlUnion dsmTrick;

        // Additional_copy_info_flag
        uint8_t additionalCopyInfo;

        // PES_CRC_flag
        uint16_t previousCrc;

        // PES_extension_flag
        bool pesPrivateDataFlag;
        bool packHeaderFieldFlag;
        bool programPacketSequenceCounterFlag;
        bool pstdBufferFlag;
        uint8_t reserved;
        bool pesExtensionFlag2;

            // PES_private_data_flag == 1
            int8_t pesPrivateData[16];

            // pack_header_field_flag
            uint8_t packFieldLength;
            int8_t packHeader[188];

            // program_packet_sequence_counter_flag
            uint8_t programPacketSequenceCounter;
            bool mpeg1Mpeg2Identifier;
            uint8_t originalStuffLength;

            // P-STD_buffer_flag == 1
            bool pstdBufferScale;
            bool pstdBufferSize;

            // PES_entension_flag_2
            uint8_t pesExtensionFieldLength;

        int8_t stuffingByte[188];
    /*
    stream_id == program_stream_map
    || stream_id == private_stream_2
    || stream_id == ECM
    || stream_id == EMM
    || stream_id == program_stream_directory
    || stream_id == DSMCC_stream
    || stream_id == ITU-T Rec. H.222.1 type E stream
    */
        int8_t pesPacketDataByte[188];
        uint8_t pesPacketDataByteLength;

     /* stream_id == padding_stream */
        int8_t paddingByte[188];
}XXtsPESHeader;
#endif

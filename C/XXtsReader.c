#include "XXtsReader.h"
#include "XXtsType.h"
#include "XXbyteOrder.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TS_PACKET_LENGTH    188
#define TS_MAP_ITEM_MAX     32
#define TS_LOG_BUFFER_MAX   1024
#define TS_DATA_VIDEO_BUFFER_MAX    256*1024
#define TS_DATA_AUDIO_BUFFER_MAX    32*1024

// handle对应的数据
typedef struct XXtsData
{
    bool isFoundPAT;    // 是否找到PAT
    int16_t pmtPid;     // PMT的PID,>0可用
    int16_t videoPid;   // 视频数据的PID,>0可用
    int16_t audioPid;   // 音频数据的PID,>0可用
    //
    XXavFrameType videoType;        // 视频类型
    uint32_t videoBufferLength;     // buffer中的数据长度
    char *videoBuffer;              // 帧buffer
    uint64_t videoBufferPts;        // buffer中的数据对应的pts
    //
    XXavFrameType audioType;        // 音频类型
    uint32_t audioBufferLength;
    char *audioBuffer;
    uint64_t audioBufferPts;
    //
    XXtsPATItem* patItem[TS_MAP_ITEM_MAX];  // PAT条目
    XXtsPMTItem* pmtItem[TS_MAP_ITEM_MAX];  // PMT条目
    //
    XXtsReaderFrameCallBack frameCallBack;  // 帧数据获取回调
    XXtsReaderLogCallBack logCallBack;      // 信息输出回调
    void *opaque;                           // 自定义数据,作为帧数据获取回调的参数
}XXtsData, *XXtsDataPtr;

// *私有函数声明
static void addVideoData(XXtsDataPtr tsData, const char *data, uint8_t length);
static void addAudioData(XXtsDataPtr tsData, const char *data, uint8_t length);
static int headerFromData(XXtsHeader *header, const char *data);
static bool patFromData(XXtsPAT *pat, const char *data, uint8_t length, XXtsPATItem **item);
static bool pmtFromData(XXtsPMT *pmt, const char *data, uint8_t length, XXtsPMTItem **item);
static bool pesHeaderFromData(XXtsPESHeader *pesHeader, const char *data, uint8_t length);
static void makeLog(XXtsDataPtr dataPtr, char *format, ...);
static void freeItem(XXtsDataPtr dataPtr);
static void resetHandleData(XXtsDataPtr dataPtr);
static uint8_t adjustOffsetForPSI(XXtsHeader *header);
// 私有函数声明*

// 公开函数 - 创建句柄
XXtsHandle xxts_reader_create(void *opaque, XXtsReaderFrameCallBack frameCallBack, XXtsReaderLogCallBack logCallBack)
{
    if(NULL == frameCallBack)
        return NULL;
    
    XXtsDataPtr handle  = (XXtsDataPtr)malloc(sizeof(XXtsData));
    if(NULL == handle)
        return NULL;
    memset(handle, 0, sizeof(XXtsData));
    
    handle->videoBuffer     = (char*)malloc(TS_DATA_VIDEO_BUFFER_MAX);
    memset(handle->videoBuffer, 0, TS_DATA_VIDEO_BUFFER_MAX);
    
    handle->audioBuffer = (char*)malloc(TS_DATA_AUDIO_BUFFER_MAX);
    memset(handle->audioBuffer, 0, TS_DATA_AUDIO_BUFFER_MAX);
    
    handle->frameCallBack   = frameCallBack;
    handle->logCallBack     = logCallBack;
    handle->opaque          = opaque;
    return handle;
}
// 公开函数 - 添加数据
void xxts_reader_push(XXtsHandle handle, const char *data, int length)
{
    if (NULL == handle || NULL == data || length < TS_PACKET_LENGTH)
        return;
    
    const char *dataOffset = data;
    XXtsHeader header;
    uint8_t headerOffset;
    XXtsDataPtr tsData = (XXtsDataPtr)handle;
    
    while (length >= TS_PACKET_LENGTH)
    {
        memset(&header, 0, sizeof(XXtsHeader));
        headerOffset = headerFromData(&header, dataOffset);
        
        // PAT
        if (header.pid == 0)
        {
            //resetHandleData(tsData);
            headerOffset += adjustOffsetForPSI(&header);
            
            XXtsPAT pat;
            patFromData(&pat, dataOffset + headerOffset, TS_PACKET_LENGTH - headerOffset, tsData->patItem);
            
            if (NULL == tsData->patItem[0])
            {
                makeLog(tsData, "can not find PMT.");
            }
            else
            {
                tsData->isFoundPAT = true;
                tsData->pmtPid = tsData->patItem[0]->pid;
                makeLog(tsData, "PMT is found.");
            }
        }
        // PMT
        else if (tsData->pmtPid > 0 && header.pid == tsData->pmtPid)
        {
            headerOffset += adjustOffsetForPSI(&header);
            
            XXtsPMT pmt;
            pmtFromData(&pmt, dataOffset + headerOffset, TS_PACKET_LENGTH - headerOffset, tsData->pmtItem);
            
            for (uint8_t index = 0; index < TS_MAP_ITEM_MAX && NULL != tsData->pmtItem[index]; index++)
            {
                switch (tsData->pmtItem[index]->streamType) {
                    case 0x0f:
                        tsData->audioPid = tsData->pmtItem[index]->elementaryPid;
                        tsData->audioType = XXavFrameTypeAAC;
                        break;
                    case 0x1b:
                        tsData->videoPid = tsData->pmtItem[index]->elementaryPid;
                        tsData->videoType = XXavFrameTypeH264;
                        break;
                    case 0x24:
                        tsData->videoPid = tsData->pmtItem[index]->elementaryPid;
                        tsData->videoType = XXavFrameTypeH265;
                        break;
                    default:
                        makeLog(tsData, "unknown stream type.");
                        break;
                }
            }
            
            if (tsData->audioPid <= 0 || tsData->videoPid <= 0)
                makeLog(tsData, "lack of audio or video stream.");
            else
                makeLog(tsData, "AudioPID=%d VideoPID=%d.", tsData->audioPid, tsData->videoPid);
        }
        // Video + Audio
        else if ((tsData->videoPid > 0 && header.pid == tsData->videoPid)
                 || (tsData->audioPid > 0 && header.pid == tsData->audioPid))
        {
            if (header.payloadUnitStartIndicator)
            {
                if (NULL != tsData->frameCallBack)
                {
                    XXtsReaderFrame frame = {
                        .data       = header.pid == tsData->videoPid ? tsData->videoBuffer : tsData->audioBuffer,
                        .length     = header.pid == tsData->videoPid ? tsData->videoBufferLength : tsData->audioBufferLength,
                        .pts        = header.pid == tsData->videoPid ? tsData->videoBufferPts : tsData->audioBufferPts,
                        .isVideo    = header.pid == tsData->videoPid,
                        .isKey      = false,
                        .type       = header.pid == tsData->videoPid ? tsData->videoType : tsData->audioType,
                    };
                    if (frame.length > 0)
                        tsData->frameCallBack(tsData->userData, frame);
                }
                
                XXtsPESHeader pesHeader;
                pesHeaderFromData(&pesHeader, dataOffset + headerOffset, TS_PACKET_LENGTH - headerOffset);
                if (header.pid == tsData->videoPid)
                {
                    addVideoData(tsData, NULL, 0);
                    addVideoData(tsData, pesHeader.pesPacketDataByte, pesHeader.pesPacketDataByteLength);
                    tsData->videoBufferPts = pesHeader.pts * 1000 / 90000;
                }
                else
                {
                    addAudioData(tsData, NULL, 0);
                    addAudioData(tsData, pesHeader.pesPacketDataByte, pesHeader.pesPacketDataByteLength);
                    tsData->audioBufferPts = pesHeader.pts * 1000 / 90000;
                }
            }
            else
            {
                if (header.pid == tsData->videoPid)
                {
                    addVideoData(tsData, dataOffset + headerOffset, TS_PACKET_LENGTH - headerOffset);
                }
                else
                {
                    addAudioData(tsData, dataOffset + headerOffset, TS_PACKET_LENGTH - headerOffset);
                }
            }
        }
        else
        {
            makeLog(tsData, "unknown pid:%d", header.pid);
        }
        
        length -= TS_PACKET_LENGTH;
        dataOffset += TS_PACKET_LENGTH;
    }
}
// 公开函数 - 释放句柄对应的数据空间,并设置其为NULL
void xxts_reader_free(XXtsHandle *handle)
{
    if (NULL == handle || NULL == *handle)
        return;
    XXtsDataPtr data = (XXtsDataPtr)(*handle);
    freeItem(data);
    free(data->audioBuffer);
    free(data->videoBuffer);
    free(data);
    *handle = NULL;
}

// 私有函数 - 向handle中添加视频帧数据
static void addVideoData(XXtsDataPtr tsData, const char *data, uint8_t length)
{
    if (NULL == data)
    {
        memset(tsData->videoBuffer, 0, tsData->videoBufferLength);
        tsData->videoBufferLength = 0;
    }
    else
    {
        if (length + tsData->videoBufferLength > TS_DATA_VIDEO_BUFFER_MAX)
        {
            makeLog(tsData, "ts video data buffer will be overflow. (%d -> %d)", tsData->videoBufferLength, length + tsData->videoBufferLength);
            return;
        }
        memcpy(tsData->videoBuffer + tsData->videoBufferLength, data, length);
        tsData->videoBufferLength += length;
    }
}
// 私有函数 - 向handle中添加视频帧数据
static void addAudioData(XXtsDataPtr tsData, const char *data, uint8_t length)
{
    if (NULL == data)
    {
        memset(tsData->audioBuffer, 0, tsData->audioBufferLength);
        tsData->audioBufferLength = 0;
    }
    else
    {
        if (length + tsData->audioBufferLength > TS_DATA_AUDIO_BUFFER_MAX)
        {
            makeLog(tsData, "ts audio data buffer will be overflow. (%d -> %d)", tsData->audioBufferLength, length + tsData->audioBufferLength);
            return;
        }
        memcpy(tsData->audioBuffer + tsData->audioBufferLength, data, length);
        tsData->audioBufferLength += length;
    }
}
// 私有函数 - 解析ts头
static int headerFromData(XXtsHeader *header, const char *data)
{
    const char *offset = data;
    const char *adapationOffset = NULL;
    
    header->syncByte = *offset;
    offset += 1;
    
    uint16_t u16Tmp = beToU16(offset);
    offset += 2;
    header->transportErrorIndicator = (u16Tmp >> 15);
    header->payloadUnitStartIndicator = (u16Tmp >> 14) & 0x0001;
    header->transportPriority = (u16Tmp >> 13) & 0x0001;
    header->pid = u16Tmp & 0x1fff;
    
    uint8_t u8Tmp = *offset;
    offset += 1;
    header->transportScramblingControl = u8Tmp >> 6;
    header->adaptationFieldControl = (u8Tmp >> 4) & 0x03;
    header->continuityCounter = u8Tmp & 0x0f;
    
    header->adaptationFieldLength = 0;
    if (header->adaptationFieldControl & 0x02)
    {
        header->adaptationFieldLength = *offset;
        offset += 1;
        adapationOffset = offset;
    }
    
    if (header->adaptationFieldLength > 0)
    {
        u8Tmp = *offset;
        offset += 1;
        header->discontinuityIndicator = u8Tmp >> 7;
        header->randomAccessIndicator = (u8Tmp >> 6) & 0x01;
        header->elementaryStreamPriorityIndicator = (u8Tmp >> 5) & 0x01;
        header->pcrFlag = (u8Tmp >> 4) & 0x01;
        
        header->opcrFlag = (u8Tmp >> 3) & 0x01;
        header->splicingPointFlag = (u8Tmp >> 2) & 0x01;
        header->transportPrivateDataFlag = (u8Tmp >> 1) & 0x01;
        header->adaptationFieldExtansionFlag = u8Tmp & 0x01;
        
        // pcr
        if (header->pcrFlag)
        {
            uint64_t u64Tmp = beToU32(offset);
            offset += 4;
            u16Tmp = beToU16(offset);
            offset += 2;
            header->programClockReferenceBase = (u64Tmp << 1) | ((uint64_t)(u16Tmp >> 15));
            header->reserved1 = (u16Tmp >> 9) & 0x003f;
            header->programClockReferenceExtension = u16Tmp & 0x01ff;
        }
        
        // opcr
        if (header->opcrFlag)
        {
            uint64_t u64Tmp = beToU32(offset);
            offset += 4;
            u16Tmp = beToU16(offset);
            offset += 2;
            header->originalProgramClockReferenceBase = (u64Tmp << 1) | ((uint64_t)(u16Tmp >> 15));
            header->reserved2 = (u16Tmp >> 9) & 0x003f;
            header->originalProgramClockReferenceExtension = u16Tmp & 0x01ff;
        }
        
        // splicing_point
        if (header->splicingPointFlag)
        {
            header->spliceCountdown = *offset;
            offset += 1;
        }
        
        // transport_private_data
        if (header->transportPrivateDataFlag)
        {
            header->transportPrivateDataLength = *offset;
            offset += 1;
            if (header->transportPrivateDataLength > 0)
            {
                memcpy(header->transportPrivateData, offset, header->transportPrivateDataLength);
                offset += header->transportPrivateDataLength;
            }
        }
        
        // adaotaion_field_extension
        if (header->adaptationFieldExtansionFlag)
        {
            header->adaptationFieldExtensionLength = *offset;
            offset += 1;
            
            u8Tmp = *offset;
            offset += 1;
            header->ltwFlag = u8Tmp >> 7;
            header->piecewiseRateFlag = (u8Tmp >> 6) & 0x01;
            header->seamlessSpliceFlag = (u8Tmp >> 5) & 0x01;
            header->reserved3 = u8Tmp & 0x1f;
            
            if (header->ltwFlag)
            {
                u16Tmp = beToU16(offset);
                offset += 2;
                header->ltwValidFlag = u16Tmp >> 15;
                header->ltwOffset = u16Tmp & 0x7f;
            }
            
            if (header->piecewiseRateFlag)
            {
                uint32_t u32Tmp = beToU32(offset);
                offset += 3;
                header->reserved4 = u32Tmp >> 30;
                header->piecewiseRate = (u32Tmp >> 8) & 0x003fffff;
            }
            
            if (header->seamlessSpliceFlag)
            {
                u8Tmp = *offset;
                offset += 1;
                header->spliceType = u8Tmp >> 4;
                header->dtsNextAu = 0;
                header->dtsNextAu = ((uint64_t)((u8Tmp << 4) & 0xe0)) << 25;
                
                u16Tmp = beToU16(offset);
                offset += 2;
                header->dtsNextAu |= (uint64_t)(u16Tmp & 0xfffe) << 14;
                
                u16Tmp = beToU16(offset);
                offset += 2;
                header->dtsNextAu |= (uint64_t)(u16Tmp & 0xfffe) >> 1;
            }
        }
        offset = adapationOffset + header->adaptationFieldLength;
    }
    return (int)(offset - data);
}
// 私有函数 - 解析pat表
static bool patFromData(XXtsPAT *pat, const char *data, uint8_t length, XXtsPATItem **item)
{
    const char *offset = data;
    
    pat->tableId = *offset;
    offset += 1;
    
    pat->sectionSyntaxIndicator = *offset >> 7;
    pat->zero = (*offset >> 6) & 0x01;
    pat->reserved1 = (*offset >> 4) & 0x03;
    
    pat->sectionLength = beToU16(offset);
    offset += 2;
    pat->sectionLength &= 0x0fff;
    
    pat->transportStreamId = beToU16(offset);  offset += 2;
    
    uint8_t u8Tmp = *offset;
    offset += 1;
    pat->reserved2 = u8Tmp >> 6;
    pat->versionNumber = (u8Tmp >> 1) & 0x1f;
    pat->currentNextIndicator = u8Tmp & 0x01;
    
    pat->sectionNumber = *offset;
    offset += 1;
    
    pat->lastSectionNumber = *offset;
    offset += 1;
    
    // 计算有多少个节目信息
    int count = (pat->sectionLength - 5 - 4) / 4;
    int itemIndex = 0;
    for (int i = 0; i < count; i++)
    {
        uint16_t programNumber = beToU16(offset);
        offset += 2;
        
        uint16_t pid = beToU16(offset);
        offset += 2;
        
        if (0 == programNumber)
        {
            pat->networkPid = pid & 0x1fff;
        }
        else
        {
            XXtsPATItem *patItem = (XXtsPATItem*)malloc(sizeof(XXtsPATItem));
            patItem->number = programNumber;
            patItem->reserved = pid >> 13;
            patItem->pid = pid & 0x1fff;
            
            if (NULL != item[itemIndex])
                free(item[itemIndex]);
            item[itemIndex] = patItem;
            itemIndex++;
        }
    }
    
    pat->crc32 = beToU32(offset);
    offset += 4;
    return true;
}
// 私有函数 - 解析pmt表
static bool pmtFromData(XXtsPMT *pmt, const char *data, uint8_t length, XXtsPMTItem **item)
{
    const char *offset = data;
    
    pmt->tableId = *offset;
    offset += 1;
    
    pmt->sectionSyctaxIndicator = *offset >> 7;
    pmt->zero = (*offset >> 6) & 0x01;
    pmt->reserved1 = (*offset >> 4) & 0x03;
    
    pmt->sectionLength = beToU16(offset);
    offset += 2;
    pmt->sectionLength &= 0x0fff;
    
    const char *start = offset;
    
    pmt->programNumber = beToU16(offset);
    offset += 2;
    
    uint8_t u8Tmp = *offset;
    offset += 1;
    pmt->reserved2 = u8Tmp >> 6;
    pmt->versionNumber = (u8Tmp >> 1) & 0x1f;
    pmt->currentNextIndicator = u8Tmp & 0x01;
    
    pmt->sectionNumber = *offset;
    offset += 1;
    pmt->lastSectionNumber = *offset;
    offset += 1;
    
    uint16_t u16Tmp = beToU16(offset);
    offset += 2;
    pmt->reserved3 = u16Tmp >> 13;
    pmt->pcrPid = u16Tmp & 0x1fff;
    
    u16Tmp = beToU16(offset);
    offset += 2;
    pmt->reserved4 = u16Tmp >> 12;
    pmt->programInfoLength = u16Tmp & 0x0fff;
    
    /*
     for()
     descriptor
     */
    offset += pmt->programInfoLength;
    
    int i = 0;
    while (offset < start + pmt->sectionLength - 4)
    {
        XXtsPMTItem *pmtItem = (XXtsPMTItem*)malloc(sizeof(XXtsPMTItem));
        pmtItem->streamType = *offset;
        offset += 1;
        
        u16Tmp = beToU16(offset);
        offset += 2;
        pmtItem->reserved1 = u16Tmp >> 13;
        pmtItem->elementaryPid = u16Tmp & 0x1fff;
        
        u16Tmp = beToU16(offset);
        offset += 2;
        pmtItem->reserved2 = u16Tmp >> 13;
        pmtItem->esInfoLength = u16Tmp & 0x0fff;
        
        if (NULL != item[i])
            free(item[i]);
        item[i] = pmtItem;
        i++;
        offset += pmtItem->esInfoLength;
    }
    
    pmt->crc32 = beToU32(offset);
    offset += 4;
    return true;
}
// 私有函数 - 解析pes头数据
static bool pesHeaderFromData(XXtsPESHeader *pesHeader, const char *data, uint8_t length)
{
    const char *offset = data;
    pesHeader->startCode = beToU32(offset) >> 8;
    offset += 3;
    pesHeader->streamId = *offset;
    offset += 1;
    pesHeader->pesPacketLength = beToU16(offset);
    offset += 2;
    
    PESStreamType type = pesStreamType(pesHeader->streamId);
    if (ProgramStreamMap != type
        && PaddingStream != type
        && PrivateStream2 != type
        && ECMStream != type
        && EMMStream != type
        && ProgramStreamDirectory != type
        && H2220_AnnexA_DSMCC_Stream != type
        && H2221_TypeE != type)
    {
        uint8_t u8Tmp = *offset;
        offset += 1;
        pesHeader->two = u8Tmp >> 6;
        pesHeader->pesScramblingControl = (u8Tmp >> 4) & 0x03;
        pesHeader->pesPriority = (u8Tmp >> 3) & 0x01;
        pesHeader->dataAlignmentIndicator = (u8Tmp >> 2) & 0x01;
        pesHeader->copyright = (u8Tmp >> 1) & 0x01;
        pesHeader->originalOrCopy = u8Tmp & 0x01;
        
        u8Tmp = *offset;
        offset += 1;
        pesHeader->ptsDtsFlags = (u8Tmp >> 6) & 0x03;
        pesHeader->escrFlag = (u8Tmp >> 5) & 0x01;
        pesHeader->esRateFlag = (u8Tmp >> 4) & 0x01;
        pesHeader->dsmTrickModeFlag = (u8Tmp >> 3) & 0x01;
        pesHeader->additionalCopyInfoFlag = (u8Tmp >> 2) & 0x01;
        pesHeader->pesCrcFlag = (u8Tmp >> 1) & 0x01;
        pesHeader->pesExtensionFlag = u8Tmp & 0x01;
        
        pesHeader->pesHeaderDataLength = *offset;
        offset += 1;
        
        // PTS_DTS_flags
        uint16_t u16Tmp;
        pesHeader->pts = 0;
        pesHeader->dts = 0;
        if (0x02 == pesHeader->ptsDtsFlags)
        {
            u8Tmp = *offset;
            offset += 1;
            pesHeader->pts = (uint64_t)((u8Tmp << 4) & 0xe0) << 25;
            
            u16Tmp = beToU16(offset);
            offset += 2;
            pesHeader->pts |= (uint64_t)(u16Tmp & 0xfffe) << 14;
            
            u16Tmp = beToU16(offset);
            offset += 2;
            pesHeader->pts |= (uint64_t)(u16Tmp & 0xfffe) >> 1;
        }
        else if (0x03 == pesHeader->ptsDtsFlags)
        {
            u8Tmp = *offset;
            offset += 1;
            pesHeader->pts = (uint64_t)((u8Tmp << 4) & 0xe0) << 25;
            
            u16Tmp = beToU16(offset);
            offset += 2;
            pesHeader->pts |= (uint64_t)(u16Tmp & 0xfffe) << 14;
            
            u16Tmp = beToU16(offset);
            offset += 2;
            pesHeader->pts |= (uint64_t)(u16Tmp & 0xfffe) >> 1;
            
            u8Tmp = *offset;
            offset += 1;
            pesHeader->dts = (uint64_t)((u8Tmp << 4) & 0xe0) << 25;
            
            u16Tmp = beToU16(offset);
            offset += 2;
            pesHeader->dts |= (uint64_t)(u16Tmp & 0xfffe) << 14;
            
            u16Tmp = beToU16(offset);
            offset += 2;
            pesHeader->dts |= (uint64_t)(u16Tmp & 0xfffe) >> 1;
        }
        else {}
        
        // ESCR_flag
        pesHeader->escrBase = 0;
        pesHeader->escrExtension = 0;
        if (pesHeader->escrFlag)
        {
            u8Tmp = *offset;
            offset += 1;
            pesHeader->escrBase = (uint64_t)((u8Tmp << 2) & 0xe0) << 25; // 3bit
            pesHeader->escrBase |= (uint64_t)(u8Tmp << 6) << 22; // 2bit
            
            u16Tmp = beToU16(offset);
            offset += 2;
            pesHeader->escrBase |= (uint64_t)(u16Tmp & 0xffe0) << 12; // 13bit
            pesHeader->escrBase |= (uint64_t)((u16Tmp << 13) & 0x6000); // 2bit
            
            u16Tmp = beToU16(offset);
            offset += 2;
            pesHeader->escrBase |= (uint64_t)(u16Tmp >> 3);
            
            u8Tmp = *offset;
            offset += 1;
            pesHeader->escrExtension = u16Tmp << 8;
            pesHeader->escrExtension |= (uint16_t)(u8Tmp >> 1);
        }
        
        // ES_rate_flag
        pesHeader->esRate = 0;
        if (pesHeader->esRateFlag)
        {
            uint32_t u32Tmp = beToU32(offset);
            offset += 3;
            pesHeader->esRate = (u32Tmp >> 9) & 0x003FFFFF;
        }
        
        // DSM_trick_mode_flag
        pesHeader->trickModeControl = 0;
        if (pesHeader->dsmTrickModeFlag)
        {
            u8Tmp = *offset;
            offset += 1;
            pesHeader->trickModeControl = u8Tmp >> 5;
            PES_DSM_TrickType trickType = dsmTrickType(pesHeader->trickModeControl);
            switch (trickType)
            {
                case FastForwardType:
                    pesHeader->dsmTrick.fastForward.fieldId = (u8Tmp >> 3) & 0x03;
                    pesHeader->dsmTrick.fastForward.intraSliceRefresh = (u8Tmp >> 2) & 0x01;
                    pesHeader->dsmTrick.fastForward.frquencyTruncation = u8Tmp & 0x03;
                    break;
                case SlowMotionType:
                    pesHeader->dsmTrick.slowMotion.repCntrl = u8Tmp & 0x1f;
                    break;
                case FreezeFrameType:
                    pesHeader->dsmTrick.freezeFrame.fieldId = (u8Tmp >> 3) & 0x03;
                    pesHeader->dsmTrick.freezeFrame.reserved = u8Tmp & 0x07;
                    break;
                case FastReverseType:
                    pesHeader->dsmTrick.fastReverse.fieldId = (u8Tmp >> 3) & 0x03;
                    pesHeader->dsmTrick.fastReverse.intraSliceRefresh = (u8Tmp >> 2) & 0x01;
                    pesHeader->dsmTrick.fastReverse.frquencyTruncation = u8Tmp & 0x03;
                    break;
                case SlowReverseType:
                    pesHeader->dsmTrick.slowReverse.repCntrl = u8Tmp & 0x1f;
                    break;
                case ReservedType:
                    break;
                default:
                    break;
            }
        }
        
        // additional_copy_info_flag
        pesHeader->additionalCopyInfo = 0;
        if (pesHeader->additionalCopyInfoFlag)
        {
            pesHeader->additionalCopyInfo = *offset & 0x7f;
            offset += 1;
        }
        
        // PES_CRC_flag
        pesHeader->previousCrc = 0;
        if (pesHeader->pesCrcFlag)
        {
            pesHeader->previousCrc = beToU16(offset);
            offset += 2;
        }
        
        // PES_extension_flag
        pesHeader->pesPrivateDataFlag = false;
        pesHeader->packHeaderFieldFlag = false;
        pesHeader->programPacketSequenceCounterFlag = false;
        pesHeader->pstdBufferFlag = false;
        pesHeader->pesExtensionFlag2 = false;
        if (pesHeader->pesExtensionFlag)
        {
            u8Tmp = *offset;
            offset += 1;
            pesHeader->pesPrivateDataFlag = (u8Tmp >> 7) & 0x01;
            pesHeader->packHeaderFieldFlag = (u8Tmp >> 6) & 0x01;
            pesHeader->programPacketSequenceCounterFlag = (u8Tmp >> 5) & 0x01;
            pesHeader->pstdBufferFlag = (u8Tmp >> 4) & 0x01;
            pesHeader->reserved = (u8Tmp >> 1) & 0x07;
            pesHeader->pesExtensionFlag2 = u8Tmp & 0x01;
            
            // PES_extension_flag
            if (pesHeader->pesPrivateDataFlag)
            {
                memcpy(pesHeader->pesPrivateData, offset, 16);
                offset += 16;
            }
            
            // pack_header_field_flag
            if (pesHeader->packHeaderFieldFlag)
            {
                pesHeader->packFieldLength = *offset;
                offset += 1 + pesHeader->packFieldLength;
            }
            
            // prograpesHeader->packet_sequence_counter_flag
            if (pesHeader->programPacketSequenceCounterFlag)
            {
                pesHeader->programPacketSequenceCounter = *offset & 0x7f;
                offset += 1;
                pesHeader->mpeg1Mpeg2Identifier = (*offset >> 6) & 0x01;
                pesHeader->originalStuffLength = *offset & 0x3f;
                offset += 1;
            }
            
            // P-STD_buffer_flag
            if (pesHeader->pstdBufferFlag)
            {
                u16Tmp = beToU16(offset);
                offset += 2;
                pesHeader->pstdBufferScale = (u16Tmp >> 13) & 0x0001;
                pesHeader->pstdBufferSize = u16Tmp & 0x01ff;
            }
            
            // PES_extension_flag_2
            if (pesHeader->pesExtensionFlag2)
            {
                pesHeader->pesExtensionFieldLength = *offset & 0x7f;
                offset += 1 + pesHeader->pesExtensionFieldLength;
            }
        }
        
        if (pesHeader->programPacketSequenceCounterFlag && pesHeader->originalStuffLength > 0)
        {
            memcpy(pesHeader->stuffingByte, offset, pesHeader->originalStuffLength);
            offset += pesHeader->originalStuffLength;
        }
        
        pesHeader->pesPacketDataByteLength = 0;
        if (length > (offset - data))
        {
            pesHeader->pesPacketDataByteLength = length - (offset - data);
            if (pesHeader->pesPacketDataByteLength > 0)
            {
                memcpy(pesHeader->pesPacketDataByte, offset, pesHeader->pesPacketDataByteLength);
                offset += pesHeader->pesPacketDataByteLength;
            }
        }
    }
    else if (ProgramStreamMap == type
             || PrivateStream2 == type
             || ECMStream == type
             || EMMStream == type
             || ProgramStreamDirectory == type
             || H2220_AnnexA_DSMCC_Stream == type
             || H2221_TypeE == type)
    {
        if (pesHeader->pesPacketLength == 0)
        {
            if (length != 0)
            {
                memcpy(pesHeader->pesPacketDataByte, offset, length);
                offset += length;
            }
        }
        else
        {
            memcpy(pesHeader->pesPacketDataByte, offset, pesHeader->pesPacketLength);
            offset += pesHeader->pesPacketLength;
        }
    }
    else if (PaddingStream == type)
    {
        if (pesHeader->pesPacketLength > 0)
        {
            memcpy(pesHeader->paddingByte, offset, pesHeader->pesPacketLength);
            offset += pesHeader->pesPacketLength;
        }
    }
    else
    {
    }
    
    return true;
}
// 私有函数 - 信息输出
static void makeLog(XXtsDataPtr dataPtr, char *format, ...)
{
    static char logBuffer[TS_LOG_BUFFER_MAX] = { 0 };
    if (NULL == dataPtr->logCallBack)
        return;
    
    memset(logBuffer, 0, TS_LOG_BUFFER_MAX);
    
    va_list valist;
    va_start(valist, format);
    vsnprintf(logBuffer, TS_LOG_BUFFER_MAX, format, valist);
    va_end(valist);
    
    dataPtr->logCallBack(logBuffer, (int)strlen(logBuffer));
}
// 私有函数 - 释放handle对应的pat/pmt表条目数据
static void freeItem(XXtsDataPtr dataPtr)
{
    for (int i = 0; i < TS_MAP_ITEM_MAX; i++)
    {
        if (NULL != dataPtr->patItem[i])
        {
            free(dataPtr->patItem[i]);
            dataPtr->patItem[i] = NULL;
        }
        
        if (NULL != dataPtr->pmtItem[i])
        {
            free(dataPtr->pmtItem[i]);
            dataPtr->pmtItem[i] = NULL;
        }
    }
}
// 私有函数 -
static void resetHandleData(XXtsDataPtr dataPtr){
    dataPtr->isFoundPAT = false;
    dataPtr->pmtPid     = 0;
    
    dataPtr->audioPid   = 0;
    dataPtr->audioType  = XXtsUnknown;
    dataPtr->audioBufferLength = 0;
    memset(dataPtr->audioBuffer, 0, TS_DATA_AUDIO_BUFFER_MAX);
    
    dataPtr->videoPid   = 0;
    dataPtr->videoType  = XXtsUnknown;
    dataPtr->videoBufferLength = 0;
    memset(dataPtr->videoBuffer, 0, TS_DATA_VIDEO_BUFFER_MAX);
    freeItem(dataPtr);
}
// 私有函数 - 为PSI表进行偏移自适应
static uint8_t adjustOffsetForPSI(XXtsHeader *header)
{
    if (header->adaptationFieldControl & 0x02)
        return 0;
    return 1;
}

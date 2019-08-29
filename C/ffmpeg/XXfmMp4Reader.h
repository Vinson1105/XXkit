//
//  XXfmFrameReader.h
//  XXapp
//
//  Created by VINSON on 2019/5/6.
//  Copyright © 2019 VINSON. All rights reserved.
//

#ifndef XXfmMp4Reader_h
#define XXfmMp4Reader_h

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "XXavUtility.h"

#define XXFM_MP4_READER_IS_END_OF_FILE 1
#define XXFM_MP4_READER_NEXT 2
typedef void* XXfmMp4ReaderHandle;

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */
    /**
     * @brief   创建句柄
     * @param   path 目标MP4文件路径
     * @param   needDecode 是否需要解码输出
     * @return  返回NULL失败,否则成功
     */
    XXfmMp4ReaderHandle xxfmMp4Reader_create(const char *path, bool needDecode);
    
    /**
     * @brief   获取下一个帧数据
     * @param   handle 目标句柄
     * @param   frame 帧数据承载体,由xxavFrame_alloc分配和xxavFrame_free释放
     * @return  返回<0,发生错误;返回XXFM_MP4_READER_IS_END_OF_FILE,文件已经全部读取;返回0,已成功获取帧数据
     */
    int xxfmMp4Reader_next(XXfmMp4ReaderHandle handle, XXavFrame *frame);
    
    /**
     * @brief   跳转到指定的时间点
     * @param   handle 目标句柄
     * @param   msec 跳转的时间点
     * @return  返回0,成功;否则失败
     */
    int xxfmMp4Reader_seek(XXfmMp4ReaderHandle handle, uint64_t msec);
    
    /**
     * @brief   释放句柄对应的资源
     * @param   handle 目标句柄
     */
    void xxfmMp4Reader_free(XXfmMp4ReaderHandle *handle);
    
    /**
     * @brief   获取文件分辨率
     * @param   handle 目标句柄
     * @param   width 宽度输出
     * @param   height 高度输出
     * @return  返回0,成功;否则失败
     */
    int xxfmMp4Reader_resolution(XXfmMp4ReaderHandle handle, int *width, int *height);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XXfmMp4Reader_h */

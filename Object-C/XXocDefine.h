/*
 * @author: GWX
 * @date: 20190109
 * @descirption: XX库下的Object-C工具全局定义
 * @history:
 *        1.author:
 *          date:
 *          modification:
 */

#ifndef __XXocDefine_h
#define __XXocDefine_h

#include "../XXdefine.h"

// 调试信息输出(注意：由于__FILE__并不是输出文件名称而是路径，调试信息输出可能过长，需要再添加部分处理以输出文件名称)
#ifdef XX_DEBUG
#define XXNSLog(...) NSLog(@"[XX] [FILE:%s] [LINE:%d] %@", __FILE__, __LINE__, [NSString stringWithFormat:__VA_ARGS__])
#else
#define XXNSLog(...) {}
#endif

#define XXOC_WS     __weak __typeof(&*self)ws = self // self的弱引用

#endif

/*
 * @author: GWX
 * @date: 20190412
 * @descirption: XX库通用模块
 * @history:
 *        1.author:
 *          date:
 *          modification:
 */

#ifndef XXutility_h
#define XXutility_h

#include <stdint.h>
#include <stdbool.h>

typedef struct XXdata{
    int8_t *data; uint64_t length;
}XXdata;
typedef struct XXbuffer{
    int8_t *data; uint64_t currentLength; uint64_t maxLength;
}XXbuffer;

// 获取GCC版本
#if !defined __GNUC_MINOR__ || defined __INTEL_COMPILER || defined __SUNPRO_C || defined __SUNPRO_CC || defined __llvm__ || defined __clang__
  #define XX_GCC_VERSION(major,minor) 0
#else
  #define XX_GCC_VERSION(major,minor) (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#endif

// 获取CLANG版本
#define XX_CLANG_VERSION(major,minor) (__clang_major__ > (major) || (__clang_major__ == (major) && __clang_minor__ >= (minor)))

#if __clang__ && defined __has_builtin
  #define XX_CLANG_BUILTIN(x) __has_builtin (x)
#else
  #define XX_CLANG_BUILTIN(x) 0
#endif

// 条件分支优化宏，XX_LIKELY：很大可能为真；XX_LIKELY：很大可能为假；
#if XX_GCC_VERSION(3,1) || XX_CLANG_BUILTIN(__builtin_expect)
    #define XX_LIKELY(expr)         __builtin_expect(!!(expr), 1)
    #define XX_UNLIKELY(expr)       __builtin_expect(!!(expr), 0)
#else
    #define XX_UNLIKELY(expr)       (expr)
    #define XX_LIKELY(expr,value)   (expr)
#endif

#endif

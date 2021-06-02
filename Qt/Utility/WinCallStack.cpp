#include "WinCallStack.h"

#include <Windows.h>
#include <DbgHelp.h>
#include <iostream>
#include <vector>
#include "XXlogger.h"
#include <QDebug>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QDateTime>

// 添加对dbghelp.lib的编译依赖
//
#pragma comment(lib, "dbghelp.lib")

using namespace std;

const int MAX_ADDRESS_LENGTH = 32;
const int MAX_NAME_LENGTH = 1024;

/** 崩溃信息 */
struct CrashInfo{
    CHAR ErrorCode[MAX_ADDRESS_LENGTH];
    CHAR Address[MAX_ADDRESS_LENGTH];
    CHAR Flags[MAX_ADDRESS_LENGTH];
};

/** CallStack信息 */
struct CallStackInfo
{
    CHAR ModuleName[MAX_NAME_LENGTH];
    CHAR MethodName[MAX_NAME_LENGTH];
    CHAR FileName[MAX_NAME_LENGTH];
    CHAR LineNumber[MAX_NAME_LENGTH];
};

/** 安全拷贝字符串函数 */
static void safeStrCpy(char* dest, size_t destSizeMax, const char* src){
    if (destSizeMax <= 0){
        return;
    }
    if (strlen(src) < destSizeMax){
        strcpy_s(dest, destSizeMax, src);
    }
    else{
        strncpy_s(dest, destSizeMax, src, destSizeMax);
        dest[destSizeMax-1] = '\0';
    }
}

/** 得到程序崩溃信息 */
static CrashInfo getCrashInfo(const EXCEPTION_RECORD *pRecord){
    CrashInfo crashinfo;
    safeStrCpy(crashinfo.Address, MAX_ADDRESS_LENGTH, "N/A");
    safeStrCpy(crashinfo.ErrorCode, MAX_ADDRESS_LENGTH, "N/A");
    safeStrCpy(crashinfo.Flags, MAX_ADDRESS_LENGTH, "N/A");

    sprintf_s(crashinfo.Address, "%08X", pRecord->ExceptionAddress);
    sprintf_s(crashinfo.ErrorCode, "%08X", pRecord->ExceptionCode);
    sprintf_s(crashinfo.Flags, "%08X", pRecord->ExceptionFlags);

    return crashinfo;
}

/** 得到CallStack信息 */
static vector<CallStackInfo> getCallStack(CONTEXT *context)
{
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);

    STACKFRAME64 sf;
    memset(&sf, 0, sizeof(STACKFRAME64));
    DWORD dwImageType = IMAGE_FILE_MACHINE_I386;

    /** 不同的CPU类型，具体信息可查询MSDN */
#ifdef _M_IX86
    sf.AddrPC.Offset = context->Eip;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Offset = context->Esp;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = context->Ebp;
    sf.AddrFrame.Mode = AddrModeFlat;
#elif _M_X64
    dwImageType = IMAGE_FILE_MACHINE_AMD64;
    sf.AddrPC.Offset = context->Rip;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = context->Rsp;
    sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = context->Rsp;
    sf.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
    dwImageType = IMAGE_FILE_MACHINE_IA64;
    sf.AddrPC.Offset = context->StIIP;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = context->IntSp;
    sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrBStore.Offset = context->RsBSP;
    sf.AddrBStore.Mode = AddrModeFlat;
    sf.AddrStack.Offset = context->IntSp;
    sf.AddrStack.Mode = AddrModeFlat;
#else
    #error "Platform not supported!"
#endif

    HANDLE thread = GetCurrentThread();
    vector<CallStackInfo> callStackInfos;
    while (true)
    {
        // 该函数是实现这个功能的最重要的一个函数
        // 函数的用法以及参数和返回值的具体解释可以查询MSDN
        //
        /**
          该函数是实现这个功能的最重要的一个函数,
          函数的用法以及参数和返回值的具体解释可以查询MSDN;
        */
        if (!StackWalk64(dwImageType, process, thread, &sf, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)){
            break;
        }
        if (sf.AddrFrame.Offset == 0){
            break;
        }

        CallStackInfo callstackinfo;
        safeStrCpy(callstackinfo.MethodName, MAX_NAME_LENGTH, "N/A");
        safeStrCpy(callstackinfo.FileName, MAX_NAME_LENGTH, "N/A");
        safeStrCpy(callstackinfo.ModuleName, MAX_NAME_LENGTH, "N/A");
        safeStrCpy(callstackinfo.LineNumber, MAX_NAME_LENGTH, "N/A");

        BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH];
        IMAGEHLP_SYMBOL64 *symbol = (IMAGEHLP_SYMBOL64*)symbolBuffer;
        memset(symbol, 0, sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH);

        symbol->SizeOfStruct = sizeof(symbolBuffer);
        symbol->MaxNameLength = MAX_NAME_LENGTH;

        DWORD symDisplacement = 0;

        /** 得到函数名 */
        if (SymGetSymFromAddr64(process, sf.AddrPC.Offset, NULL, symbol)){
            safeStrCpy(callstackinfo.MethodName, MAX_NAME_LENGTH, symbol->Name);
        }

        IMAGEHLP_LINE64 lineInfo;
        memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));

        lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        DWORD dwLineDisplacement;

        /** 得到文件名和所在的代码行 */
        if (SymGetLineFromAddr64(process, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo)){
            safeStrCpy(callstackinfo.FileName, MAX_NAME_LENGTH, lineInfo.FileName);
            sprintf_s(callstackinfo.LineNumber, "%d", lineInfo.LineNumber);
        }

        IMAGEHLP_MODULE64 moduleInfo;
        memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));

        moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

        /** 得到模块名 */
        if (SymGetModuleInfo64(process, sf.AddrPC.Offset, &moduleInfo)){
            safeStrCpy(callstackinfo.ModuleName, MAX_NAME_LENGTH, moduleInfo.ModuleName);
        }

        callStackInfos.push_back(callstackinfo);
    }

    SymCleanup(process);

    return callStackInfos;
}

/** */
static void createDumpFile(const QString &path, EXCEPTION_POINTERS *exception){
    // 创建目录
    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if(!dir.exists()){
        QDir().mkpath(dir.absolutePath());
    }

    // 创建文件
    if(!fileInfo.exists()){
        QFile file(path);
        file.open(QIODevice::ReadWrite);
        file.close();
    }
    HANDLE hDumpFile = CreateFile(path.toStdWString().data(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // dump信息
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = exception;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    // 写入文件并关闭
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    CloseHandle(hDumpFile);
}

/** 处理Unhandled Exception的回调函数 */
static LONG applicationCrashHandler(EXCEPTION_POINTERS *exception)
{
    // 确保有足够的栈空间
    //
#ifdef _M_IX86
    if (exception->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
    {
        static char TempStack[1024 * 128];
        __asm mov eax,offset TempStack;
        __asm mov esp,eax;
    }
#endif

    CrashInfo crashinfo = getCrashInfo(exception->ExceptionRecord);

    // 输出Crash信息
    //
    //cout << "ErrorCode: " << crashinfo.ErrorCode << endl;
    //cout << "Address: " << crashinfo.Address << endl;
    //cout << "Flags: " << crashinfo.Flags << endl;

    vector<CallStackInfo> callStackInfos = getCallStack(exception->ContextRecord);
    for (vector<CallStackInfo>::iterator i = callStackInfos.begin(); i != callStackInfos.end(); ++i)
    {
        CallStackInfo callstackinfo = (*i);

        QString string = QString("MethodName:[")+callstackinfo.ModuleName+"]" +
                        QString(" File:[")+callstackinfo.FileName+"]" +
                        QString(" Line:[")+callstackinfo.LineNumber+"]\n";
        XXlogger::instance().message(string,true);
    }

    QString path = QDir::homePath() + "/"
            + QCoreApplication::applicationName() + "/"
            + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz")+".dmp";
    createDumpFile(path, exception);

    return EXCEPTION_EXECUTE_HANDLER;
}

void WinCallStack::install(){
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)applicationCrashHandler);
}

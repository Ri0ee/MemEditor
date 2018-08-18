#pragma once

#include "windows.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class Memory_class
{
public:
    Memory_class(){}
    ~Memory_class(){}

    bool Init(int process_id_);
    void Shutdown();

    void GetSysInf();
    bool SetDbgPrivilege();
    bool GetMBI(LPCVOID address_);
    bool CheckMemoryAccess();
    void PrintMemoryInfo(MEMORY_BASIC_INFORMATION memory_basic_info_);

    std::string ReadString(DWORD address_, int amount_);
    int ReadInteger(DWORD address_);
    char ReadChar(DWORD address_);

    int GetPID()                        { return m_process_id; }
    bool GetDebugPrivilegeStatus()      { return m_debug_privilege_status; }
    HANDLE GetPHandle()                 { return m_process_handle; }

private:
    int m_process_id;
    HANDLE m_process_handle;
    MEMORY_BASIC_INFORMATION m_memory_basic_info;
    SYSTEM_INFO m_system_info;

    BOOL m_debug_privilege_status;
};

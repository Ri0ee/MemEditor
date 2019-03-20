#pragma once

#include "windows.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "src/utility.h"

typedef long long (NTAPI* RtlAdjustPrivilege_)(ULONG Privilege, BOOL Enable, BOOL CurrentThread, PBOOL Enabled);

template<typename T> std::string to_string(const T& n)
{
    std::ostringstream stm;
    stm << n;
    return stm.str();
}

class MemoryManager
{
public:
    MemoryManager(int process_id_) : m_process_id(process_id_) { Initialize(); }
    ~MemoryManager() { Shutdown(); }

    void GetSysInf();
    bool SetDbgPrivilege();
    bool GetMBI(LPCVOID address_, MEMORY_BASIC_INFORMATION& mbi_);
    bool MemoryAccess(MEMORY_BASIC_INFORMATION mbi_);
    std::string FormatMemoryInfo(MEMORY_BASIC_INFORMATION memory_basic_info_);

    bool ReadString(DWORD address_, std::string& value_, int max_amount_);
    bool ReadInteger(DWORD address_, int& value_);
    bool ReadByte(DWORD address_, unsigned char& value_);

    bool WriteInteger(DWORD address_, int value_);
    bool WriteByte(DWORD address_, unsigned char value_);
    bool WriteString(DWORD address_, std::string value_);

    bool ReadSection(DWORD section_address_, std::vector<unsigned char>& value_);
    bool SearchForString(DWORD start_address_, DWORD end_address_, const std::string value_, DWORD& result_address_);
    bool ParseMemory(PVOID start_address_, PVOID max_address_, std::vector<char> value_, DWORD& result_address_);

    int GetPID()                        { return m_process_id; }
    bool GetDebugPrivilegeStatus()      { return m_debug_privilege_status; }
    HANDLE GetPHandle()                 { return m_process_handle; }

private:
    bool m_initialized = false;

    int m_process_id = 0;
    HANDLE m_process_handle = 0;
    MEMORY_BASIC_INFORMATION m_memory_basic_info;
    SYSTEM_INFO m_system_info;
    BOOL m_debug_privilege_status = false;

    void Initialize();
    void Shutdown();
};

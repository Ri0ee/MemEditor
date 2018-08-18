#include "memory.h"

bool Memory_class::Init(int process_id_)
{
    m_debug_privilege_status = false;

    std::cout << "Calling SetDbgPrivilege: ";
    if(!SetDbgPrivilege())
        return false;
    std::cout << "Success\n";

    m_process_id = process_id_;

    std::cout << "Opening process: ";
    m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, 0, m_process_id);
    if(!m_process_handle)
    {
        std::cerr << "Failed to open process object\n";
        return false;
    }
    std::cout << "Success\n";

    GetSysInf();
    std::cout << "Getting first memory basic info: ";
    if(!GetMBI((LPCVOID)m_system_info.lpMinimumApplicationAddress))
        return false;
    std::cout << "Success\n";

    PrintMemoryInfo(m_memory_basic_info);

    return true;
}

void Memory_class::Shutdown()
{
    CloseHandle(m_process_handle);
}

void Memory_class::PrintMemoryInfo(MEMORY_BASIC_INFORMATION memory_basic_info_)
{
    std::cout << "Memory allocation base: " << memory_basic_info_.AllocationBase << "\n";
    std::cout << "Memory base address: " << memory_basic_info_.BaseAddress << "\n";
    std::cout << "Memory region size: " << memory_basic_info_.RegionSize << "\n";

    /// Memory state
    if(memory_basic_info_.State & 0x001000) std::cout << "Memory state: MEM_COMMIT\n";
    if(memory_basic_info_.State & 0x010000) std::cout << "Memory state: MEM_FREE\n";
    if(memory_basic_info_.State & 0x002000) std::cout << "Memory state: MEM_RESERVE\n";

    /// Memory type
    if(memory_basic_info_.Type & 0x1000000) std::cout << "Memory type: MEM_IMAGE\n";
    if(memory_basic_info_.Type & 0x0040000) std::cout << "Memory type: MEM_MAPPED\n";
    if(memory_basic_info_.Type & 0x0020000) std::cout << "Memory type: MEM_PRIVATE\n";

    /// Memory protection
    if(memory_basic_info_.Protect & 0x0010) std::cout << "Memory protection: PAGE_EXECUTE\n";
    if(memory_basic_info_.Protect & 0x0020) std::cout << "Memory protection: PAGE_EXECUTE_READ\n";
    if(memory_basic_info_.Protect & 0x0040) std::cout << "Memory protection: PAGE_EXECUTE_READWRITE\n";
    if(memory_basic_info_.Protect & 0x0080) std::cout << "Memory protection: PAGE_EXECUTE_WRITECOPY\n";
    if(memory_basic_info_.Protect & 0x0001) std::cout << "Memory protection: PAGE_NOACCESS\n";
    if(memory_basic_info_.Protect & 0x0002) std::cout << "Memory protection: PAGE_READONLY\n";
    if(memory_basic_info_.Protect & 0x0004) std::cout << "Memory protection: PAGE_READWRITE\n";
    if(memory_basic_info_.Protect & 0x0008) std::cout << "Memory protection: PAGE_WRITECOPY\n";
}

void Memory_class::GetSysInf()
{
    ZeroMemory(&m_system_info, sizeof(m_system_info));
    GetSystemInfo(&m_system_info);
}

bool Memory_class::GetMBI(LPCVOID address_)
{
    ZeroMemory(&m_memory_basic_info, sizeof(m_memory_basic_info));
    if(!VirtualQueryEx(m_process_handle, address_, &m_memory_basic_info, sizeof(m_memory_basic_info)))
    {
        std::cout << "Failed to get basic memory information\n";
        return false;
    }

    return true;
}

bool Memory_class::CheckMemoryAccess()
{
    if((m_memory_basic_info.Protect & PAGE_READWRITE) || (m_memory_basic_info.Protect & PAGE_WRITECOPY))
        return true;

    return false;
}

bool Memory_class::SetDbgPrivilege()
{
    #define DEBUG_PRIVILEGE 20L

    LONG (WINAPI *RtlAdjustPrivilege)(DWORD, BOOL, INT, PBOOL);
    *(FARPROC *)&RtlAdjustPrivilege = GetProcAddress(GetModuleHandle("ntdll.dll"), "RtlAdjustPrivilege");

    if(!RtlAdjustPrivilege)
    {
        std::cout << "Failed to set debugger privilege on current process\n";
        return false;
    }

    RtlAdjustPrivilege(DEBUG_PRIVILEGE, true, 0, &m_debug_privilege_status);

    return true;
}

std::string Memory_class::ReadString(DWORD address_, int amount_)
{
    char temp_char_array[amount_] = {};
    ReadProcessMemory(m_process_handle, (void*)address_, &temp_char_array, sizeof(temp_char_array), 0);
    return std::string(temp_char_array);
}

char Memory_class::ReadChar(DWORD address_)
{
    char temp_char = ' ';
    ReadProcessMemory(m_process_handle, (void*)address_, &temp_char, sizeof(temp_char), 0);
    return temp_char;
}

int Memory_class::ReadInteger(DWORD address_)
{
    int temp_int = 0;
    ReadProcessMemory(m_process_handle, (void*)address_, &temp_int, sizeof(temp_int), 0);
    return temp_int;
}

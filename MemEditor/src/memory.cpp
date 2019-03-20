#include "src/memory.h"

static RtlAdjustPrivilege_ RtlAdjustPrivilege =
        (RtlAdjustPrivilege_)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "RtlAdjustPrivilege");

void MemoryManager::Initialize()
{
    if(!SetDbgPrivilege()) /// Try getting debugger privileges on process
        return;

    m_process_handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, 0, m_process_id);
    if(!m_process_handle)
        return;

    GetSysInf();
    if(!GetMBI((LPCVOID)m_system_info.lpMinimumApplicationAddress, m_memory_basic_info))
        return;

    m_initialized = true;
}

void MemoryManager::Shutdown()
{
    CloseHandle(m_process_handle);
    m_initialized = false;
}

std::string MemoryManager::FormatMemoryInfo(MEMORY_BASIC_INFORMATION memory_basic_info_)
{
    std::string temp_string;

    temp_string.append("Memory allocation base: " + to_string(memory_basic_info_.AllocationBase) + "\n");
    temp_string.append("Memory base address: " + to_string(memory_basic_info_.BaseAddress) + "\n");
    temp_string.append("Memory region size: " + to_string(memory_basic_info_.RegionSize) + "\n");

    if(memory_basic_info_.State & 0x001000) temp_string.append("Memory state: MEM_COMMIT\n");
    if(memory_basic_info_.State & 0x010000) temp_string.append("Memory state: MEM_FREE\n");
    if(memory_basic_info_.State & 0x002000) temp_string.append("Memory state: MEM_RESERVE\n");

    if(memory_basic_info_.Type & 0x1000000) temp_string.append("Memory type: MEM_IMAGE\n");
    if(memory_basic_info_.Type & 0x0040000) temp_string.append("Memory type: MEM_MAPPED\n");
    if(memory_basic_info_.Type & 0x0020000) temp_string.append("Memory type: MEM_PRIVATE\n");

    if(memory_basic_info_.Protect & 0x0010) temp_string.append("Memory protection: PAGE_EXECUTE\n");
    if(memory_basic_info_.Protect & 0x0020) temp_string.append("Memory protection: PAGE_EXECUTE_READ\n");
    if(memory_basic_info_.Protect & 0x0040) temp_string.append("Memory protection: PAGE_EXECUTE_READWRITE\n");
    if(memory_basic_info_.Protect & 0x0080) temp_string.append("Memory protection: PAGE_EXECUTE_WRITECOPY\n");
    if(memory_basic_info_.Protect & 0x0001) temp_string.append("Memory protection: PAGE_NOACCESS\n");
    if(memory_basic_info_.Protect & 0x0002) temp_string.append("Memory protection: PAGE_READONLY\n");
    if(memory_basic_info_.Protect & 0x0004) temp_string.append("Memory protection: PAGE_READWRITE\n");
    if(memory_basic_info_.Protect & 0x0008) temp_string.append("Memory protection: PAGE_WRITECOPY\n");

    if(memory_basic_info_.State & 0x001000) temp_string.append("Memory state: MEM_COMMIT\n");
    if(memory_basic_info_.State & 0x010000) temp_string.append("Memory state: MEM_FREE\n");
    if(memory_basic_info_.State & 0x002000) temp_string.append("Memory state: MEM_RESERVE\n");

    if(memory_basic_info_.Type & 0x1000000) temp_string.append("Memory type: MEM_IMAGE\n");
    if(memory_basic_info_.Type & 0x0040000) temp_string.append("Memory type: MEM_MAPPED\n");
    if(memory_basic_info_.Type & 0x0020000) temp_string.append("Memory type: MEM_PRIVATE\n");

    if(memory_basic_info_.Protect & 0x0010) temp_string.append("Memory protection: PAGE_EXECUTE\n");
    if(memory_basic_info_.Protect & 0x0020) temp_string.append("Memory protection: PAGE_EXECUTE_READ\n");
    if(memory_basic_info_.Protect & 0x0040) temp_string.append("Memory protection: PAGE_EXECUTE_READWRITE\n");
    if(memory_basic_info_.Protect & 0x0080) temp_string.append("Memory protection: PAGE_EXECUTE_WRITECOPY\n");
    if(memory_basic_info_.Protect & 0x0001) temp_string.append("Memory protection: PAGE_NOACCESS\n");
    if(memory_basic_info_.Protect & 0x0002) temp_string.append("Memory protection: PAGE_READONLY\n");
    if(memory_basic_info_.Protect & 0x0004) temp_string.append("Memory protection: PAGE_READWRITE\n");
    if(memory_basic_info_.Protect & 0x0008) temp_string.append("Memory protection: PAGE_WRITECOPY\n");

    return temp_string;
}

void MemoryManager::GetSysInf()
{
    m_system_info = { 0 };
    GetSystemInfo(&m_system_info);
}

bool MemoryManager::GetMBI(LPCVOID address_, MEMORY_BASIC_INFORMATION& mbi_)
{
    mbi_ = { 0 };
    if(!VirtualQueryEx(m_process_handle, address_, &mbi_, sizeof(mbi_)))
        return false;

    return true;
}

bool MemoryManager::MemoryAccess(MEMORY_BASIC_INFORMATION mbi_)
{
    if((mbi_.Protect & PAGE_READWRITE) || (mbi_.Protect & PAGE_WRITECOPY))
        return true;

    return false;
}

bool MemoryManager::SetDbgPrivilege()
{
    long long nt_error_code = RtlAdjustPrivilege(20, true, 0, &m_debug_privilege_status);
    if(nt_error_code != 0) return true;

    return true;
}

bool MemoryManager::WriteInteger(DWORD address_, int value_)
{
    if(!m_initialized)
        return false;

    if(!WriteProcessMemory(m_process_handle, (void*)address_, &value_, sizeof(value_), 0))
        return false;

    return true;
}

bool MemoryManager::WriteByte(DWORD address_, unsigned char value_)
{
    if(!m_initialized)
        return false;

    if(!WriteProcessMemory(m_process_handle, (void*)address_, &value_, sizeof(value_), 0))
        return false;

    return true;
}

bool MemoryManager::WriteString(DWORD address_, std::string value_)
{
    if(!m_initialized)
        return false;

    if(!WriteProcessMemory(m_process_handle, (void*)address_, value_.data(), sizeof(value_), 0))
        return false;

    return true;
}

bool MemoryManager::ReadString(DWORD address_, std::string& value_, int max_amount_)
{
    if(!m_initialized)
        return false;

    char temp_char_array[max_amount_] = {};
    if(!ReadProcessMemory(m_process_handle, (void*)address_, &temp_char_array, sizeof(temp_char_array), 0))
        return false;

    value_ = std::string(temp_char_array);

    return true;
}

bool MemoryManager::ReadByte(DWORD address_, unsigned char& value_)
{
    if(!m_initialized)
        return false;

    unsigned char temp_byte = 0;
    if(!ReadProcessMemory(m_process_handle, (void*)address_, &temp_byte, sizeof(temp_byte), 0))
        return false;

    value_ = temp_byte;

    return true;
}

bool MemoryManager::ReadInteger(DWORD address_, int& value_)
{
    if(!m_initialized)
        return false;

    int temp_int = 0;
    if(!ReadProcessMemory(m_process_handle, (void*)address_, &temp_int, sizeof(temp_int), 0))
        return false;

    value_ = temp_int;

    return true;
}

bool MemoryManager::SearchForString(DWORD start_address_, DWORD end_address_, const std::string value_, DWORD& result_address_)
{
    PVOID temp_start_address = (start_address_ == 0)?m_system_info.lpMinimumApplicationAddress:(void*)start_address_;
    PVOID temp_end_address = (end_address_ == 0)?m_system_info.lpMaximumApplicationAddress:(void*)end_address_;
    DWORD temp_result_address = 0;
    std::vector<char> temp_char_vector(value_.begin(), value_.end());

    if(!ParseMemory(temp_start_address, temp_end_address, temp_char_vector, temp_result_address))
        return false;

    result_address_ = temp_result_address;

    return true;
}

bool MemoryManager::ReadSection(DWORD section_address_, std::vector<unsigned char>& value_)
{
    MEMORY_BASIC_INFORMATION temp_mbi;
    GetMBI((LPCVOID)section_address_, temp_mbi);
    if(!MemoryAccess(temp_mbi))
        return false;

    std::vector<unsigned char> temp_data_vec(temp_mbi.RegionSize);

    if(!ReadProcessMemory(m_process_handle, (void*)temp_mbi.AllocationBase, temp_data_vec.data(), temp_mbi.RegionSize, 0))
        return false;

    value_ = std::vector<unsigned char>(temp_data_vec);

    return true;
}

bool MemoryManager::ParseMemory(PVOID start_address_, PVOID max_address_, std::vector<char> value_, DWORD& result_address_)
{
//    std::cout.write(value_.data(), value_.size());
    return true;
}

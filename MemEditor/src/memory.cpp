#include "src/memory.h"

static RtlAdjustPrivilege_ RtlAdjustPrivilege =
        (RtlAdjustPrivilege_)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "RtlAdjustPrivilege");

bool Memory_class::Init(int process_id_)
{
    m_debug_privilege_status = false;
    m_process_id = process_id_;
    m_is_initialized = false;

    std::cout << "Calling SetDbgPrivilege: ";
    if(!SetDbgPrivilege())
        return false; std::cout << "Success\n";

    std::cout << "Calling OpenProcess: ";
    m_process_handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, 0, m_process_id);
    if(!m_process_handle)
    {
        std::cerr << "Failed to open process object. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    } std::cout << "Success\n";

    GetSysInf();
    std::cout << "Calling GetMBI: ";
    if(!GetMBI((LPCVOID)m_system_info.lpMinimumApplicationAddress, m_memory_basic_info))
        return false; std::cout << "Success\n";

    PrintMemoryInfo(m_memory_basic_info);

    m_is_initialized = true;

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

    if(memory_basic_info_.State & 0x001000) std::cout << "Memory state: MEM_COMMIT\n";
    if(memory_basic_info_.State & 0x010000) std::cout << "Memory state: MEM_FREE\n";
    if(memory_basic_info_.State & 0x002000) std::cout << "Memory state: MEM_RESERVE\n";

    if(memory_basic_info_.Type & 0x1000000) std::cout << "Memory type: MEM_IMAGE\n";
    if(memory_basic_info_.Type & 0x0040000) std::cout << "Memory type: MEM_MAPPED\n";
    if(memory_basic_info_.Type & 0x0020000) std::cout << "Memory type: MEM_PRIVATE\n";

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

bool Memory_class::GetMBI(LPCVOID address_, MEMORY_BASIC_INFORMATION& mbi_)
{
    ZeroMemory(&mbi_, sizeof(mbi_));
    if(!VirtualQueryEx(m_process_handle, address_, &mbi_, sizeof(mbi_)))
    {
        std::cout << "Failed to get basic memory information. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    }

    return true;
}

bool Memory_class::MemoryAccess(MEMORY_BASIC_INFORMATION mbi_)
{
    if((mbi_.Protect & PAGE_READWRITE) || (mbi_.Protect & PAGE_WRITECOPY))
        return true;

    return false;
}

bool Memory_class::SetDbgPrivilege()
{
    long long nt_error_code = RtlAdjustPrivilege(20, true, 0, &m_debug_privilege_status);
    if(nt_error_code != 0) return true;

    return true;
}

bool Memory_class::WriteInteger(DWORD address_, int value_)
{
    if(!m_is_initialized)
    {
        std::cerr << "Error: memory class hasn't been initialized\n";
        return false;
    }

    if(!WriteProcessMemory(m_process_handle, (void*)address_, &value_, sizeof(value_), 0))
    {
        std::cerr << "Failed to write into a process memory. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    }

    return true;
}

bool Memory_class::WriteByte(DWORD address_, unsigned char value_)
{
    if(!m_is_initialized)
    {
        std::cerr << "Error: memory class hasn't been initialized\n";
        return false;
    }

    if(!WriteProcessMemory(m_process_handle, (void*)address_, &value_, sizeof(value_), 0))
    {
        std::cerr << "Failed to write into a process memory. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    }

    return true;
}

bool Memory_class::WriteString(DWORD address_, std::string value_)
{
    if(!m_is_initialized)
    {
        std::cerr << "Error: memory class hasn't been initialized\n";
        return false;
    }

    if(!WriteProcessMemory(m_process_handle, (void*)address_, value_.data(), sizeof(value_), 0))
    {
        std::cerr << "Failed to read process memory. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    }

    return true;
}

bool Memory_class::ReadString(DWORD address_, std::string& value_, int max_amount_)
{
    if(!m_is_initialized)
    {
        std::cerr << "Error: memory class hasn't been initialized\n";
        return false;
    }

    char temp_char_array[max_amount_] = {};
    if(!ReadProcessMemory(m_process_handle, (void*)address_, &temp_char_array, sizeof(temp_char_array), 0))
    {
        std::cerr << "Failed to read process memory. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    }

    value_ = std::string(temp_char_array);

    return true;
}

bool Memory_class::ReadByte(DWORD address_, unsigned char& value_)
{
    if(!m_is_initialized)
    {
        std::cerr << "Error: memory class hasn't been initialized\n";
        return false;
    }

    unsigned char temp_byte = 0;
    if(!ReadProcessMemory(m_process_handle, (void*)address_, &temp_byte, sizeof(temp_byte), 0))
    {
        std::cerr << "Failed to read process memory. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    }

    value_ = temp_byte;

    return true;
}

bool Memory_class::ReadInteger(DWORD address_, int& value_)
{
    if(!m_is_initialized)
    {
        std::cerr << "Error: memory class hasn't been initialized\n";
        return false;
    }

    int temp_int = 0;
    if(!ReadProcessMemory(m_process_handle, (void*)address_, &temp_int, sizeof(temp_int), 0))
    {
        std::cerr << "Failed to read process memory. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    }

    value_ = temp_int;

    return true;
}

bool Memory_class::SearchForString(DWORD start_address_, DWORD end_address_, const std::string value_, DWORD& result_address_)
{
    PVOID temp_start_address = (start_address_ == 0)?m_system_info.lpMinimumApplicationAddress:(void*)start_address_;
    PVOID temp_end_address = (end_address_ == 0)?m_system_info.lpMaximumApplicationAddress:(void*)end_address_;
    DWORD temp_result_address = 0;
    std::vector<char> temp_char_vector(value_.begin(), value_.end());

    if(!ParseMemory(temp_start_address, temp_end_address, temp_char_vector, temp_result_address))
    {
        std::cerr << "Failed to parse process memory\n";
        return false;
    }

    result_address_ = temp_result_address;

    return true;
}

bool Memory_class::ReadSection(DWORD section_address_, std::vector<unsigned char>& value_)
{
    MEMORY_BASIC_INFORMATION temp_mbi;
    GetMBI((LPCVOID)section_address_, temp_mbi);
    if(!MemoryAccess(temp_mbi))
    {
        std::cerr << "Memory region at " << section_address_ << " is unaccessible\n";
        return false;
    }

    std::vector<unsigned char> temp_data_vec(temp_mbi.RegionSize);

    if(!ReadProcessMemory(m_process_handle, (void*)temp_mbi.AllocationBase, temp_data_vec.data(), temp_mbi.RegionSize, 0))
    {
        std::cerr << "Failed to read process memory. Error: " << FormatError(GetLastError()) << "\n";
        return false;
    }

    value_ = std::vector<unsigned char>(temp_data_vec);

    return true;
}

bool Memory_class::ParseMemory(PVOID start_address_, PVOID max_address_, std::vector<char> value_, DWORD& result_address_)
{
//    std::cout.write(value_.data(), value_.size());
    return true;
}

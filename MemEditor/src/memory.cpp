#include "memory.h"

bool Memory_class::Init(int process_id_)
{
    m_process_id = process_id_;
    m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, 0, m_process_id);
    if(!m_process_handle)
    {
        std::cerr << "Failed to open process object\n";
        return false;
    }

    return true;
}

void Memory_class::Shutdown()
{
    CloseHandle(m_process_handle);
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

#include "instance.h"

bool Instance_class::Init()
{
    m_process.possible_exe_names.push_back(std::string("TestSubject.exe"));
    if(!GetPIDs(m_process, true))
    {
        std::cerr << "Error getting PIDs\n";
        return false;
    }

    if(!m_process.process_id_vector.empty())
    {
        for(auto it = m_process.process_id_vector.begin(); it != m_process.process_id_vector.end(); it++)
            std::cout << "PID: " << *it << "\n";
    } else
    {
        std::cout << "No PID found\n";
        return false;
    }

    return true;
}

void Instance_class::Run()
{
    WaitForInput();

    Memory_class* memory_manager = new Memory_class();
    if(!memory_manager->Init(*m_process.process_id_vector.begin()))
    {
        std::cout << "Failed to initialize memory manager\n";
        delete memory_manager;
        return;
    }

    /*
    for(int i = 0; i < 256; i++)
    {
        std::cout << memory_manager->ReadChar(0x47F035 + i) << " ";
    }
    */

    std::cout << memory_manager->ReadString(0x69FDF0, 100);

    memory_manager->Shutdown();
    delete memory_manager;

    WaitForInput();

    DWORD str_1_adress = 0x47F035;
    DWORD strcpp = 0x69FDF0;
}

void Instance_class::WaitForInput()
{
    std::cin.get();
}

bool Instance_class::GetPIDs(process_info& process_info_, bool only_first_one_)
{
    PROCESSENTRY32 process_entry;
    process_entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(process_snapshot == INVALID_HANDLE_VALUE)
        return false;

    if(Process32First(process_snapshot, &process_entry)) /// if returns true
    {
        while(Process32Next(process_snapshot, &process_entry)) /// while there still are some processes
        {
            for(auto it = process_info_.possible_exe_names.begin(); it != process_info_.possible_exe_names.end(); it++)
            {
                std::string temp_exe_name = std::string(process_entry.szExeFile);
                if(temp_exe_name == *it)
                {
                    process_info_.process_id_vector.push_back(process_entry.th32ProcessID);
                    if(only_first_one_)
                    {
                        CloseHandle(process_snapshot);
                        return true;
                    }
                }
            }
        }
    } else
    {
        CloseHandle(process_snapshot);
        return false;
    }

    CloseHandle(process_snapshot);
    return true;
}

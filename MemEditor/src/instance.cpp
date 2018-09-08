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
    Memory_class* memory_manager = new Memory_class();
    if(!memory_manager->Init(*m_process.process_id_vector.begin()))
    {
        std::cout << "Failed to initialize memory manager\n";
        delete memory_manager;
        system("PAUSE");
        return;
    }

    /*
    std::vector<unsigned char> temp_byte_vector;
    for(int i = 0; i < 255; i++)
    {
        unsigned char temp_byte = 0;
        memory_manager->ReadByte(0x10000 + i, temp_byte);
        temp_byte_vector.push_back(temp_byte);
    }
    std::cout << "\n" << FormatHex(temp_byte_vector) << "\n";
    */

    std::vector<unsigned char> temp_byte_vector;
    memory_manager->ReadSection(0x20000, temp_byte_vector);
    std::string temp_output = FormatHex(temp_byte_vector);

    std::cout << temp_output << "\n\n";

    memory_manager->Shutdown();
    delete memory_manager;

    system("PAUSE");
}

bool Instance_class::GetPIDs(process_info& process_info_, bool only_first_one_)
{
    PROCESSENTRY32 process_entry;
    process_entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(process_snapshot == INVALID_HANDLE_VALUE)
    {
        std::cerr << "CreateToolhelp32Snapshot returned INVALID_HANDLE_VALUE\n";
        return false;
    }

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
        std::cerr << "Process32First failed\n";
        CloseHandle(process_snapshot);
        return false;
    }

    CloseHandle(process_snapshot);
    return true;
}

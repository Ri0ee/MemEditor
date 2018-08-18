#pragma once

#include "windows.h"
#include "tlhelp32.h"

#include "src/memory.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct process_info
{
    std::vector<std::string> possible_exe_names;
    std::vector<int> process_id_vector;
};

class Instance_class
{
public:
    Instance_class(){}
    ~Instance_class(){}

    bool Init();
    void Run();
    void WaitForInput();
    bool GetPIDs(process_info& process_info_, bool only_first_one_);

private:
    process_info m_process;
};

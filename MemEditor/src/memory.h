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

    std::string ReadString(DWORD address_, int amount_);
    int ReadInteger(DWORD address_);
    char ReadChar(DWORD address_);

    int GetPID()        { return m_process_id; }
    HANDLE GetPHandle() { return m_process_handle; }

private:
    int m_process_id;
    HANDLE m_process_handle;
};

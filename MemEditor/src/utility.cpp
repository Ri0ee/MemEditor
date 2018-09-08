#include "src/utility.h"

std::string FormatError(unsigned long error_code_)
{
    char* error_text;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  0, error_code_, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (char*)&error_text, 0, 0);

    std::string temp_string(error_text);
    LocalFree(error_text);

    return temp_string;
}

std::string FormatHex(std::vector<unsigned char> input_vector_)
{
    std::stringstream temp_sstream;

    for(auto it = input_vector_.begin(); it != input_vector_.end(); it++)
    {
        if((int)*it < 16)
            temp_sstream << "0";
        temp_sstream << std::hex << std::uppercase << (int)*it << std::nouppercase << std::dec << " ";
    }

    return std::string(temp_sstream.str());
}

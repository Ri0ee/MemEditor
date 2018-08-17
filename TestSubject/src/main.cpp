#include <iostream>
#include "windows.h"

char str_1[] = "TestStringOne";
char str_2[] = "TestStringTwo";

int main()
{
    char buffer[256] = {};
    strcat(buffer, str_1);
    strcat(buffer, str_2);

    MessageBox(0, buffer, "Test message box", MB_OK);

    std::cout << "str_1: " << str_1 << "\n";
    std::cout << "str_2: " << str_2 << "\n";

    return 0;
}

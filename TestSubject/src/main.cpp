#include "windows.h"

char str_1[] = "TestStringOne";
char str_2[] = "TestStringTwo";

int main()
{
    while(true)
    {
        MessageBox(0, str_1, str_2, MB_OK);
        MessageBox(0, (IsDebuggerPresent())?"Debugger present":"Debugger not present", "Message box", MB_OK);
    }

    return 0;
}

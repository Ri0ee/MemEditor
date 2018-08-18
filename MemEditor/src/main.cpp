#include "instance.h"

int main()
{
    Instance_class* instance = new Instance_class();
    if(instance->Init())
    {
        instance->Run();
    }
    delete instance;
    return 0;
}

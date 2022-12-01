#include "Stdafx.h"
#include "SystemClass.h"

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,    _In_ int nCmdShow)
{
    AllocConsole();
    FILE* input;
    freopen_s(&input, "CONOUT$", "wt", stdout);
    
    auto system = new SystemClass;
    if(!system)
    {
        return -1;
    }

    if(system->Initialize())
    {
        system->Run();
    }

    system->Shutdown();
    delete system;
    system = nullptr;

    fclose(input);
    return 0;
}

#include <string>

#include "event/Event.h"
#include "event/game/PlayerEvents.h"

#include "DotNetWrapper.h"
#include "ModInfo.h"

void setup() {
    log(LogType::Normal, "Setup completed for ", ModName);
    std::cout << "[.NET] Setup invoked." << std::endl;

    clrHost = new ClrHost();
    if (!clrHost->load())
    {
        return;
    }

    std::cout << "[.NET] clrhost loaded." << std::endl;

    if (clrHost->mainCallback() == nullptr)
    {
        return;
    }

    std::cout << "[.NET] found main callback." << std::endl;

    clrHost->mainCallback()();
}

GLOBAL void run(EventType type, std::vector<void*>& args) {
    dispatcher.post(type, args);
}

void stop()
{
    if (clrHost == nullptr) {
        return;
    }

    clrHost->unload();

    delete clrHost;
    clrHost = nullptr;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
std::cout << "DllMain invoked" << std::endl;

switch (dwReason)
{
case DLL_THREAD_ATTACH:
setup();
break;
case DLL_THREAD_DETACH:
stop();
break;
case DLL_PROCESS_ATTACH:
case DLL_PROCESS_DETACH:
break;
}

return TRUE;
}
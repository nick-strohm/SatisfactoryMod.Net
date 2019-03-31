#include <string>

#include "util/Utility.h"

#include "DotNetWrapper.h"
#include "ModInfo.h"

GLOBAL void setup() {
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
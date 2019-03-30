#include "ModInfo.h"
#include "Main.h"
#include <map>
#include <event/Event.h>
#include <event/FunctionHolder.h>
#include <util/Utility.h>

EXTERN_DLL_EXPORT std::string ModName = "SatisfactoryMod.Net";
EXTERN_DLL_EXPORT std::string ModVersion = "0.0.1";
EXTERN_DLL_EXPORT std::string ModDescription = "Test";
EXTERN_DLL_EXPORT std::string ModAuthors = "Nick Strohm";

void init()
{
	std::cout << "[SATISFACTORY.NET] Setup invoked." << std::endl;

	clrHost = new ClrHost();
	if (!clrHost->load())
	{
		return;
	}
	
	std::cout << "[SATISFACTORY.NET] clrhost loaded." << std::endl;
	
	if (clrHost->mainCallback() == nullptr)
	{
		return;
	}
	
	std::cout << "[SATISFACTORY.NET] found main callback." << std::endl;
	
	clrHost->mainCallback()();
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

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		init();
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		stop();
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}

	return TRUE;
}

EXTERN_DLL_EXPORT void recieve_hooked_functions(std::map<EventType, PVOID> functions)
{
	hookedFunctions = functions;
}
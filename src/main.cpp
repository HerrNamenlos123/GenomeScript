
#include "system.hpp"
#include "log.hpp"
#include "hooks.hpp"

namespace GenomeScript {

void dllattach() {
    log::enableDevMode();
    log::info("DLL attached");

    log::debug("Attaching detouring ...");
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    detour(false);
    DetourTransactionCommit();
    log::debug("Attaching detouring ... Done");

    log::debug("Loading all script modules ...");
    ScriptMaster::get().loadAllScripts();
    log::debug("Loading all script modules ... Done");
}

void dlldetach() {
    log::debug("Unloading all script modules ...");
    ScriptMaster::get().unloadAllScripts();
    log::debug("Unloading all script modules ... Done");

    log::debug("Detaching detouring ...");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    detour(true);
    DetourTransactionCommit();
    log::debug("Detaching detouring ... Done");
}

} // namespace GenomeScript

GS_API void dummy() {   // Needed to make DLL linkable
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        GenomeScript::dllattach();
        break;
    case DLL_PROCESS_DETACH:
        GenomeScript::dlldetach();
        break;
    }
    return true;
}
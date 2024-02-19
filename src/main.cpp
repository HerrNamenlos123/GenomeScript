
#include "system.hpp"
#include "log.hpp"

namespace GenomeScript {

void dllattach() {
    log::enableDevMode();
    log::info("DLL attached");

    log::info("Attaching detouring ...");
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    //detour(false);
    DetourTransactionCommit();
    log::info("Attaching detouring ... Done");

    ScriptMaster::get().loadAllScripts();
}

void dlldetach() {
    ScriptMaster::get().unloadAllScripts();

    log::info("Detaching detouring ...");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    //detour(true);
    DetourTransactionCommit();
    log::info("Detaching detouring ... Done");
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
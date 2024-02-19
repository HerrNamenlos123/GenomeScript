
#include "hooks.hpp"
#include "log.hpp"
#include "system.hpp"

namespace GenomeScript {

class MusicHook {
public:
    bool TriggerExplore() {
        HOOK_ACTION_BOOL("Music", TriggerExplore);
        return TriggerExplore_Base();
    }

    bool TriggerFight() {
        HOOK_ACTION_BOOL("Music", TriggerFight);
        return TriggerFight_Base();
    }

    bool TriggerRevolution() {
        HOOK_ACTION_BOOL("Music", TriggerRevolution);
        return TriggerRevolution_Base();
    }

    bool TriggerShowdown() {
        HOOK_ACTION_BOOL("Music", TriggerShowdown);
        return TriggerShowdown_Base();
    }

    bool TriggerSituation(const bCString& str) {
        HOOK_ACTION_BOOL("Music", TriggerSituation, str);
        return TriggerSituation_Base(str);
    }

    bool TriggerVictory() {
        HOOK_ACTION_BOOL("Music", TriggerVictory);
        return TriggerVictory_Base();
    }

    DETOUR_DECLARE_MEMBER(TriggerExplore, "Script.dll", "?TriggerExplore@Music@@QAE_NXZ")
    DETOUR_DECLARE_MEMBER(TriggerFight, "Script.dll", "?TriggerFight@Music@@QAE_NXZ")
    DETOUR_DECLARE_MEMBER(TriggerRevolution, "Script.dll", "?TriggerRevolution@Music@@QAE_NXZ")
    DETOUR_DECLARE_MEMBER(TriggerShowdown, "Script.dll", "?TriggerShowdown@Music@@QAE_NXZ")
    DETOUR_DECLARE_MEMBER(TriggerSituation, "Script.dll", "?TriggerSituation@Music@@QAE_NABVbCString@@@Z")
    DETOUR_DECLARE_MEMBER(TriggerVictory, "Script.dll", "?TriggerVictory@Music@@QAE_NXZ")

    static void detour(bool detach) {
        DETOUR_EXTERN_MEMBER(MusicHook, TriggerExplore);
        DETOUR_EXTERN_MEMBER(MusicHook, TriggerFight);
        DETOUR_EXTERN_MEMBER(MusicHook, TriggerRevolution);
        DETOUR_EXTERN_MEMBER(MusicHook, TriggerShowdown);
        DETOUR_EXTERN_MEMBER(MusicHook, TriggerSituation);
        DETOUR_EXTERN_MEMBER(MusicHook, TriggerVictory);
    }

    static void defineLuaTypes(lua_State* state) {
        luabridge::getGlobalNamespace(state)
            .beginClass<MusicHook>("Music")
            .addFunction("TriggerExplore", &MusicHook::TriggerExplore_Base<>)
            .addFunction("TriggerFight", &MusicHook::TriggerFight_Base<>)
            .addFunction("TriggerRevolution", &MusicHook::TriggerRevolution_Base<>)
            .addFunction("TriggerShowdown", &MusicHook::TriggerShowdown_Base<>)
            .addFunction("TriggerSituation", &MusicHook::TriggerSituation_Base<const bCString&>)
            .addFunction("TriggerVictory", &MusicHook::TriggerVictory_Base<>)
            .endClass();
    }
};

class eCApplicationHook {
public:
    bEResult Run() {
        HOOK_ACTION("eCApplication", Run, bEResult, bEResult::bEResult_Ok, bEResult::bEResult_False);
        return Run_Base();
    }

    void PlayVideo(const bCString& video) {
        HOOK_ACTION_VOID("eCApplication", PlayVideo, video);
        PlayVideo_Base(video);
    }

    void Process() {
        updateScriptHotreload();
        HOOK_ACTION_VOID("eCApplication", Process);
        Process_Base();
    }
    //GEBool IsPaused(GEBool);

    DETOUR_DECLARE_MEMBER(Run, "Engine.dll", "?Run@eCApplication@@QAE?AW4bEResult@@XZ");
    DETOUR_DECLARE_MEMBER(PlayVideo, "Engine.dll", "?PlayVideo@eCApplication@@QAEXABVbCString@@@Z");
    DETOUR_DECLARE_MEMBER(Process, "Engine.dll", "?Process@eCApplication@@QAEXXZ");
    //DETOUR_DECLARE_MEMBER(OnRun, "Engine.dll", "?OnRun@eCApplication@@UAEXXZ");

    static void detour(bool detach) {
        DETOUR_EXTERN_MEMBER(eCApplicationHook, Run);
        DETOUR_EXTERN_MEMBER(eCApplicationHook, PlayVideo);
        DETOUR_EXTERN_MEMBER(eCApplicationHook, Process);
        //DETOUR_EXTERN_MEMBER(eCApplicationHook, OnRun);
    }

    static void defineLuaTypes(lua_State* state) {
        luabridge::getGlobalNamespace(state)
            .beginClass<eCApplicationHook>("eCApplication")
            .addFunction("PlayVideo", &eCApplicationHook::PlayVideo_Base<const bCString&>)
            /*.ADD_FUNCTION_RETURN(Run)
            .ADD_FUNCTION_VOID_1P(PlayVideo, const bCString&)
            .ADD_FUNCTION_VOID(Run)*/
            .endClass();
    }
};

void detour(bool detach) {

    /*DETOUR_EXTERN_MEMBER(eCVirtualFileHook, Open);
    DETOUR_EXTERN_MEMBER(eCVisualAnimationLoDHook, Read);
    DETOUR_EXTERN_MEMBER(eCVisualAnimation_PSHook, Read);
    DETOUR_EXTERN_MEMBER(eCVisualAnimation_PSHook, PlayMotion);
    DETOUR_EXTERN_MEMBER(eCVisualAnimationFactoryHook, Read);
    DETOUR_EXTERN_MEMBER(eCWrapper_emfx2MotionHook, LoadMotion);
    DETOUR_EXTERN_MEMBER(EntityHook, DoDamage);
    DETOUR_EXTERN_MEMBER(EntityHook, GetDisplayName);*/

    MusicHook::detour(detach);
    eCApplicationHook::detour(detach);

    // DETOUR_EXTERN_MEMBER(eCVisualAnimation_PS, PlayMotion, "Engine.dll",
    // "?PlayMotion@eCVisualAnimation_PS@@QAEXW4eEMotionType@eCWrapper_emfx2Actor@@PAUeSMotionDesc@eCWrapper_emfx2Motion@@@Z");
    // DETOUR_EXTERN_MEMBER(eCVisualAnimationLoD, Read, "Engine.dll",
    // "?Read@eCVisualAnimationLoD@@UAE?AW4bEResult@@AAVbCIStream@@@Z"); DETOUR_EXTERN_MEMBER(eCVisualAnimation_PS,
    // Read, "Engine.dll", "?Read@eCVisualAnimation_PS@@UAE?AW4bEResult@@AAVbCIStream@@@Z");
    // DETOUR_EXTERN_MEMBER(eCVirtualFile, Open, "Engine.dll",
    // "?Open@eCVirtualFile@@UAE_NABVbCString@@W4bEFileCreationMode@@E@Z");

    // DETOUR_EXTERN_MEMBER(bCFile, Open, "SharedBase.dll",
    // "?Open@bCFile@@UAE_NABVbCString@@W4bEFileCreationMode@@@Z"); DETOUR_EXTERN_MEMBER(eCVisualAnimationFactory,
    // GetMainActorFileName, "Engine.dll", "?GetMainActorFileName@eCVisualAnimationFactory@@QBEABVbCString@@XZ");
    // DETOUR_EXTERN_MEMBER(eCVisualAnimationFactory, SetMainActorFileName, "Engine.dll",
    // "?SetMainActorFileName@eCVisualAnimationFactory@@QAEXABVbCString@@@Z");
    // DETOUR_EXTERN_MEMBER(eCVisualAnimationFactory, Read, "Engine.dll",
    // "?Read@eCVisualAnimationFactory@@QAE?AW4bEResult@@AAVbCIStream@@@Z"); DETOUR_EXTERN_MEMBER(eCApplication,
    // GetInstance, "Engine.dll", "?GetInstance@eCApplication@@SGAAV1@XZ"); DETOUR_EXTERN_MEMBER(eCApplication,
    // GetCurrentCamera, "Engine.dll", "?GetCurrentCamera@eCApplication@@QAEAAVeCCameraBase@@XZ");
    // DETOUR_EXTERN_MEMBER(eCModuleAdmin, Process, "Engine.dll", "?Process@eCModuleAdmin@@QAEXXZ");
    // DETOUR_EXTERN_MEMBER(eCModuleAdmin, PostProcess, "Engine.dll", "?PostProcess@eCModuleAdmin@@QAEXXZ");
    // DETOUR_EXTERN_MEMBER(eCEntityAdmin, Process, "Engine.dll", "?Process@eCEntityAdmin@@QAEXXZ");
    // DETOUR_EXTERN_MEMBER(eCEntityAdmin, EnableProcessing, "Engine.dll",
    // "?EnableProcessing@eCEntityAdmin@@QAEX_N@Z"); DETOUR_EXTERN_MEMBER(eCEntityAdmin, UpdateProcessingRangeEntity,
    // "Engine.dll",
    // "?UpdateProcessingRangeEntity@eCEntityAdmin@@QAEXPAVeCEntity@@AAV?$bTSmallArray@PAVeCEntity@@@@1@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, EnterProcessingRange, "Engine.dll", "?EnterProcessingRange@eCEntity@@IAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, ExitProcessingRange, "Engine.dll", "?ExitProcessingRange@eCEntity@@IAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, PreProcess, "Engine.dll", "?PreProcess@eCEntity@@QAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, Process, "Engine.dll", "?Process@eCEntity@@QAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, PostProcess, "Engine.dll", "?PostProcess@eCEntity@@QAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, HasProcessingRangeEntered, "Engine.dll",
    // "?HasProcessingRangeEntered@eCEntity@@QBE_NXZ"); DETOUR_EXTERN_MEMBER(eCEntity, IsEnabled, "Engine.dll",
    // "?IsEnabled@eCEntity@@QBE_NXZ"); DETOUR_EXTERN_MEMBER(eCPVSPrefetcher3, GetNonDeactivationCell, "Engine.dll",
    // "?GetNonDeactivationCell@eCPVSPrefetcher3@@QAEPAVeCPVSCellItem@@XZ"); DETOUR_EXTERN_MEMBER(bCTimer,
    // GetTimeStamp, "SharedBase.dll", "?GetTimeStamp@bCTimer@@SGKXZ"); DETOUR_EXTERN_MEMBER(bCMemoryAdmin,
    // GetInstance, "SharedBase.dll", "?GetInstance@bCMemoryAdmin@@SGAAV1@XZ");
}

void defineLuaTypes(lua_State* state) {
    luabridge::getGlobalNamespace(state)
        .beginClass<bCString>("bCString")
        .addConstructor([](void* ptr, const std::string& str) {
            bCString* bcstr = new(ptr) bCString();
            bcstr->SetText(str.c_str());
            return bcstr;
        })
        .endClass();

    luabridge::getGlobalNamespace(state)
        .addFunction("String", [](const bCString& str) { return std::string(str.GetText()); });

    MusicHook::defineLuaTypes(state);
    eCApplicationHook::defineLuaTypes(state);
}

} // namespace GenomeScript

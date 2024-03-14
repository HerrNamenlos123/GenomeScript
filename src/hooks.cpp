
#include "hooks.hpp"
#include "log.hpp"
#include "system.hpp"

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 1024
#include "magic_enum/magic_enum.hpp"

enum eCWrapper_emfx2Actor::eEMotionType {};

/*class GE_DLLIMPORT eCWrapper_emfx2Motion::eSMotionDesc
        {
            public:
                enum eEMotionOwner;

            public:                                 ~eSMotionDesc( void );
                                                     eSMotionDesc( eSMotionDesc const & );
                                                     eSMotionDesc( bCString const & );
                                                     eSMotionDesc( eCResourceAnimationMotion_PS * );
                                                     eSMotionDesc( eCResourceDataEntity * );
                                                     eSMotionDesc( void );
                eSMotionDesc const &                 operator =  ( eSMotionDesc const & );
                bCOStream &                          operator >> ( bCOStream & );
                bCIStream &                          operator << ( bCIStream & );
                eCResourceAnimationMotion_PS *       GetMotion( void );
                eCResourceAnimationMotion_PS const * GetMotion( void ) const;
                bCString const &                     GetMotionFilename( void ) const;   
                eEMotionOwner                        GetMotionOwner( void ) const;
                GEBool                               IsValid( void ) const;
                void                                 SetMotion( bCString const & );
                void                                 SetMotion( eCResourceAnimationMotion_PS * );
                void                                 SetMotion( eCResourceDataEntity * );
                void                                 SetMotionOwner( eEMotionOwner );
        };*/

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
    // DETOUR_DECLARE_MEMBER(OnRun, "Engine.dll", "?OnRun@eCApplication@@UAEXXZ");

    static void detour(bool detach) {
        DETOUR_EXTERN_MEMBER(eCApplicationHook, Run);
        DETOUR_EXTERN_MEMBER(eCApplicationHook, PlayVideo);
        DETOUR_EXTERN_MEMBER(eCApplicationHook, Process);
        // DETOUR_EXTERN_MEMBER(eCApplicationHook, OnRun);
    }

    static void defineLuaTypes(lua_State* state) {
        luabridge::getGlobalNamespace(state)
            .beginClass<eCApplicationHook>("eCApplication")
            .addFunction("Run", &eCApplicationHook::Run_Base<>)
            .addFunction("PlayVideo", &eCApplicationHook::PlayVideo_Base<const bCString&>)
            .addFunction("Process", &eCApplicationHook::Process_Base<>)
            .endClass();
    }
};

struct sAICombatMoveInstr_Args {
    eCEntity* SelfEntity;
    eCEntity* TargetEntity;
    gEAction Action;
    bCString PhaseName;
    GEFloat AniSpeedScale;

    sAICombatMoveInstr_Args(eCEntity* a_SelfEntity,
                            eCEntity* a_TargetEntity,
                            gEAction a_Action,
                            bCString a_Phase,
                            GEFloat a_AniSpeedScale = 1.0f)
        : SelfEntity(a_SelfEntity)
        , TargetEntity(a_TargetEntity)
        , Action(a_Action)
        , PhaseName(a_Phase)
        , AniSpeedScale(a_AniSpeedScale) {
    }
};

class gCScriptProcessingUnitHook {
public:
    void AISetState(bCString str) {
        log::info("AISetState");
        HOOK_ACTION_VOID("gCScriptProcessingUnit", AISetState, str);
        AISetState_Base(str);
    }

    void AISetTask(bCString a_strTaskName, GEBool a_bJustSetIfCaughtTask) {
        // This sets a task that any NPC in the world is doing. For example:
        // ZS_HangAround or PS_Melee or PS_Interact or PS_Interact_End.
        // But nothing inbetween, the animation itself is done elsewhere.
        //log::info("AISetTask = {}", a_strTaskName);
        AISetTask_Base(a_strTaskName, a_bJustSetIfCaughtTask);
    }

    static GEBool GE_STDCALL sAICombatMoveInstr(GELPVoid a_pArgs, gCScriptProcessingUnit* a_pSPU, GEBool a_bFullStop) {
        auto args = reinterpret_cast<sAICombatMoveInstr_Args*>(a_pArgs);
        HOOK_STATIC_ACTION_BOOL("gCScriptProcessingUnit", sAICombatMoveInstr, args, a_pSPU, a_bFullStop);
        return sAICombatMoveInstr_Base(a_pArgs, a_pSPU, a_bFullStop);
    }

    DETOUR_DECLARE_MEMBER(AISetState, "Game.dll", "?AISetState@gCScriptProcessingUnit@@QAEXVbCString@@@Z");
    DETOUR_DECLARE_MEMBER(AISetTask, "Game.dll", "?AISetTask@gCScriptProcessingUnit@@QAEXVbCString@@_N@Z");
    DETOUR_DECLARE_STATIC_MEMBER(sAICombatMoveInstr, "Game.dll", "?sAICombatMoveInstr@gCScriptProcessingUnit@@SG_NPAXPAV1@_N@Z");

    static void detour(bool detach) {
        DETOUR_EXTERN_MEMBER(gCScriptProcessingUnitHook, AISetState);
        DETOUR_EXTERN_MEMBER(gCScriptProcessingUnitHook, AISetTask);
        DETOUR_EXTERN_MEMBER(gCScriptProcessingUnitHook, sAICombatMoveInstr);
    }

    static void defineLuaTypes(lua_State* state) {
        luabridge::getGlobalNamespace(state)
            .beginClass<sAICombatMoveInstr_Args>("sAICombatMoveInstr_Args")
            .addProperty("Action", &sAICombatMoveInstr_Args::Action)
            .addProperty("PhaseName", &sAICombatMoveInstr_Args::PhaseName)
            .addProperty("AniSpeedScale", &sAICombatMoveInstr_Args::AniSpeedScale)
            .endClass();

        luabridge::getGlobalNamespace(state)
            .beginClass<gCScriptProcessingUnit>("gCScriptProcessingUnit")
            .endClass();
        
        luabridge::getGlobalNamespace(state)
            .beginClass<gCScriptProcessingUnitHook>("gCScriptProcessingUnit")
            .addFunction("AISetState", &gCScriptProcessingUnitHook::AISetState_Base<bCString>)
            .addFunction("AISetTask", &gCScriptProcessingUnitHook::AISetTask_Base<bCString, GEBool>)
            .addStaticFunction("sAICombatMoveInstr",
                &gCScriptProcessingUnitHook::sAICombatMoveInstr_Base<GELPVoid, gCScriptProcessingUnit*, GEBool>)
            .endClass();
    }
};

class eCWrapper_emfx2MotionHook {
public:
    bEResult LoadMotion(eCArchiveFile& file) {
        log::error("LoadMotion");
        HOOK_ACTION("eCWrapper_emfx2Motion", LoadMotion, bEResult, bEResult::bEResult_Ok, bEResult::bEResult_False, file);
        return LoadMotion_Base(file);
    }

    DETOUR_DECLARE_MEMBER(LoadMotion, "Engine.dll", "?LoadMotion@eCWrapper_emfx2Motion@@QAE?AW4bEResult@@AAVeCArchiveFile@@@Z");

    static void detour(bool detach) {
        DETOUR_EXTERN_MEMBER(eCWrapper_emfx2MotionHook, LoadMotion);
    }

    static void defineLuaTypes(lua_State* state) {
        luabridge::getGlobalNamespace(state)
            .beginClass<eCWrapper_emfx2MotionHook>("eCWrapper_emfx2Motion")
            .addFunction("PlayMotion", &eCWrapper_emfx2MotionHook::LoadMotion_Base<eCArchiveFile>)
            .endClass();
    }
};

class eCVisualAnimation_PSHook : public eCVisualAnimation_PS {
public:
    void PlayMotion(eCWrapper_emfx2Actor::eEMotionType type, eCWrapper_emfx2Motion::eSMotionDesc* motion) {
        HOOK_ACTION_VOID("eCVisualAnimation_PS", PlayMotion, type, motion);
        PlayMotion_Base(type, motion);
    }

    DETOUR_DECLARE_MEMBER(PlayMotion, "Engine.dll", "?PlayMotion@eCVisualAnimation_PS@@QAEXW4eEMotionType@eCWrapper_emfx2Actor@@PAUeSMotionDesc@eCWrapper_emfx2Motion@@@Z");

    static void detour(bool detach) {
        DETOUR_EXTERN_MEMBER(eCVisualAnimation_PSHook, PlayMotion);
    }

    static void defineLuaTypes(lua_State* state) {
        luabridge::getGlobalNamespace(state)
            .beginClass<eCWrapper_emfx2Actor::eEMotionType>("eEMotionType")
            .endClass();
        luabridge::getGlobalNamespace(state)
            .beginClass<eCWrapper_emfx2Motion::eSMotionDesc>("eSMotionDesc")
            //.addFunction("GetMotionFilename", &eCWrapper_emfx2Motion::eSMotionDesc::GetMotionFilename)
            .endClass();

        luabridge::getGlobalNamespace(state)
            .beginClass<eCVisualAnimation_PSHook>("eCVisualAnimation_PS")
            .addFunction("PlayMotion", &eCVisualAnimation_PSHook::PlayMotion_Base<eCWrapper_emfx2Actor::eEMotionType, eCWrapper_emfx2Motion::eSMotionDesc*>)
            .addFunction("GetResourceFilePath", &eCVisualAnimation_PSHook::GetResourceFilePath<>)
            .endClass();
    }
};

void detour(bool detach) {

    /*DETOUR_EXTERN_MEMBER(eCVirtualFileHook, Open);
    DETOUR_EXTERN_MEMBER(eCVisualAnimationLoDHook, Read);
    DETOUR_EXTERN_MEMBER(eCVisualAnimation_PSHook, Read);
    DETOUR_EXTERN_MEMBER(eCVisualAnimationFactoryHook, Read);
    DETOUR_EXTERN_MEMBER(EntityHook, DoDamage);
    DETOUR_EXTERN_MEMBER(EntityHook, GetDisplayName);*/

    MusicHook::detour(detach);
    eCApplicationHook::detour(detach);
    gCScriptProcessingUnitHook::detour(detach);
    //eCVisualAnimation_PSHook::detour(detach);
    eCWrapper_emfx2MotionHook::detour(detach);

    // DETOUR_EXTERN_MEMBER(eCVisualAnimationLoD, Read, "Engine.dll", "?Read@eCVisualAnimationLoD@@UAE?AW4bEResult@@AAVbCIStream@@@Z"); 
    // DETOUR_EXTERN_MEMBER(eCVisualAnimation_PS, Read, "Engine.dll", "?Read@eCVisualAnimation_PS@@UAE?AW4bEResult@@AAVbCIStream@@@Z");
    // DETOUR_EXTERN_MEMBER(eCVirtualFile, Open, "Engine.dll", "?Open@eCVirtualFile@@UAE_NABVbCString@@W4bEFileCreationMode@@E@Z");

    // DETOUR_EXTERN_MEMBER(bCFile, Open, "SharedBase.dll", "?Open@bCFile@@UAE_NABVbCString@@W4bEFileCreationMode@@@Z"); 
    // DETOUR_EXTERN_MEMBER(eCVisualAnimationFactory, GetMainActorFileName, "Engine.dll", "?GetMainActorFileName@eCVisualAnimationFactory@@QBEABVbCString@@XZ");
    // DETOUR_EXTERN_MEMBER(eCVisualAnimationFactory, SetMainActorFileName, "Engine.dll", "?SetMainActorFileName@eCVisualAnimationFactory@@QAEXABVbCString@@@Z");
    // DETOUR_EXTERN_MEMBER(eCVisualAnimationFactory, Read, "Engine.dll", "?Read@eCVisualAnimationFactory@@QAE?AW4bEResult@@AAVbCIStream@@@Z"); 
    // DETOUR_EXTERN_MEMBER(eCApplication, GetInstance, "Engine.dll", "?GetInstance@eCApplication@@SGAAV1@XZ"); 
    // DETOUR_EXTERN_MEMBER(eCApplication, GetCurrentCamera, "Engine.dll", "?GetCurrentCamera@eCApplication@@QAEAAVeCCameraBase@@XZ");
    // DETOUR_EXTERN_MEMBER(eCModuleAdmin, Process, "Engine.dll", "?Process@eCModuleAdmin@@QAEXXZ");
    // DETOUR_EXTERN_MEMBER(eCModuleAdmin, PostProcess, "Engine.dll", "?PostProcess@eCModuleAdmin@@QAEXXZ");
    // DETOUR_EXTERN_MEMBER(eCEntityAdmin, Process, "Engine.dll", "?Process@eCEntityAdmin@@QAEXXZ");
    // DETOUR_EXTERN_MEMBER(eCEntityAdmin, EnableProcessing, "Engine.dll", "?EnableProcessing@eCEntityAdmin@@QAEX_N@Z"); 
    // DETOUR_EXTERN_MEMBER(eCEntityAdmin, UpdateProcessingRangeEntity, "Engine.dll", "?UpdateProcessingRangeEntity@eCEntityAdmin@@QAEXPAVeCEntity@@AAV?$bTSmallArray@PAVeCEntity@@@@1@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, EnterProcessingRange, "Engine.dll", "?EnterProcessingRange@eCEntity@@IAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, ExitProcessingRange, "Engine.dll", "?ExitProcessingRange@eCEntity@@IAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, PreProcess, "Engine.dll", "?PreProcess@eCEntity@@QAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, Process, "Engine.dll", "?Process@eCEntity@@QAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, PostProcess, "Engine.dll", "?PostProcess@eCEntity@@QAEX_N@Z");
    // DETOUR_EXTERN_MEMBER(eCEntity, HasProcessingRangeEntered, "Engine.dll", "?HasProcessingRangeEntered@eCEntity@@QBE_NXZ"); 
    // DETOUR_EXTERN_MEMBER(eCEntity, IsEnabled, "Engine.dll", "?IsEnabled@eCEntity@@QBE_NXZ"); 
    // DETOUR_EXTERN_MEMBER(eCPVSPrefetcher3, GetNonDeactivationCell, "Engine.dll", "?GetNonDeactivationCell@eCPVSPrefetcher3@@QAEPAVeCPVSCellItem@@XZ"); 
    // DETOUR_EXTERN_MEMBER(bCTimer, GetTimeStamp, "SharedBase.dll", "?GetTimeStamp@bCTimer@@SGKXZ"); 
    // DETOUR_EXTERN_MEMBER(bCMemoryAdmin, GetInstance, "SharedBase.dll", "?GetInstance@bCMemoryAdmin@@SGAAV1@XZ");
}

void defineLuaTypes(lua_State* state) {
    
    luabridge::getGlobalNamespace(state)
        .beginClass<bCString>("bCString")
        .addConstructor([](void* ptr, const std::string& str) {
            bCString* bcstr = new(ptr) bCString();
            bcstr->SetText(str.c_str());
            return bcstr;
        })
        .addFunction("GetText", &bCString::GetText)
        .endClass();
    
    luabridge::getGlobalNamespace(state)
        .beginClass<bCAnimationResourceString>("bCAnimationResourceString")
        .addConstructor([](void* ptr, const std::string& str) {
            bCAnimationResourceString* bcstr = new(ptr) bCAnimationResourceString();
            bcstr->SetText(str.c_str());
            return bcstr;
        })
        .addFunction("GetText", &bCAnimationResourceString::GetText)
        .endClass();
    
    luabridge::getGlobalNamespace(state)
        .beginClass<eCArchiveFile>("eCArchiveFile")
        .addProperty("m_strFileName", &eCArchiveFile::m_strFileName)
        .addProperty("m_bOpenForWrite", &eCArchiveFile::m_bOpenForWrite)
        .endClass();

    DEFINE_LUA_ENUM(gEAction, gEAction_None);
        
    luabridge::getGlobalNamespace(state)
        .addFunction("String", [](gEAction action) { return std::to_string((int)action); })
        .addFunction("Int", [](gEAction action) { return (int)action; })
        .addFunction("reinterpret_voidptr", [](sAICombatMoveInstr_Args* args) { return reinterpret_cast<GELPVoid>(args); });

    MusicHook::defineLuaTypes(state);
    eCApplicationHook::defineLuaTypes(state);
    gCScriptProcessingUnitHook::defineLuaTypes(state);
    //eCVisualAnimation_PSHook::defineLuaTypes(state);
    eCWrapper_emfx2MotionHook::defineLuaTypes(state);
}

} // namespace GenomeScript

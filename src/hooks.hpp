#pragma once

#include "pch.hpp"
#include "system.hpp"
#include "log.hpp"

auto constexpr MAX_FUNCTIONCALL_MILLISECONDS = 50;

#define DETOUR_DECLARE_MEMBER(function, dll, symbol)                                                                  \
    inline static auto function##_funcptr = getDllFunc<decltype(&function)>(dll, symbol);                             \
    template<typename... TArgs> auto function##_Base(TArgs... args) {                                                 \
        return (this->*function##_funcptr)(args...);                                                                  \
    }

#define DETOUR_DECLARE_STATIC_MEMBER(function, dll, symbol)                                                           \
    inline static auto function##_funcptr = getDllFunc<decltype(&function)>(dll, symbol);                             \
    template <typename... TArgs> static auto function##_Base(TArgs... args) {                                         \
        return function##_funcptr(args...);                                                                           \
    }

#define DETOUR_EXTERN_MEMBER(classname, function)                                                                     \
    auto classname##_##function##_tmp = &classname::function;                                                         \
    Detour(detach, &(PVOID&)classname::function##_funcptr, *(PBYTE*)&classname##_##function##_tmp);

#define SILENT_TRACE(...) ScopedTimerWarner __t(__FUNCTION__);
#define TRACE(...)                                                                                                    \
    ScopedTimerWarner __t(__FUNCTION__);                                                                              \
    LOG(__FUNCTION__ "() {}", fmt::format(__VA_ARGS__))
#define SCOPE_TRACE(...)                                                                                              \
    ScopedTracePrinter __st(__FUNCTION__);                                                                            \
    TRACE(__VA_ARGS__)

#define BREAK() MessageBoxA(nullptr, __FUNCTION__, "break", MB_OK)
#define BREAK_MSG(...)                                                                                                \
    MessageBoxA(nullptr, fmt::format(__FUNCTION__ ": {}", fmt::format(__VA_ARGS__)).c_str(), "break", MB_OK)

#define ADD_FUNCTION_VOID(function)                                                                                   \
    addFunction(#function, [&]() { (m_this->*function##_funcptr)(); })
#define ADD_FUNCTION_VOID_1P(function, T1)                                                                        \
    addFunction(#function, [&](T1 p1) { (m_this->*function##_funcptr)(p1); })
#define ADD_FUNCTION_VOID_2P(function, T1, T2)                                                                    \
    addFunction(#function, [&](T1 p1, T2 p2) { (m_this->*function##_funcptr)(p1, p2); })
#define ADD_FUNCTION_VOID_3P(function, T1, T2, T3)                                                                \
    addFunction(#function, [&](T1 p1, T2 p2, T3 p3) { (m_this->*function##_funcptr)(p1, p2, p3); })

#define ADD_FUNCTION_RETURN(function) addFunction(#function, [&]() { return (m_this->*function##_funcptr)(); })
#define ADD_FUNCTION_RETURN_1P(function, T1)                                                                          \
    addFunction(#function, [&](T1 p1) { return (m_this->*function##_funcptr)(p1); })
#define ADD_FUNCTION_RETURN_2P(function, T1, T2)                                                                      \
    addFunction(#function, [&](T1 p1, T2 p2) { return (m_this->*function##_funcptr)(p1, p2); })
#define ADD_FUNCTION_RETURN_3P(function, T1, T2, T3)                                                                  \
    addFunction(#function, [&](T1 p1, T2 p2, T3 p3) { return (m_this->*function##_funcptr)(p1, p2, p3); })

#define HOOK_ACTION_RET(classname, funcname, returntype, success, failure)                                            \
    if (hookAction == HookAction::PreventDefaultAsSuccess) {                                                          \
        return success;                                                                                               \
    }                                                                                                                 \
    else if (hookAction == HookAction::PreventDefaultAsFailure) {                                                     \
        return failure;                                                                                               \
    }  

#define HOOK_ACTION_RET_VALUE(classname, funcname, returntype, success, failure)                                      \
    else if (hookAction == HookAction::PreventDefaultWithValue) {                                                     \
        if (!result) {                                                                                                \
            log::error("Function {} used PreventDefaultWithValue, but did not return a value",                        \
                       #classname##"_" #funcname);                                                                     \
        }                                                                                                             \
        else {                                                                                                        \
            if (result->isInstance<returntype>()) {                                                                   \
                return result->cast<returntype>().value();                                                            \
            }                                                                                                         \
            else {                                                                                                    \
                log::error("Function {} returned an invalid datatype", #classname##"_" #funcname);                     \
            }                                                                                                         \
        }                                                                                                             \
    }

#define HOOK_ACTION_VOID(classname, funcname, ...)                                                                    \
    auto [hookAction, result]                                                                                         \
        = ScriptMaster::get().callFunctionInAllScripts(#classname##"_" #funcname, this, __VA_ARGS__);                  \
    HOOK_ACTION_RET(classname, funcname, returntype,,);          

#define HOOK_ACTION(classname, funcname, returntype, success, failure, ...)                                           \
    auto [hookAction, result]                                                                                         \
        = ScriptMaster::get().callFunctionInAllScripts(#classname##"_" #funcname, this, __VA_ARGS__);                  \
    HOOK_ACTION_RET(classname, funcname, returntype, success, failure)                                                \
    HOOK_ACTION_RET_VALUE(classname, funcname, returntype, success, failure);

#define HOOK_ACTION_BOOL(classname, funcname, ...)                                                                    \
    HOOK_ACTION(classname, funcname, bool, true, false, __VA_ARGS__)

#define HOOK_STATIC_ACTION_VOID(classname, funcname, ...)                                                             \
    auto [hookAction, result] = ScriptMaster::get().callFunctionInAllScripts(#classname##"_" #funcname, __VA_ARGS__);  \
    HOOK_STATIC_ACTION_RET(classname, funcname, returntype, , );

#define HOOK_STATIC_ACTION(classname, funcname, returntype, success, failure, ...)                                    \
    auto [hookAction, result] = ScriptMaster::get().callFunctionInAllScripts(#classname##"_" #funcname, __VA_ARGS__);  \
    HOOK_ACTION_RET(classname, funcname, returntype, success, failure)                                         \
    HOOK_ACTION_RET_VALUE(classname, funcname, returntype, success, failure);

#define HOOK_STATIC_ACTION_BOOL(classname, funcname, ...)                                                             \
    HOOK_STATIC_ACTION(classname, funcname, bool, true, false, __VA_ARGS__)

#define DEFINE_LUA_ENUM(enumType, defaultEnumValue)                                                                   \
    luabridge::getGlobalNamespace(state)                                                                              \
        .beginClass<enumType>(#enumType)                                                                              \
        .addConstructor([](void* ptr, int value) {                                                                    \
            enumType* v = reinterpret_cast<enumType*>(new (ptr) int);                                                 \
            *v = magic_enum::enum_cast<enumType>(value).value_or(defaultEnumValue);                                   \
            return v;                                                                                                 \
        })                                                                                                            \
        .endClass();                                                                                                  \
    /*for (auto [enumValue, enumKey] : magic_enum::enum_entries<enumType>()) {                                          \
        luabridge::getGlobalNamespace(state).addVariable(enumKey.data(), enumValue);                                  \
    }*/

namespace GenomeScript {
    
class ScopedTracePrinter {
public:
    ScopedTracePrinter(const std::string& func)
        : func(func) {
        log::trace("{} Start", func);
    }

    ~ScopedTracePrinter() {
        log::trace("{} End", func);
    }

private:
    std::string func;
};

class ScopedTimerWarner {
public:
    ScopedTimerWarner(const std::string& func)
        : func(func) {
        start = std::chrono::high_resolution_clock::now();
    }

    ~ScopedTimerWarner() {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        if (elapsed > MAX_FUNCTIONCALL_MILLISECONDS) {
            log::error("WARNING: {} took more than {}ms: Took {}ms", func, MAX_FUNCTIONCALL_MILLISECONDS, elapsed);
        }
    }

private:
    std::string func;
    std::chrono::steady_clock::time_point start;
};

inline std::string threadid() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

static HINSTANCE loadDll(const std::string& dll) {
    static std::unordered_map<std::string, HINSTANCE> loadedDlls;
    if (loadedDlls.find(dll) != loadedDlls.end()) {
        return loadedDlls[dll];
    }

    HINSTANCE handle = LoadLibraryW(widen(dll).c_str());
    if (!handle) {
        std::cerr << "Failed to load DLL Importer - Original.dll" << std::endl;
        abort();
        return nullptr;
    }
    loadedDlls[dll] = handle;
    return handle;
}

template <typename T> T getDllFunc(const char* dll, const char* symbolName) {
    FARPROC raw = GetProcAddress(loadDll(dll), symbolName);
    if (raw == nullptr) {
        BREAK_MSG("{} was not found in {}", symbolName, dll);
        std::terminate();
    }
    T f;
    *(FARPROC*)&f = raw;
    return f;
}

template <typename... TArgs> inline LONG Detour(bool detach, TArgs&&... args) {
    if (!detach) {
        return DetourAttach(std::forward<TArgs>(args)...);
    }
    else {
        return DetourDetach(std::forward<TArgs>(args)...);
    }
}

void detour(bool detach);
void defineLuaTypes(lua_State* state);

} // namespace GenomeScript

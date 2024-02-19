
#include "system.hpp"
#include "log.hpp"
#include "hooks.hpp"

namespace GenomeScript {

std::wstring widen(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string narrow(const std::wstring& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(str);
}

double time() {
    using namespace std::chrono;
    static auto program_start = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<microseconds>(high_resolution_clock::now() - program_start);
    return static_cast<double>(duration.count()) / 1000000.0;
}

ScriptMaster::~ScriptMaster() {
    unloadAllScripts();
}

void ScriptMaster::loadAllScripts() {
    const std::string scriptDirectory = "GenomeScript/";

    // We loop through all folders in the script directory
    using iter = std::filesystem::directory_iterator;
    for (auto& dirEntry : iter(scriptDirectory)) {
        // In every folder, we call the main script
        const auto& mainFile = dirEntry / "main.lua";
        const auto moduleName = dirEntry.path().filename();
        loadScript(mainFile.u8string(), moduleName.u8string());
    }
}

void ScriptMaster::unloadAllScripts() {
    while (m_scripts.size() > 0) {
        unloadScript(m_scripts.back().moduleName);
    }
}

void ScriptMaster::loadScript(const std::string& filename, const std::string& moduleName) {
    m_scripts.emplace_back();

    Script& script = m_scripts.back();
    script.luaState = luaL_newstate();
    script.filepath = filename;
    script.moduleName = moduleName;
    luaL_openlibs(script.luaState);

    const auto directory = script.filepath.parent_path();
    ExecLuaCode(script, fmt::format("package.path = package.path .. \";\" .. \"{}/?.lua\"", directory.u8string()));

    defineLuaTypes(script);
    luabridge::getGlobalNamespace(script.luaState)
        .addFunction("PreventDefaultAsSuccess", [&script]() { script.hookAction = HookAction::PreventDefaultAsSuccess; })
        .addFunction("PreventDefaultAsFailure", [&script]() { script.hookAction = HookAction::PreventDefaultAsFailure; })
        .addFunction("PreventDefaultWithValue", [&script]() { script.hookAction = HookAction::PreventDefaultWithValue; });

    loadLuaFile(script);
    
    callLuaFunction(script, m_luaOnAttachFunction);
}

void ScriptMaster::loadLuaFile(Script& script) {
    std::ifstream file(widen(script.filepath.u8string()).c_str(), std::ios::in);
    if (file.fail()) {
        log::error("Failed to load script '{}'", script.filepath.u8string());
        return;
    }

    script.lastWriteTime = std::filesystem::last_write_time(script.filepath);
    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    ExecLuaCode(script, code);
}

bool ScriptMaster::unloadScript(const std::string& moduleName) {
    for (int i = 0; i < m_scripts.size(); i++) {
        auto& script = m_scripts[i];
        if (script.moduleName == moduleName) {
            callLuaFunction(script, m_luaOnDetachFunction);
            lua_close(script.luaState);
            m_scripts.erase(m_scripts.begin() + i);
            return true;
        }
    }
    return false;
}

void ScriptMaster::ExecLuaCode(const Script& script, const std::string& code) {
    if (luaL_dostring(script.luaState, code.c_str()) != 0) {
        log::error("{}: {}", script.filepath.u8string(), lua_tostring(script.luaState, -1));
        lua_pop(script.luaState, 1);
    }
}

void ScriptMaster::defineLuaTypes(const Script& script) {
    log::defineLuaTypes(script.luaState, script.moduleName);
    GenomeScript::defineLuaTypes(script.luaState);
}

void ScriptMaster::updateScriptHotreload() {
    for (auto& script : m_scripts) {
        auto last = std::filesystem::last_write_time(script.filepath);
        if (last != script.lastWriteTime) {
            log::info("Hot-reloading changed script {} ...", script.filepath.u8string());
            loadLuaFile(script);
            log::info("Hot-reloading changed script ... Done");
        }
    }
}

void updateScriptHotreload() {
    if (!log::devModeEnabled()) {
        return;
    }

    static auto prevUpdateTime = time();
    auto now = time();
    if ((now - prevUpdateTime) > HOTRELOAD_INTERVAL_S) {
        prevUpdateTime = now;

        ScriptMaster::get().updateScriptHotreload();
    }
}

} // namespace GenomeScript

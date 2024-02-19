
#include "log.hpp"

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>

namespace GenomeScript {

void log::defineLuaTypes(lua_State* state, const std::string& moduleName) {
    luabridge::getGlobalNamespace(state)
        .beginNamespace("log")
        .addFunction("trace", [&](const std::string& msg) { log::trace("[{}] {}", moduleName, msg); })
        .addFunction("debug", [&](const std::string& msg) { log::debug("[{}] {}", moduleName, msg); })
        .addFunction("info", [&](const std::string& msg) { log::info("[{}] {}", moduleName, msg); })
        .addFunction("warn", [&](const std::string& msg) { log::warn("[{}] {}", moduleName, msg); })
        .addFunction("error", [&](const std::string& msg) { log::error("[{}] {}", moduleName, msg); })
        .endNamespace();
}

} // namespace GenomeScript

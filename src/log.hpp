#pragma once

#include "common.hpp"

namespace GenomeScript {

class log {
public:

    template<typename... TArgs> static void trace(TArgs... args) {
        if (m_consoleEnabled) {
            init();
            spdlog::trace(args...);
        }
    }

    template <typename... TArgs> static void info(TArgs... args) {
        if (m_consoleEnabled) {
            init();
            spdlog::info(args...);
        }
    }

    template <typename... TArgs> static void debug(TArgs... args) {
        if (m_consoleEnabled) {
            init();
            spdlog::debug(args...);
        }
    }

    template <typename... TArgs> static void warn(TArgs... args) {
        if (m_consoleEnabled) {
            init();
            spdlog::warn(args...);
        }
    }

    template <typename... TArgs> static void error(TArgs... args) {
        if (m_consoleEnabled) {
            init();
            spdlog::error(args...);
        }
    }

    static void consoleEnabled() {
        m_consoleEnabled = true;
    }

private:
    log() = default;

    static void init() {
        if (!m_logger) {
            m_logger = spdlog::basic_logger_mt("G3", "logs/GenomeScript-last.txt", true);
        }
    }

    inline static bool m_consoleEnabled = false;
    inline static std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace GenomeScript

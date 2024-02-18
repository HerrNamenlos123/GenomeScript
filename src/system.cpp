
#include "system.hpp"
#include "log.hpp"

namespace GenomeScript {

void CreateConsole() {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    log::consoleEnabled();
}

} // namespace GenomeScript

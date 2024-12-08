#include "serviceCommand.h"

Command::Command(std::string target, std::string type, std::string action, std::string name, std::vector<std::string> option)
{
}

bool Command::construct(std::string command)
{
    return false;
}

bool Command::isValidCommand(std::string command)
{
    return false;
}

bool Command::executeCommand()
{
    return false;
}

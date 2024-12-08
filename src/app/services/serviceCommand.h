#pragma once
#include <iostream>
#include <vector>
#include <string>

class Command {
public:
    std::string target; 
    std::string type;
    std::string action;
    std::string name;
    std::vector<std::string> option;

    Command(std::string target, std::string type, std::string action, std::string name, std::vector<std::string> option);
    bool construct(std::string command);
    bool isValidCommand(std::string command);
    bool executeCommand();
};
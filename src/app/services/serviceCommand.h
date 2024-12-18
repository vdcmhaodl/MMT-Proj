#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>
#include <random>

extern std::mt19937_64 rng;

class Command {
public:
    std::string target; 
    std::string type;
    std::string action;
    std::vector<std::string> listName;
    std::vector<std::string> listOption;

    Command() = default;
    Command(std::string target, std::string type, std::string action, std::vector<std::string> listName, std::vector<std::string> listOption);
    void parseString(std::string stringStr, std::vector<std::string> &listStr);
    void construct(std::string command);
    void print();
    std::string to_string();
    virtual bool isValidCommand(std::string command);
    virtual bool executeCommand();

    static std::string generateFilename(int length);
};
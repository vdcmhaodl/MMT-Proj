#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>
#include <random>
#include <filesystem>
#include <syncstream>

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

    static std::string generateFilename(int length);
    static std::string generateFilepath(int length, std::string extension);
    static std::string generateFilepathWithFolder(std::string filename);
};
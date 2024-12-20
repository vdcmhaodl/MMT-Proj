#include "serviceCommand.h"

std::string seperator = "><";
std::mt19937_64 rng(std::chrono::steady_clock().now().time_since_epoch().count());

Command::Command(std::string target, std::string type, std::string action, std::vector<std::string> listName, std::vector<std::string> listOption) {
    this->target = target;
    this->type = type;
    this->action = action;
    this->listName = listName;
    this->listOption = listOption;
}

void Command::parseString(std::string stringStr, std::vector<std::string> &listStr) {
    size_t pos1 = 0, pos2 = 0;
    while (true) {
        pos2 = stringStr.find(">", pos2);
        if (pos2 == std::string::npos)
            return;
        pos1 = stringStr.find("<", pos1);
        listStr.push_back(stringStr.substr(pos1 + 1, pos2 - pos1 - 1));
        ++pos1; ++pos2;
    }
}

void Command::construct(std::string command) {
    std::stringstream ss;
    std::string stringName, stringOption;

    ss << command;
    std::getline(ss, target, '|');
    std::getline(ss, type, '|');
    std::getline(ss, action, '|');
    std::getline(ss, stringName, '|');
    std::getline(ss, stringOption, '|');

    parseString(stringName, listName);
    parseString(stringOption, listOption);
}

void Command::print() {
    std::cout << "+------\n";
    std::cout << "Target: " << target << '\n';
    std::cout << "Type: " << type << '\n';
    std::cout << "Action: " << action << '\n';
    std::cout << "Name: "; for (auto it: listName) std::cout << it << ' '; std::cout << '\n';
    std::cout << "Option: "; for (auto it: listOption) std::cout << it << ' '; std::cout << '\n';
    std::cout << "+------\n";
}

std::string Command::to_string() {
    std::string command;
    command += target + '|';
    command += type + '|';
    command += action + '|';
    for (auto name : listName) {
        command += '<' + name + '>';
    } command += '|';
    for (auto option : listOption) {
        command += '<' + option + '>';
    } command += '|';
    return command;
}

bool Command::isValidCommand(std::string command) {
    return false;
}

bool Command::executeCommand() {
    return false;
}

std::string Command::generateFilename(int length) {
    std::string result(length, ' ');
    for (char &ch: result) {
        ch = (char)(std::uniform_int_distribution<int>(65, 90)(rng));
    }
    return result;
}

std::string Command::generateFilepath(int length, std::string extension) {
    auto folder = std::filesystem::temp_directory_path();
    auto filename = generateFilename(length) + extension;
    folder /= filename;
    return folder.string();
}

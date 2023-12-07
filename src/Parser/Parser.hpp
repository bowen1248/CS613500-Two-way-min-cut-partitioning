#pragma once
#include "../Structure/Data.hpp"
#include <string>

class Parser
{
    void readTech(Input *input, std::ifstream &fin);
    void readCell(Input *input, std::ifstream &fin);
    void readNet(Input *input, std::ifstream &fin);

public:
    Parser();
    Input::ptr parse(const std::string &filename);
};

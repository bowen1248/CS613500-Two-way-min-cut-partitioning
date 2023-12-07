#include "Parser.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, std::unordered_map<std::string, long long int>> techCell;
std::unordered_map<std::string, Cell *> strToCell;

Parser::Parser() {}

void Parser::readTech(Input *input, std::ifstream &fin)
{
    std::string techName, cellName, _;
    long long int techFileNum, numCells, cellHeight, cellWidth;

    fin >> _ >> techFileNum;
    for (int i = 0; i < techFileNum; i++) 
    {
        fin >> _ >> techName >> numCells;
        
        techCell.insert({techName, std::unordered_map<std::string, long long int>()});
        for (int j = 0; j < numCells; j++) {
            fin >> _ >> cellName >> cellHeight >> cellWidth;
            techCell[techName][cellName] = cellHeight * cellWidth;
        }
    }

    std::string dieATech, dieBTech;
    long long int dieHeight, dieWidth;
    double dieAMaxUtility, dieBMaxUtility;
    fin >> _ >> dieHeight >> dieWidth;
    fin >> _ >> dieATech >> dieAMaxUtility;
    fin >> _ >> dieBTech >> dieBMaxUtility;
    input->dieAMaxSize = dieHeight * dieWidth * (dieAMaxUtility / 100);
    input->dieBMaxSize = dieHeight * dieWidth * (dieBMaxUtility / 100);
    input->dieATech = dieATech;
    input->dieBTech = dieBTech;
}

void Parser::readCell(Input *input, std::ifstream &fin)
{
    std::string cellName, techCellName, _;
    std::string techA = input->dieATech, techB = input->dieBTech;
    long long int numCells, sizeA, sizeB;

    fin >> _ >> numCells;
    for (int i = 0; i < numCells; i++)
    {
        fin >> _ >> cellName >> techCellName;
        sizeA = techCell[techA][techCellName];
        sizeB = techCell[techB][techCellName];
        auto cell = new Cell(cellName, sizeA, sizeB);
        input->cells.emplace_back(cell);
        strToCell.emplace(cellName, cell);
    };
}

void Parser::readNet(Input *input, std::ifstream &fin)
{
    std::string netName, cellName, _;
    int pinSize;

    fin >> _ >> _;
    while (fin >> _ >> netName >> pinSize)
    {
        auto net = new Net(netName);
        input->nets.emplace_back(net);
        for (int i = 0; i < pinSize; i++) {
            fin >> _ >> cellName;
            auto cell = strToCell[cellName];
            net->cells.emplace_back(cell);
            cell->nets.emplace_back(net);
        }
    }
}

Input::ptr Parser::parse(const std::string &filename)
{
    auto input = new Input();
    
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "[Error] Cannot open \"" << filename << "\".\n";
        exit(EXIT_FAILURE);
    }

    readTech(input, fin);
    readCell(input, fin);
    readNet(input, fin);

    return std::unique_ptr<Input>(input);
}

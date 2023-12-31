#include "ResultWriter.hpp"
#include <fstream>
#include <iostream>

ResultWriter::ResultWriter() : cutSize(0), groups(2) {}

void ResultWriter::setCutsize(int cutsize_)
{
    cutSize = cutsize_;
}

void ResultWriter::addCell(Cell *cell)
{
    groups[cell->groupIdx].emplace_back(cell->name);
}

void ResultWriter::write(std::string const &filename) const
{
    std::ofstream fout(filename);
    if (!fout.is_open())
    {
        std::cerr << "[Error] Cannot open \"" << filename << "\".\n";
        exit(EXIT_FAILURE);
    }

    fout << "CutSize " << cutSize << "\n";

    fout << "DieA " << groups[0].size() << "\n";
    for (const auto &cellName : groups[0])
        fout << cellName << "\n";

    fout << "DieB " << groups[1].size() << "\n";
    for (const auto &cellName : groups[1])
        fout << cellName << "\n";
}

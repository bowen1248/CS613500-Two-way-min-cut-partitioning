#include "Partitioner.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

void Partitioner::generateInitialPartition(int seed = 3)
{
    // can set the random seed for different testcases
    std::shuffle(input->cells.begin(), input->cells.end(), std::default_random_engine(seed));
    for (auto &cell : input->cells)
    {
        int groupIdx = (groups[0].size < groups[1].size) ? 0 : 1;
        cell->groupIdx = groupIdx;
        groups[groupIdx].size += cell->size[groupIdx];
    }
}

int Partitioner::getCutSize() const
{
    for (auto &net : input->nets)
        net->updateNumCellInGroup();

    int cutSize = 0;
    for (const auto &net : input->nets)
        if (net->numCellInGroup[0] > 0 && net->numCellInGroup[1] > 0)
            ++cutSize;
    return cutSize;
}

void Partitioner::updateAllCellGain()
{
    for (auto &net : input->nets)
        net->updateNumCellInGroup();

    for (auto &cell : input->cells)
    {
        cell->lock = false;
        cell->gain = 0;
        for (const auto net : cell->nets)
        {
            if (net->numCellInGroup[cell->groupIdx] == 1)
                ++cell->gain;
            if (net->numCellInGroup[!cell->groupIdx] == 0)
                --cell->gain;
        }
    }
}

void Partitioner::bulidBucketList()
{
    groups[0].initBucketList();
    groups[1].initBucketList();

    for (const auto &cell : input->cells)
        groups[cell->groupIdx].insertNode(cell.get());
}

void Partitioner::updateGain(Cell *baseCell)
{
    int from = baseCell->groupIdx;
    int to = !baseCell->groupIdx;
    baseCell->groupIdx = to;
    baseCell->lock = true;
    groups[from].size -= baseCell->size[from];
    groups[to].size += baseCell->size[to];

    groups[from].removeNode(baseCell);
    for (auto net : baseCell->nets)
    {
        if (net->numCellInGroup[to] == 0)
        {
            for (auto cell : net->cells)
            {
                if (!cell->lock)
                {
                    ++cell->gain;
                    groups[cell->groupIdx].moveNode(cell);
                }
            }
        }
        else if (net->numCellInGroup[to] == 1)
        {
            for (auto cell : net->cells)
            {
                if (!cell->lock && cell->groupIdx == to)
                {
                    --cell->gain;
                    groups[cell->groupIdx].moveNode(cell);
                }
            }
        }
        --net->numCellInGroup[from];
        ++net->numCellInGroup[to];
        if (net->numCellInGroup[from] == 0)
        {
            for (auto cell : net->cells)
            {
                if (!cell->lock)
                {
                    --cell->gain;
                    groups[cell->groupIdx].moveNode(cell);
                }
            }
        }
        else if (net->numCellInGroup[from] == 1)
        {
            for (auto cell : net->cells)
            {
                if (!cell->lock && cell->groupIdx == from)
                {
                    ++cell->gain;
                    groups[cell->groupIdx].moveNode(cell);
                }
            }
        }
    }
}

int Partitioner::fmProcess()
{
    updateAllCellGain();
    bulidBucketList();

    int partialSum = 0, maxPartialSum = 0, bestStep = 0;
    std::vector<Cell *> cellStack;

    while (groups[0].numCellInBucketList > 0 && groups[1].numCellInBucketList > 0)
    {
        int deadlock = -1;
        while (groups[0].numCellInBucketList > 0)
        {
            auto baseCell = groups[0].getBaseCell();

            if ((groups[1].size + baseCell->size[0]) > groups[1].maxSize) 
            {
                deadlock++;
                break;
            }

            partialSum += baseCell->gain;
            cellStack.emplace_back(baseCell);
            updateGain(baseCell);
            if (maxPartialSum <= partialSum)
            {
                maxPartialSum = partialSum;
                bestStep = cellStack.size();
            }
        }

        while (groups[1].numCellInBucketList > 0)
        {
            auto baseCell = groups[1].getBaseCell();

            if ((groups[0].size + baseCell->size[1]) > groups[0].maxSize) 
            {
                deadlock++;
                break;
            }
            partialSum += baseCell->gain;
            cellStack.emplace_back(baseCell);
            updateGain(baseCell);
            if (maxPartialSum < partialSum)
            {
                maxPartialSum = partialSum;
                bestStep = cellStack.size();
            }
        }

        if (deadlock >= 1) 
        {
            for (int i = 0; i < 2; i++) 
            {
                auto baseCell = groups[i].getBaseCell();
                partialSum += baseCell->gain;
                cellStack.emplace_back(baseCell);
                updateGain(baseCell);
                if (maxPartialSum <= partialSum)
                {
                    maxPartialSum = partialSum;
                    bestStep = cellStack.size();
                }
            }
        }

    }

    for (int i = cellStack.size() - 1; i >= bestStep; --i)
    {
        auto cell = cellStack[i];
        groups[cell->groupIdx].size -= cell->size[cell->groupIdx];
        cell->groupIdx = !cell->groupIdx;
        groups[cell->groupIdx].size += cell->size[cell->groupIdx];
    }
    return maxPartialSum;
}

Partitioner::Partitioner(Input *input) : input(input), groups(2)
{
    int pMax = 0;
    for (const auto &cell : input->cells)
        if (pMax < static_cast<int>(cell->nets.size()))
            pMax = cell->nets.size();
    groups[0].pMax = groups[1].pMax = pMax;
    groups[0].maxSize = input->dieAMaxSize;
    groups[1].maxSize = input->dieBMaxSize;

    generateInitialPartition(0);
}

ResultWriter::ptr Partitioner::solve()
{
    int cutSize = 0;
    int seed = 100;
    while (true) {
        while (true)
        {
            auto maxPartialSum = fmProcess();

            if (maxPartialSum == 0) {
                cutSize = getCutSize();
                break;
            }

        }
        if (groups[0].maxSize < groups[0].size || groups[1].maxSize < groups[1].size) 
        {
            groups[0].size = 0;
            groups[1].size = 0;
            generateInitialPartition(seed);
            seed++;
        } else {
            break;
        }
    }

    auto result = new ResultWriter();
    result->setCutsize(cutSize);
    for (const auto &cell : input->cells)
        result->addCell(cell.get());
    return std::unique_ptr<ResultWriter>(result);
}

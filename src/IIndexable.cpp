// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "IIndexable.hpp"


int Indexator::getIndex() noexcept
{
    if(!releasedIndexes.empty())
    {
        int index = releasedIndexes.front();
        releasedIndexes.pop();
        return index;
    }

    return globalIndex++;
}

void Indexator::releaseIndex(const int index) noexcept
{
    releasedIndexes.push(index);
}

IIndexable::~IIndexable() noexcept
{
    Indexator::releaseIndex(index);
}

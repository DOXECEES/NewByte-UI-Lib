#ifndef NBUI_SRC_IINDEXABLE_HPP
#define NBUI_SRC_IINDEXABLE_HPP

#include <queue>

class Indexator
{
public:
    static int getIndex() noexcept;

    static void releaseIndex(const int index) noexcept;

private:
    inline static int               globalIndex         = 0;
    inline static std::queue<int>   releasedIndexes;
};

class IIndexable
{
public:
    IIndexable() noexcept
        :index(Indexator::getIndex()) {}
    
    virtual ~IIndexable() noexcept;

    inline int getIndex() const noexcept { return index; }

private:
    int index = 0;
};

#endif


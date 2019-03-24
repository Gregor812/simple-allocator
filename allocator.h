#pragma once

#include <cstdint>

struct BorderTag
{
    size_t Size;
    bool IsFree;
};

struct BlockMetaData
{
    BlockMetaData* Next;
    BlockMetaData* Previous;
    BorderTag Tag;
};

void mysetup(void *buf, size_t size);
void* myalloc(size_t size);
void myfree(void* p);

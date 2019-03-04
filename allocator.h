#include <cstdint>

struct HeadBlockMetaData
{
    bool IsFree;
    HeadBlockMetaData* Next;
    HeadBlockMetaData* Previous;
    std::size_t Size;
};

struct TailBlockMetaData
{
    bool IsFree;
    std::size_t Size;
};

void mysetup(void *buf, std::size_t size);
void *myalloc(std::size_t size);
void myfree(void *p);

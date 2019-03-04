#include "allocator.h"

void* Memory;
std::size_t BufSize;
HeadBlockMetaData* Head;

void mysetup(void *buf, std::size_t size)
{
    Memory = buf;
    BufSize = size;
    
    Head = (HeadBlockMetaData*)Memory;
    *Head = HeadBlockMetaData();
    
    Head->IsFree = true;
    Head->Next = (HeadBlockMetaData*)0;
    Head->Previous = (HeadBlockMetaData*)0;
    Head->Size = BufSize;
    
    TailBlockMetaData* tailAddress = (TailBlockMetaData*)(Memory + BufSize - sizeof(TailBlockMetaData));
    *tailAddress = TailBlockMetaData();
    tailAddress->IsFree = true;
    tailAddress->Size = BufSize;
}

// Функция аллокации
void *myalloc(std::size_t size)
{
    return (void*)0;
}

// Функция освобождения
void myfree(void *p)
{

}

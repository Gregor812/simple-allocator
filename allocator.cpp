#include <cstdint>

struct HeadBlockMetaData
{
    HeadBlockMetaData* Next;
    HeadBlockMetaData* Previous;
    std::size_t Size;
    bool IsFree;
};

struct TailBlockMetaData
{
    std::size_t Size;
    bool IsFree;
};

std::size_t const headSize = sizeof(HeadBlockMetaData);
std::size_t const tailSize = sizeof(TailBlockMetaData);
std::size_t const MetaDataSize = headSize + tailSize;
std::size_t const MinimumFreeSpaceSize = 16;
std::size_t const MinimumBlockSize = MetaDataSize + MinimumFreeSpaceSize;

void* Memory;
std::size_t BufSize;
HeadBlockMetaData* Head;
void* MemoryEnd;

void mysetup(void *buf, std::size_t size)
{
    Memory = buf;
    BufSize = size;
    MemoryEnd = buf + size;
    
    Head = (HeadBlockMetaData*)Memory;
    *Head = HeadBlockMetaData();
    
    Head->IsFree = true;
    Head->Next = (HeadBlockMetaData*)0;
    Head->Previous = (HeadBlockMetaData*)0;
    Head->Size = BufSize;
    
    TailBlockMetaData* tailAddress = (TailBlockMetaData*)(Memory + BufSize - tailSize);
    *tailAddress = TailBlockMetaData();
    tailAddress->IsFree = true;
    tailAddress->Size = BufSize;
}

void *myalloc(std::size_t size)
{
    HeadBlockMetaData* blockHead = Head;
    std::size_t requiredSpaceSize = size + MetaDataSize;
    std::size_t currentBlockFreeSpaceSize = blockHead->Size - MetaDataSize;
    
    while (currentBlockFreeSpaceSize < requiredSpaceSize)
    {
        blockHead = blockHead->Next;
        if (blockHead == (HeadBlockMetaData*)0)
        {
            return (void*)0;
        }
        currentBlockFreeSpaceSize = blockHead->Size - MetaDataSize;
    }
    
    std::size_t tailOffset = blockHead->Size - tailSize;
    TailBlockMetaData* blockTail = (TailBlockMetaData*)((void*)blockHead + tailOffset);

    blockTail->IsFree = false;

    std::size_t blockLeftFreeSpaceSize = currentBlockFreeSpaceSize - requiredSpaceSize;
    if (blockLeftFreeSpaceSize < MinimumBlockSize)
    {
        blockHead->IsFree = false;
        
        if (blockHead->Previous != (HeadBlockMetaData*)0)
        {
            blockHead->Previous->Next = blockHead->Next;
        }
        if (blockHead->Next != (HeadBlockMetaData*)0)
        {
            blockHead->Next->Previous = blockHead->Previous;
        }

        return (void*)blockHead + headSize;
    }
    
    blockTail->Size = requiredSpaceSize;

    blockHead->Size -= requiredSpaceSize;

    tailOffset = blockHead->Size - tailSize;
    blockTail = (TailBlockMetaData*)((void*)blockHead + tailOffset);
    blockTail->IsFree = true;
    blockTail->Size = blockHead->Size;

    blockHead = (HeadBlockMetaData*)((void*)blockHead + blockHead->Size);
    blockHead->IsFree = false;
    blockHead->Size = requiredSpaceSize;

    return (void*)blockHead + headSize;
}

void myfree(void *p)
{
    if (p < (Memory + headSize) || p > (MemoryEnd - MinimumBlockSize))
    {
        return;
    }

    HeadBlockMetaData* blockHead = (HeadBlockMetaData*)(p - headSize);

    HeadBlockMetaData* rightBlockHead = (HeadBlockMetaData*)((void*)blockHead + blockHead->Size);
    TailBlockMetaData* leftBlockTail = (TailBlockMetaData*)((void*)blockHead - tailSize);

    bool isLeftBlockFree = false;
    bool isRightBlockFree = false;

    if (leftBlockTail >= (Memory + MinimumBlockSize - tailSize))
    {
        isLeftBlockFree = leftBlockTail->IsFree;
    }
    if (rightBlockHead <= (MemoryEnd - MinimumBlockSize))
    {
        isRightBlockFree = rightBlockHead->IsFree;
    }

    if (isLeftBlockFree)
    {
        HeadBlockMetaData* leftBlockHead = (HeadBlockMetaData*)((void*)leftBlockTail + tailSize - leftBlockTail->Size);

        leftBlockHead->IsFree = false;
        leftBlockHead->Size += blockHead->Size;
        blockHead = leftBlockHead;

        if (blockHead->Previous != (HeadBlockMetaData*)0)
        {
            blockHead->Previous->Next = blockHead->Next;
        }
        if (blockHead->Next != (HeadBlockMetaData*)0)
        {
            blockHead->Next->Previous = blockHead->Previous;
        }
    }

    if (isRightBlockFree)
    {
        if (rightBlockHead->Previous != (HeadBlockMetaData*)0)
        {
            rightBlockHead->Previous->Next = rightBlockHead->Next;
        }
        if (rightBlockHead->Next != (HeadBlockMetaData*)0)
        {
            rightBlockHead->Next->Previous = rightBlockHead->Previous;
        }

        blockHead->Size += rightBlockHead->Size;
    }

    blockHead->Next = Head;
    blockHead->IsFree = true;
    Head = blockHead;

    TailBlockMetaData* blockTail = (TailBlockMetaData*)((void*)blockHead + blockHead->Size);
    blockTail->Size = blockHead->Size;
    blockTail->IsFree = true;
}

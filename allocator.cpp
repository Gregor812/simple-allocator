#include "allocator.h"

BorderTag* GetBlockTail(BlockMetaData* block);
BlockMetaData* FindEnoughSizeBlock(size_t size);
void AllocateWholeBlock(BlockMetaData* block);
void ExcludeAllocatedBlockFromFreeBlocksList(BlockMetaData* allocated);
BlockMetaData* AllocateBlockPart(BlockMetaData* allocatingBlock, size_t size);
BorderTag* GetLeftNeighbor(BlockMetaData* block);
BlockMetaData* GetRightNeighbor(BlockMetaData* block);
BlockMetaData* JoinLeftBlock(BlockMetaData* freeing_block, BorderTag* border_tag);
BlockMetaData* JoinRightBlock(BlockMetaData* freeing_block, BlockMetaData* block_meta_data);
void IncludeFreeBlockIntoFreeBlocksList(BlockMetaData* newFreeBlock);

size_t const headSize = sizeof(BlockMetaData);
size_t const tailSize = sizeof(BorderTag);
size_t const FullMetaDataSize = headSize + tailSize;
size_t const MinimumFreeSpaceSize = 16;
size_t const MinimumBlockSize = MinimumFreeSpaceSize + FullMetaDataSize;

uint8_t* Memory;
size_t FullMemorySize;
uint8_t* MemoryEnd;

BlockMetaData* FreeBlocksListHead;

void mysetup(void* buf, size_t size)
{
    Memory = static_cast<uint8_t*>(buf);
    FullMemorySize = size;
    MemoryEnd = static_cast<uint8_t*>(buf) + size;

    FreeBlocksListHead = static_cast<BlockMetaData*>(buf);

    FreeBlocksListHead->Next = static_cast<BlockMetaData*>(0);
    FreeBlocksListHead->Previous = static_cast<BlockMetaData*>(0);
    FreeBlocksListHead->Tag.IsFree = true;
    FreeBlocksListHead->Tag.Size = FullMemorySize;

    BorderTag* tailAddress = GetBlockTail(FreeBlocksListHead);
    tailAddress->IsFree = true;
    tailAddress->Size = FullMemorySize;
}

void* myalloc(size_t size)
{
    size += FullMetaDataSize;

    if (size > FullMemorySize)
        return static_cast<void*>(0);

    BlockMetaData* allocatingBlock = FindEnoughSizeBlock(size);

    if (allocatingBlock == static_cast<BlockMetaData*>(0))
        return static_cast<void*>(0);

    if (allocatingBlock->Tag.Size - size < MinimumBlockSize)
    {
        AllocateWholeBlock(allocatingBlock);
    }
    else
    {
        allocatingBlock = AllocateBlockPart(allocatingBlock, size);
    }

    return static_cast<void*>(
        reinterpret_cast<uint8_t*>(allocatingBlock) + headSize);
}

void myfree(void* p)
{
    uint8_t* freeingMemory = static_cast<uint8_t*>(p);
    if ((freeingMemory < Memory + headSize) ||
        (freeingMemory > MemoryEnd - MinimumBlockSize + headSize))
    {
        return;
    }

    BlockMetaData* freeingBlock = reinterpret_cast<BlockMetaData*>(freeingMemory - headSize);
    if (freeingBlock->Tag.IsFree)
        return;

    BorderTag* leftNeighbor = GetLeftNeighbor(freeingBlock);
    BlockMetaData* rightNeighbor = GetRightNeighbor(freeingBlock);

    if (leftNeighbor != static_cast<BorderTag*>(0) && leftNeighbor->IsFree)
    {
        freeingBlock = JoinLeftBlock(freeingBlock, leftNeighbor);
    }

    if (rightNeighbor != static_cast<BlockMetaData*>(0) && rightNeighbor->Tag.IsFree)
    {
        freeingBlock = JoinRightBlock(freeingBlock, rightNeighbor);
    }

    freeingBlock->Tag.IsFree = true;
    BorderTag* tailBorderTag = GetBlockTail(freeingBlock);
    tailBorderTag->IsFree = true;

    IncludeFreeBlockIntoFreeBlocksList(freeingBlock);
}

BorderTag* GetBlockTail(BlockMetaData* block)
{
    size_t blockSize = block->Tag.Size;
    return reinterpret_cast<BorderTag*>(reinterpret_cast<uint8_t*>(block) + blockSize - tailSize);
}

BlockMetaData* FindEnoughSizeBlock(size_t size)
{
    BlockMetaData* freeBlock = FreeBlocksListHead;

    while (freeBlock != static_cast<BlockMetaData*>(0))
    {
        if (freeBlock->Tag.Size >= size)
            return freeBlock;

        freeBlock = freeBlock->Next;
    }

    return freeBlock;
}

void AllocateWholeBlock(BlockMetaData* block)
{
    block->Tag.IsFree = false;
    BorderTag* tailBorderTag = GetBlockTail(block);
    tailBorderTag->IsFree = false;

    ExcludeAllocatedBlockFromFreeBlocksList(block);
}

void ExcludeAllocatedBlockFromFreeBlocksList(BlockMetaData* allocated)
{
    BlockMetaData* previousFreeBlock = allocated->Previous;
    BlockMetaData* nextFreeBlock = allocated->Next;

    if (allocated == FreeBlocksListHead)
    {
        FreeBlocksListHead = nextFreeBlock;
    }
    else
    {
        previousFreeBlock->Next = nextFreeBlock;
    }

    if (nextFreeBlock != static_cast<BlockMetaData*>(0))
        nextFreeBlock->Previous = previousFreeBlock;
}

BlockMetaData* AllocateBlockPart(BlockMetaData* allocatingBlock, size_t size)
{
    BlockMetaData* freePart = allocatingBlock;
    
    size_t freePartSize = freePart->Tag.Size - size;
    freePart->Tag.Size = freePartSize;
    BorderTag* freePartTail = GetBlockTail(freePart);
    freePartTail->Size = freePartSize;
    freePartTail->IsFree = true;

    allocatingBlock = reinterpret_cast<BlockMetaData*>(reinterpret_cast<uint8_t*>(freePart) + freePartSize);
    allocatingBlock->Tag.Size = size;
    allocatingBlock->Tag.IsFree = false;
    BorderTag* allocatingBlockTail = GetBlockTail(allocatingBlock);
    allocatingBlockTail->Size = size;
    allocatingBlockTail->IsFree = false;

    return allocatingBlock;
}

BorderTag* GetLeftNeighbor(BlockMetaData* block)
{
    if (block <= reinterpret_cast<BlockMetaData*>(Memory))
        return static_cast<BorderTag*>(0);

    return reinterpret_cast<BorderTag*>(reinterpret_cast<uint8_t*>(block) - tailSize);
}

BlockMetaData* GetRightNeighbor(BlockMetaData* block)
{
    BorderTag* tail = GetBlockTail(block);
    if (reinterpret_cast<uint8_t*>(tail) + tailSize >= MemoryEnd)
        return static_cast<BlockMetaData*>(0);

    return reinterpret_cast<BlockMetaData*>(reinterpret_cast<uint8_t*>(tail) + tailSize);
}

BlockMetaData* JoinLeftBlock(BlockMetaData* currentBlock, BorderTag* leftTail)
{
    BlockMetaData* leftBlock = reinterpret_cast<BlockMetaData*>(reinterpret_cast<uint8_t*>(currentBlock) - leftTail->Size);
    ExcludeAllocatedBlockFromFreeBlocksList(leftBlock);

    leftBlock->Tag.Size += currentBlock->Tag.Size;
    leftBlock->Tag.IsFree = false;

    leftTail = GetBlockTail(leftBlock);
    leftTail->Size = leftBlock->Tag.Size;
    leftTail->IsFree = false;

    return leftBlock;
}

BlockMetaData* JoinRightBlock(BlockMetaData* currentBlock, BlockMetaData* rightBlock)
{
    ExcludeAllocatedBlockFromFreeBlocksList(rightBlock);
    BorderTag* rightTail = GetBlockTail(rightBlock);

    currentBlock->Tag.Size += rightBlock->Tag.Size;
    currentBlock->Tag.IsFree = false;

    rightTail = GetBlockTail(currentBlock);
    rightTail->Size = currentBlock->Tag.Size;
    rightTail->IsFree = false;

    return currentBlock;
}

void IncludeFreeBlockIntoFreeBlocksList(BlockMetaData* newFreeBlock)
{
    if (FreeBlocksListHead == static_cast<BlockMetaData*>(0))
    {
        FreeBlocksListHead = newFreeBlock;
        FreeBlocksListHead->Next = static_cast<BlockMetaData*>(0);
    }
    else
    {
        BlockMetaData* listOfFreeBlocks = FreeBlocksListHead;
        FreeBlocksListHead = newFreeBlock;
        FreeBlocksListHead->Next = listOfFreeBlocks;
        listOfFreeBlocks->Previous = FreeBlocksListHead;
    }

    FreeBlocksListHead->Previous = static_cast<BlockMetaData*>(0);
}

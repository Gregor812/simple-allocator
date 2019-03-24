#include <iostream>
#include <list>

#include "allocator.h"

int main()
{
    auto memory = new uint8_t[1024];

    mysetup(memory, 1024);

    std::list<void*> list;

    void* currentPointer;

    auto i = 10;

    while (i--)
    {
        std::cout << "Iteration " << 10 - i << std::endl;

        while (currentPointer = myalloc(256))
        {
            list.push_front(currentPointer);
        }

        for (auto i = list.begin(); i != list.end(); i++)
        {
            currentPointer = *i;
            myfree(currentPointer);
        }

        currentPointer = myalloc(1024 - 48);
        myfree(currentPointer);

        list.clear();
    }

    return 0;
}

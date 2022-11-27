#pragma once

template<size_t T>
class AlignedAllocationPolicy
{
public:
    static void* operator new(const size_t size)
    {
        return _aligned_malloc(size, T);
    }

    static void operator delete(void* memory)
    {
        _aligned_free(memory);
    }
};
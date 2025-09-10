/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include "cmMemoryLog.h"
#include <cstdlib>
#include <new>

// This is a debug feature. We only enable it when explicitly requested,
// which will be controlled by a CMake option that adds a define.
#ifdef CMAKE_DEBUG_MEMORY

// The header we prepend to each allocation to store its size.
struct AllocationHeader {
    size_t size;
};

// We need to ensure the memory we return is properly aligned.
// We use a simple approach of aligning to the size of a pointer.
constexpr size_t HeaderSize = sizeof(AllocationHeader);
constexpr size_t Alignment = alignof(void*);
constexpr size_t AlignedHeaderSize = (HeaderSize + Alignment - 1) & ~(Alignment - 1);

void* operator new(size_t size) {
    size_t total_size = size + AlignedHeaderSize;
    char* p = static_cast<char*>(malloc(total_size));
    if (!p) {
        throw std::bad_alloc();
    }
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(p);
    header->size = size;

    cmMemoryLog::GetInstance().LogAllocation(size);

    return p + AlignedHeaderSize;
}

void* operator new[](size_t size) {
    size_t total_size = size + AlignedHeaderSize;
    char* p = static_cast<char*>(malloc(total_size));
    if (!p) {
        throw std::bad_alloc();
    }
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(p);
    header->size = size;

    cmMemoryLog::GetInstance().LogAllocation(size);

    return p + AlignedHeaderSize;
}

void operator delete(void* p) noexcept {
    if (!p) {
        return;
    }
    char* p_user = static_cast<char*>(p);
    char* p_real = p_user - AlignedHeaderSize;
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(p_real);
    size_t size = header->size;

    cmMemoryLog::GetInstance().LogDeallocation(size);

    free(p_real);
}

void operator delete[](void* p) noexcept {
    if (!p) {
        return;
    }
    char* p_user = static_cast<char*>(p);
    char* p_real = p_user - AlignedHeaderSize;
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(p_real);
    size_t size = header->size;

    cmMemoryLog::GetInstance().LogDeallocation(size);

    free(p_real);
}

// Sized deallocation for C++14 and later.
#if __cplusplus >= 201402L
void operator delete(void* p, size_t) noexcept {
    if (!p) {
        return;
    }
    char* p_user = static_cast<char*>(p);
    char* p_real = p_user - AlignedHeaderSize;
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(p_real);
    size_t size = header->size;
    cmMemoryLog::GetInstance().LogDeallocation(size);
    free(p_real);
}

void operator delete[](void* p, size_t) noexcept {
    if (!p) {
        return;
    }
    char* p_user = static_cast<char*>(p);
    char* p_real = p_user - AlignedHeaderSize;
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(p_real);
    size_t size = header->size;
    cmMemoryLog::GetInstance().LogDeallocation(size);
    free(p_real);
}
#endif // C++14

#endif // CMAKE_DEBUG_MEMORY

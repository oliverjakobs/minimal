#include "tb_mem.h"

#include <string.h>

void* tb_mem_malloc(tb_allocator* allocator, size_t size)
{
    size_t s = size + sizeof(size_t);
    size_t* hdr = (allocator && allocator->malloc) ? allocator->malloc(s) : malloc(s);

    if (!hdr) return NULL;

    hdr[0] = s;
    return hdr + 1;
}

void* tb_mem_calloc(tb_allocator* allocator, size_t count, size_t size)
{
    size_t s = (size * count) + sizeof(size_t);
    size_t* hdr = (allocator && allocator->malloc) ? allocator->malloc(s) : calloc(1, s);

    if (!hdr) return NULL;

    if (allocator && allocator->malloc) memset(hdr, 0, s);

    hdr[0] = s;
    return hdr + 1;
}

void* tb_mem_realloc(tb_allocator* allocator, void* block, size_t size)
{
    size_t* hdr = block ? TB_MEM_HDR(block) : NULL;
    size_t old_size = hdr ? hdr[0] : 0;

    size_t s = size + sizeof(size_t);
    hdr = (allocator && allocator->realloc) ? allocator->realloc(hdr, old_size, size) : realloc(hdr, size);

    if (!hdr) return NULL;

    hdr[0] = s;
    return hdr + 1;
}

void tb_mem_free(tb_allocator* allocator, void* block)
{
    if (!block) return;

    size_t* hdr = TB_MEM_HDR(block);
    if (allocator && allocator->free)   allocator->free(hdr, hdr[0]);
    else                                free(hdr);
}

void* tb_mem_dup(tb_allocator* allocator, const void* src, size_t size)
{
    void* dst = tb_mem_malloc(allocator, size);
    return dst ? memcpy(dst, src, size) : NULL;
}

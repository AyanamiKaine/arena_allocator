#include "arena.h"
#include <stdlib.h>
#include <string.h>  // For memset
#include <stdio.h>


Arena* arena_new(size_t initial_size) {
    Arena* arena = malloc(sizeof(Arena));
    if (!arena) { return NULL; }

    arena->start = malloc(initial_size);
    if (!arena->start) { 
        free(arena);
        return NULL;
    }

    arena->current = arena->start;
    arena->size = initial_size;
    return arena;
}

void arena_free(Arena* arena) {
    free(arena->start);
    free(arena);
}

ArenaError arena_grow(Arena* arena, size_t additional_size) {
    size_t newSize = arena->size + additional_size;
    char* newStart = realloc(arena->start, newSize);
    if (!newStart) {
        return ARENA_ERROR_REALLOCATION_FAILED; // Reallocation failed
    }

    // Update arena state
    size_t usedBytes = arena->current - arena->start; // Calculate used bytes before realloc
    arena->start = newStart;
    arena->current = arena->start + usedBytes; // Restore the current pointer
    arena->size = newSize;
    return ARENA_SUCCESS; // Growth successful
}

void* arena_allocate(Arena* arena, size_t size, size_t alignment) {
    // Align the current position
    size_t adjustment = alignment - ((size_t)arena->current % alignment);
    if (adjustment == alignment) adjustment = 0;  // Already aligned

    // Try to grow if not enough space
    if (arena->current + adjustment + size > arena->start + arena->size) {
        if (!arena_grow(arena, arena->size * 2 + adjustment)) { 
            return NULL; // Growth failed
        }
    }

    void* ptr = arena->current + adjustment;
    arena->current += adjustment + size;

    memset(ptr, 0, size); // Initialize allocated memory to zero
    return ptr;
}

void arena_reset(Arena* arena) {
    arena->current = arena->start;
}


size_t arena_available(const Arena* arena) {
    return arena->size - (arena->current - arena->start);
}

size_t arena_used(const Arena* arena) {
    return arena->current - arena->start;
}

float arena_utilization(const Arena* arena) {
    return (float)arena_used(arena) / arena->size * 100.0f;
}

void arena_print_stats(const Arena* arena) {
    printf("Arena Statistics:\n");
    printf("  Total size: %zu bytes\n", arena->size);
    printf("  Used: %zu bytes\n", arena_used(arena));
    printf("  Available: %zu bytes\n", arena_available(arena));
    printf("  Utilization: %.2f%%\n", arena_utilization(arena));
}

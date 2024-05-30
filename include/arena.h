#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>  // for size_t

/**
 * ArenaError: Represents the possible error states that can occur during operations within the Arena memory allocator.
 */
typedef enum {
    ARENA_SUCCESS,               /** The operation completed successfully. */
    ARENA_ERROR_ALLOCATION_FAILED, /** Initial memory allocation for the arena failed. */
    ARENA_ERROR_REALLOCATION_FAILED  /** Memory reallocation (for arena growth) failed. */
} ArenaError;

/**
 * @brief Represents a linear memory arena.
 *
 * The `Arena` structure is a simple linear allocator that manages a contiguous block of memory.
 * It provides functions to allocate, reset, and free memory within this block.  Arenas are useful
 * for scenarios where you want to avoid the overhead of frequent heap allocations and deallocations,
 * or when you want to manage memory for a specific purpose in a more controlled way. Usually its a far
 * better approach then to use malloc and free for every object you create. Think about the collective 
 * lifetime of objects and allocate objects with the same or similar life time in an arena.
 *
 * @field start   A pointer to the beginning of the arena's memory block.
 * @field current A pointer to the current allocation position within the memory block.
 * @field size    The total size (in bytes) of the arena's memory block.
 *
 * @note
 * - The `current` pointer keeps track of the next available byte for allocation.
 * - When the `current` pointer reaches the end of the memory block (i.e., `current == start + size`),
 *   no further allocations are possible without either resetting or resizing the arena.
 */
typedef struct Arena {
    char* start;        // Start of the arena memory
    char* current;      // Current allocation position
    size_t size;        // Total size of the arena
} Arena;

/**
 * @brief Create a new arena with the given initial size.
 *
 * Allocates memory for an Arena structure and initializes it with the specified initial capacity.
 * If successful, the arena's internal memory block is allocated and ready for use.
 *
 * @param initial_size The initial size of the arena's memory block in bytes.
 *
 * @return A pointer to the newly created Arena structure, or `NULL` if the allocation failed
 *         (e.g., due to insufficient system memory).
 *
 * @note
 * - The internal memory block of the arena is allocated using `malloc()`.
 * - If allocation of either the Arena structure or its memory block fails, any partially 
 *   allocated resources are freed before returning `NULL`.
 *
 * @example
 * Arena* myArena = arena_new(1024);  // Create an arena with 1024 bytes
 * if (myArena) {
 *     // Use the arena...
 * } else {
 *     // Handle allocation failure...
 * }
 */
Arena* arena_new(size_t initial_size);

/**
 * @brief Allocate aligned memory of the given size from the arena.
 *
 * Allocates a block of memory from the provided arena, ensuring the memory address is 
 * aligned to the specified boundary. This function is designed to be a drop-in replacement
 * for `malloc()` in most scenarios where memory is managed within an arena.
 *
 * @param arena   Pointer to the Arena structure from which to allocate memory.
 * @param size    The desired size of the memory block in bytes.
 * @param alignment The desired alignment of the memory block (must be a power of two).
 * 
 * @return A pointer to the newly allocated and aligned memory block, or `NULL` if the allocation failed
 *         (e.g., due to insufficient space in the arena or an invalid alignment).
 *
 * @note
 * - The allocated memory block is automatically initialized to zero.
 * - If the requested alignment is 1, no alignment adjustment is performed for efficiency.
 * - The arena may be automatically resized if there is insufficient space to fulfill the request. In 
 * this case the new arenas size will be = arena->size * 2 + alignment
 *
 * @example
 * Arena myArena;
 * arena_init(&myArena, 1024);  // Initialize the arena with 1024 bytes
 * 
 * int* data = (int*)arena_allocate(&myArena, 100, 4);  // Allocate 100 bytes aligned to 4
 * if (data) {
 *     // Use the allocated memory...
 * }
 */
void* arena_allocate(Arena* arena, size_t size, size_t alignment);

// Attempt to grow the arena by the given size (in bytes). Returns ARENA_SUCCESS on success, ARENA_ERROR_REALLOCATION_FAILED on failure.
ArenaError arena_grow(Arena* arena, size_t additional_size); 

/**
 * @brief Resets the arena to its initial state.
 *
 * This function resets the arena's internal position pointer (`current`) back to 
 * the beginning of the arena's memory block (`start`). This effectively makes all 
 * previously allocated memory within the arena available for reuse.
 *
 * @param arena Pointer to the Arena structure to be reset.
 *
 * @note
 * - This function does not deallocate any memory. The arena's total capacity remains unchanged.
 * - Data in the previously allocated memory blocks is not cleared or erased; it becomes 
 *   accessible for overwriting in subsequent allocations.
 */
void arena_reset(Arena* arena);

/**
 * @brief Frees all memory associated with the arena.
 *
 * Deallocates the internal memory block used by the arena and then frees the Arena structure itself.
 * After this call, the arena pointer becomes invalid and should not be used.
 *
 * @param arena Pointer to the Arena structure to be freed.
 *
 * @note This function must be called to release the resources used by the arena when it's no longer needed.
 */
void arena_free(Arena* arena);

/**
 * @brief Get the available space in the arena.
 *
 * Returns the number of bytes currently available for allocation in the arena. This
 * does not include any memory that might be freed up by resetting the arena.
 *
 * @param arena Pointer to the Arena structure.
 * @return The available space in the arena (in bytes).
 */
size_t arena_available(const Arena* arena);

/**
 * @brief Get the used space in the arena.
 *
 * Returns the number of bytes currently allocated in the arena. This includes any memory
 * that has been allocated but not yet freed, even if it's currently unused due to a reset.
 *
 * @param arena Pointer to the Arena structure.
 * @return The used space in the arena (in bytes).
 */
size_t arena_used(const Arena* arena);

/**
 * @brief Get the utilization of the arena.
 *
 * Returns the percentage of the arena's total capacity that is currently in use.
 *
 * @param arena Pointer to the Arena structure.
 * @return The utilization of the arena as a floating-point value (0.0 to 1.0).
 */
float arena_utilization(const Arena* arena);

/**
 * @brief Print statistics about the arena's usage.
 *
 * Prints a summary of the arena's usage to standard output, including the total size,
 * used space, available space, and utilization percentage. This is mainly a debugging tool.
 *
 * @param arena Pointer to the Arena structure.
 */
void arena_print_stats(const Arena* arena);

#endif // ARENA_H
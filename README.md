## Introduction

An arena/bump allocator is a an alternative approach to using malloc and free directly. Instead of freeing on a per object bases we free an entire region of memory that was allocated by an arena. Instead of asking the OS many times for memory and freeing memory many times we instead allocate once and free once.

## Using the Arena Allocator

### Include the Header

Start by including the `arena.h` header file in your project:

```c
#include "arena.h"
```

### Basic Usage

1. **Initialization:** Create a new arena with a desired initial size (in bytes):

   ```c
   Arena* myArena = arena_new(1024);  // Create a 1024-byte arena
   ```

2. **Allocation:** Allocate memory from the arena, specifying the desired size and optional alignment:

   ```c
   int* data = (int*)arena_allocate(myArena, 100, 4);  // Allocate 100 bytes, aligned to 4-byte boundary
   ```

   You can allocate any type of data (structs, arrays, etc.) by casting the returned `void*` pointer.

3. **Resetting:** Reset the arena to reuse the allocated memory:

   ```c
   arena_reset(myArena);
   ```

4. **Freeing:** When you're done with the arena, free its memory:
   ```c
   arena_free(myArena);
   ```

### Example: Managing Particle Systems

```c
Arena particleArena;
arena_new(&particleArena, 2048); // Allocate arena for particles

for (int i = 0; i < numParticles; i++) {
    Particle* p = (Particle*)arena_allocate(&particleArena, sizeof(Particle), alignof(Particle));
    // ... initialize particle data ...
}

// ... simulate particle system ...

arena_reset(&particleArena); // Reset for next frame

arena_free(&particleArena);
```

### Additional Functions

- **`arena_grow(Arena* arena, size_t additional_size)`:**

  - Attempts to increase the arena's size by `additional_size` bytes.
  - Returns `ARENA_SUCCESS` if successful, `ARENA_ERROR_REALLOCATION_FAILED` otherwise.
  - Example:
    ```c
    if (arena_grow(myArena, 512) == ARENA_SUCCESS) {
        printf("Arena successfully grown!\n");
    }
    ```

- **`arena_available(const Arena* arena)`:**

  - Returns the amount of memory currently available in the arena (in bytes).
  - Example:
    ```c
    size_t freeSpace = arena_available(myArena);
    printf("Available space: %zu bytes\n", freeSpace);
    ```

- **`arena_used(const Arena* arena)`:**

  - Returns the amount of memory currently allocated in the arena (in bytes).
  - Example:
    ```c
    size_t usedSpace = arena_used(myArena);
    printf("Used space: %zu bytes\n", usedSpace);
    ```

- **`arena_utilization(const Arena* arena)`:**

  - Returns the percentage of the arena's capacity that is currently used (0.0 to 1.0).
  - Example:
    ```c
    float utilization = arena_utilization(myArena);
    printf("Utilization: %.2f%%\n", utilization * 100.0f);
    ```

- **`arena_print_stats(const Arena* arena)`:**
  - Prints a summary of the arena's usage statistics to the console.
  - Useful for debugging and monitoring memory usage.
  - Example:
    ```c
    arena_print_stats(myArena);
    ```

## Advanced Features

- **Automatic Growth:** If you try to allocate more memory than is available, the arena will automatically try to grow either by the needed size to allocate the object or doubling its size (set if_size_too_small_double_in_size to true).
- **Alignment:** Control memory alignment for performance optimization or specific hardware requirements.
- **Statistics:** Get information about arena usage with the `arena_used`, `arena_available`, `arena_utilization`, and `arena_print_stats` functions.

## Why Use an Arena Allocator?

- **Performance:** Avoids the overhead of frequent `malloc` and `free` calls, especially beneficial for short-lived objects.
- **Fragmentation Reduction:** Prevents memory fragmentation caused by scattered allocations and deallocations.
- **Lifetime Management:** Simplify memory management by collectively deallocating objects with similar lifetimes.
- **Deterministic Behavior:** Arena allocators offer predictable memory usage patterns, helpful for real-time systems or embedded environments.

### Use Cases

- **Game Development:** Manage game entities, particle systems, temporary buffers, and more.
- **Embedded Systems:** Deal with limited memory resources and strict real-time constraints.
- **High-Performance Computing:** Optimize memory allocation in computationally intensive tasks.
- **Any Application with Frequent, Short-Lived Allocations:** Arenas excel in scenarios where you create and destroy many small objects within a defined scope.

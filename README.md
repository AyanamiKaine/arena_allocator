## Introduction

An arena/bump allocator is a an alternative approach to using malloc and free directly. Instead of freeing on a per object bases we free an entire region of memory that was allocated by an arena. Instead of asking the OS many times for memory and freeing memory many times we instead allocate once and free once.

## Use Cases

An arena allocater is usually better in 90 percent of use cases where you would use malloc and free.

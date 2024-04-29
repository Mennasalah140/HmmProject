# Custom Memory Management Library

This is a custom memory management library implemented in C, providing functions similar to `malloc`, `free`, `calloc`, and `realloc`. The library allows for dynamic memory allocation and deallocation with additional features such as alignment and memory optimization.

## Features

- **Dynamic Memory Allocation**: Provides functions for allocating and deallocating memory dynamically.
- **Alignment**: Ensures that memory addresses are aligned to a specified size, improving performance and compatibility with certain architectures.
- **Optimization**: Optimizes memory usage by merging adjacent free memory blocks.
- **Safety**: Includes checks to handle edge cases and prevent memory leaks or segmentation faults.

## Functions

- `void *malloc(size_t size)`: Allocates memory of the specified size.
- `void free(void *ptr)`: Deallocates memory previously allocated by `malloc`, `calloc`, or `realloc`.
- `void *calloc(size_t nmemb, size_t size)`: Allocates memory for an array of `nmemb` elements of `size` bytes each, initialized to zero.
- `void *realloc(void *ptr, size_t size)`: Resizes the memory block pointed to by `ptr` to the specified `size`.

## Usage

1. **Include Header File**: Include the header file `myhmm.h` in your C program.
2. **Link Library**: Link the library `libmyhmm.a` or `libmyhmm.so` with your program during compilation.
3. **Use Functions**: Use the memory management functions (`malloc`, `free`, `calloc`, `realloc`) in your program as needed.

## Example

```c
#include <stdio.h>
#include "myhmm.h"

int main() {
    // Allocate memory
    int *ptr = (int *)malloc(sizeof(int) * 10);

    if (ptr == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Use allocated memory
    for (int i = 0; i < 10; i++) {
        ptr[i] = i * 2;
        printf("%d ", ptr[i]);
    }

    // Deallocate memory
    free(ptr);

    return 0;
}

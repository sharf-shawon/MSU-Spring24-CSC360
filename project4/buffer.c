#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    int *array;
    int size;
} buffer;

// Function to initialize the array with n slots
void initBuffer(buffer *arr, int n)
{
    arr->array = (int *)malloc(n * sizeof(int));
    arr->size = n;
    // Initialize all elements to zero
    for (int i = 0; i < n; i++)
    {
        arr->array[i] = -1;
    }
}

// Function to access the value of a specific slot
int buffer_get_item(buffer *arr, int index)
{
    if (index >= 0 && index < arr->size)
    {
        return arr->array[index];
    }
    else
    {
        printf("SError: Index out of bounds\n");
        return -5;
    }
}

// Function to remove the value of a specific slot
void buffer_remove_item(buffer *arr, int index)
{
    if (index >= 0 && index < arr->size)
    {
        arr->array[index] = -1;
    }
    else
    {
        printf("Error: Index out of bounds\n");
    }
}

// Function to insert a value of in specific slot
void buffer_insert_item(buffer *arr, int val, int index)
{
    if (index >= 0 && index < arr->size)
    {
        arr->array[index] = val;
    }
    else
    {
        printf("Error: Index out of bounds\n");
    }
}

bool is_buffer_empty(buffer *arr)
{
    for (int i = 0; i < arr->size; i++)
        if (arr->array[i] != -1)
            return false;

    return true;
}

bool is_buffer_full(buffer *arr)
{
    for (int i = 0; i < arr->size; i++)
        if (arr->array[i] == -1)
            return false;

    return true;
}

int buffers_occupied(buffer *arr)
{
    int count = 0;
    for (int i = 0; i < arr->size; i++)
        if (arr->array[i] != -1)
            count++;

    return count;
}

// Function to print buffer status
void buffer_print_status(buffer *arr, int wLoc, int rLoc)
{
    printf("(buffers occupied: %d)\n", buffers_occupied(arr));
    printf("buffers:");
    for (int i = 0; i < arr->size; i++)
        printf(" %d\t", buffer_get_item(arr, i));
    printf("\n\t");
    for (int i = 0; i < arr->size; i++)
        printf("----\t");
    printf("\n\t");
    for (int i = 0; i < arr->size; i++)
    {

        printf(" ");
        printf(wLoc == i ? "W" : " ");
        printf(rLoc == i ? "R" : " ");
        printf(" \t");
    }
    printf("\n");
}

// int main()
// {
//     buffer myBuffer;
//     int n = 10; // Number of slots

//     // Initialize the array with n slots
//     initBuffer(&myBuffer, n);

//     // Access and print values
//     for (int i = 0; i < n; i++)
//     {
//         printf("Value at index %d: %d\n", i, getValue(&myBuffer, i));
//     }

//     // Remove values at specific indices
//     removeValue(&myBuffer, 2);
//     removeValue(&myBuffer, 5);

//     // Access and print values after removal
//     for (int i = 0; i < n; i++)
//     {
//         printf("Value at index %d: %d\n", i, getValue(&myBuffer, i));
//     }

//     // Free memory
//     free(myBuffer.array);

//     return 0;
// }

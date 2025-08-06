#ifndef ARRAY_INT64_H
#define ARRAY_INT64_H

#include <stdlib.h>  // For malloc, free, realloc
#include <stdint.h>  // For int16_t
#include <stdio.h>   // For printf

// Define the arraylist structure
typedef struct {
    int64_t* data;  // Array to hold the list data
    int size;       // Number of elements in the list
    int max_size;   // Maximum capacity of the list
} list_64t;

// Function declarations
void init_int64_list(list_64t* list, int max_size);
void free_int64_list(list_64t* list);
int resize_int64(list_64t* list,int new_size);
void insert_int64(list_64t* list, int index, int64_t value);
void print_int64_array(list_64t* list);

#endif // ARRAY_INT64_h
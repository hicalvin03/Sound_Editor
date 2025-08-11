#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef struct chunk chunk;

struct chunk {
    int16_t* data;
    size_t len;
    int start_index;
    int16_t* start_array;
    chunk* next;
};

typedef struct chunk_list { // stores pointers to either only parents or only children.
    chunk** data;    // pointer to array of chunk* or chunk pointers.
    int list_max_size;   // Maximum capacity of chunks in list.
    int list_size;       // Current number of chunks.
} chunk_list;

chunk* chunk_init(int16_t* data,int start_index, size_t len);
void free_chunk(chunk* node);
void free_chunk(chunk* node);
void init_chunk_list(chunk_list* list, int list_max_size);
void add_chunk(chunk_list* list, chunk* data_member);
void free_chunk_list(chunk_list* list);
chunk_list* shallow_copy_chunk_list(chunk_list* original);



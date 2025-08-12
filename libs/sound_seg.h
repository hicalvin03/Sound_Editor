#ifndef SOUND_SEG_H  
#define SOUND_SEG_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../libs/load_WAV.h"
#include "../libs/array_int64.h"
#include "../libs/chunk_list.h"


double dot_product(int16_t* arr1,int16_t* arr2, size_t size);

typedef struct {
    chunk* head_node;
    size_t total_length;
} sound_seg;

sound_seg* tr_init();
void tr_destroy(sound_seg* obj);
size_t tr_length(sound_seg* seg);
void tr_read(sound_seg* track, int16_t* dest, size_t pos, size_t len);
void tr_write(sound_seg* track, int16_t* src, size_t pos, size_t len);
bool tr_delete_range(sound_seg* track, size_t pos, size_t len);
char* solution_formatter(list_64t* solution);
void convert_into_1array(sound_seg* track,int16_t* array);
char* tr_identify(sound_seg* target,sound_seg* ad);
double dot_product(int16_t* arr1,int16_t* arr2, size_t size);
chunk* find_node_with_pos(chunk* head, size_t pos,int* local_index, chunk** prev);
chunk* split(chunk* node_2_split, size_t local_split_pos, bool right_contain);
chunk* split_dest_track(sound_seg* dest_track, size_t destpos);
chunk* create_child_copy(chunk* parent_node);
chunk* connect_chunk_list(chunk_list* children_nodes, chunk** head_ref, chunk* prev_node);
void recalc_start_indexes(sound_seg* track);
void tr_insert(sound_seg* src_track,sound_seg* dest_track,size_t destpos, size_t srcpos, size_t len);
void print_int16_array(const int16_t* arr, size_t len);
void print_sound_seg(sound_seg* seg);


#endif
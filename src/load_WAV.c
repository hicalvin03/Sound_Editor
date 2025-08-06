#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../libs/load_WAV.h"

// Load a WAV file into buffer int16_t* dest.
void wav_load(const char* filename, int16_t* dest){
    FILE* file = fopen(filename,"rb");
    
    int32_t audio_size;
    fseek(file,40,SEEK_CUR);
    fread(&audio_size,4,1,file);
    fread(dest,audio_size,1,file);
    fclose(file);
    return;
}

// Create/write a WAV file with fname from buffer
void wav_save(const char* fname, int16_t* src, size_t len){
    FILE* file = fopen(fname,"wb");

    //RIFF header
    int32_t ChunkSize = 36 + len * sizeof(int16_t);

    //FMT section
    int32_t subchunk1_size = 16;                        
    int16_t audio_format = 1;                          
    int16_t num_channels = 1;                          
    int32_t sample_rate = 8000;    
    int32_t Byte_Rate = sample_rate*num_channels*sizeof(int16_t);
    int16_t Block_Align = num_channels*sizeof(int16_t);
    int16_t Bits_Per_Sample = 16;
    
    //data
    int32_t Subchunk2Size = len*sizeof(int16_t);

    fwrite("RIFF", 1, 4, file);                          
    fwrite(&ChunkSize, 4, 1, file);                     
    fwrite("WAVE", 1, 4, file); 
    fwrite("fmt ", 1, 4, file);                         
    fwrite(&subchunk1_size, 4, 1, file);                
    fwrite(&audio_format, 2, 1, file);                 
    fwrite(&num_channels, 2, 1, file);                  
    fwrite(&sample_rate, 4, 1, file);                 
    fwrite(&Byte_Rate, 4, 1, file);                     
    fwrite(&Block_Align, 2, 1, file);                   
    fwrite(&Bits_Per_Sample, 2, 1, file);               
    fwrite("data", 1, 4, file);                        
    fwrite(&Subchunk2Size, 4, 1, file);                
    fwrite(src, sizeof(int16_t), len, file);            

    fclose(file);

    return;
}

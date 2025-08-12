all: sound_seg.o

sound_seg.o: src/sound_seg.c src/load_WAV.c libs/load_WAV.h libs/array_int64.h libs/chunk_list.h
	gcc -c -Wall -Werror -g -fPIC -I./libs src/sound_seg.c -o sound_seg_temp.o
	gcc -c -Wall -Werror -g -fPIC -I./libs src/load_WAV.c
	gcc -c -Wall -Werror -g -fPIC -I./libs src/array_int64.c
	gcc -c -Wall -Werror -g -fPIC -I./libs src/chunk_list.c
	gcc -r sound_seg_temp.o load_WAV.o array_int64.o chunk_list.o -o sound_seg.o

asan:
	make
	gcc -g -fsanitize=address sound_seg.o -o sound_seg.out

noasan:
	make
	gcc sound_seg.o -o sound_seg.out

debug:
	gcc -g -Wall -Werror -I./libs src/test.c src/sound_seg.c src/load_WAV.c src/array_int64.c src/chunk_list.c -o test

debug1:
	gcc -g -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -I./libs src/sound_seg.c src/load_WAV.c src/array_int64.c -o sound_seg

clean:
	rm -f sound_seg.o load_WAV.o sound_seg_temp.o sound_seg.out sound_seg sound_seg_debug array_int64.o chunk_list.o

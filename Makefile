
HDR += src/linenoise/linenoise.h 
HDR += src/cmd3/cmd3.h
HDR += src/cmd3/uthash.h

SRC += src/linenoise/linenoise.c 
SRC += src/cmd3/cmd3.c 

SRC += src/example.c

cmd3_example: $(HDR) $(SRC)

cmd3_example: $(SRC)
	$(CC) -Wall -Werror -Os -g -I./src -o cmd3_example $(SRC)

clean:
	rm -f cmd3_example

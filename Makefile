
HDR += src/linenoise/linenoise.h 
HDR += src/cmd3/cmd3.h
HDR += src/cmd3/uthash.h

SRC += src/linenoise/linenoise.c 
SRC += src/cmd3/cmd3.c 

SRC += src/example.c

OBJ = $(SRC:.c=.o)

EXEC = cmd3_example

.PHONY: all
all: utest $(EXEC)

.PHONY: utest
utest:
	$(MAKE) -C unit_tester

$(EXEC):$(OBJ)
	$(CC) -Wall -Werror -O0 -g -o $(EXEC) *.o

%.o: %.c $(HDR) 
	$(CC) -Wall -Werror -O0 -g -c -I./src $<

clean:
	rm -f $(EXEC)
	rm -f *.o
	$(MAKE) -C unit_tester $@

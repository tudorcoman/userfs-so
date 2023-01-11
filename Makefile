COMPILER := gcc
FILESYSTEM_FILES := userfs.c
CFLAGS := `pkg-config fuse --cflags --libs`
LIBS := `pkg-config fuse --libs`

SRC := utils.c active_users.c processes.c userfs.c 

OBJ := $(SRC:.c=.o)

.PHONY: all
all: userfs 

%.o:%.c
	$(COMPILER) -c $(CFLAGS) $^ -o $@ -I.


userfs: userfs.o utils.o active_users.o processes.o
	${COMPILER} $^ -o $@ $(LIBS)


.PHONY: clean 
clean:
	rm $(OBJ)


.PHONY: cleanall
cleanall:
	rm $(OBJ) userfs 


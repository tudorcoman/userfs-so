COMPILER = gcc
FILESYSTEM_FILES = userfs.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o userfs `pkg-config fuse --cflags --libs`
	echo 'To Mount: ./userfs -f [mount point]'

clean:
	rm userfs
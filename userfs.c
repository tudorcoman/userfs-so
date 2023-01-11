#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "active_users.h"
#include "processes.h"

ActiveUserList users;
pthread_mutex_t lock;

static int do_getattr(const char *path, struct stat *st) {
    pthread_mutex_lock(&lock);
    update_users(&users);
    pthread_mutex_unlock(&lock);

    int setat = 0; 

    printf( "[getattr] Called\n" );
    printf( "\tAttributes of %s requested\n", path );

    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);

    if (strcmp(path, "/") == 0) {
	st->st_mode = S_IFDIR | 0755;
	st->st_nlink = 2 + users.count; // directorul curent (.), directorul parinte (..) + subdirectoarele pentru users
        setat = 1;
    }

    int i;
    for (i = 0; i < users.count && !setat; i ++) {
        char cale_user[2 + MAX_CHARS_USERNAME];
        sprintf(cale_user, "/%s", users.active_users[i]);
        if (strcmp(path, cale_user) == 0) {
            st->st_mode = S_IFDIR | 0755;
		    st->st_nlink = 2; // . si ..
            setat = 1;
        }
    }

    if (setat == 0) {
	st->st_mode = S_IFREG | 0644;
	st->st_nlink = 1;
	st->st_size = 1024;
        setat = 1;
    }

    return 0;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    pthread_mutex_lock(&lock);
    update_users(&users);
    pthread_mutex_unlock(&lock);

    printf( "--> Getting The List of Files of %s\n", path );
    
    filler(buffer, ".", NULL, 0); // director curent
    filler(buffer, "..", NULL, 0); // director parinte

    int i;

    if (strcmp(path, "/") == 0) {
        int i;
        for (i = 0; i < users.count; i ++) {
            char cale_user[10 + MAX_CHARS_USERNAME];
            sprintf(cale_user, "%s", users.active_users[i]);
            filler(buffer, cale_user, NULL, 0);
        }
    } else {
        for (i = 0; i < users.count; i ++) {
            char cale_user[10 + MAX_CHARS_USERNAME];
            sprintf(cale_user, "/%s", users.active_users[i]);
            if (strcmp(path, cale_user) == 0) {
                filler(buffer, "procs", NULL, 0);
                return 0;
            }
        }
    }

    return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
    pthread_mutex_lock(&lock);
    update_users(&users);
    pthread_mutex_unlock(&lock);

    printf( "--> Trying to read %s, %u, %u\n", path, offset, size);

    char file_text[10 * MAX_PROCESSES];
    memset(file_text, 0, sizeof file_text);
    int done = 0, i = 0;

    for (i = 0; i < users.count && !done; i ++) {
        char cale[10 + MAX_CHARS_USERNAME];
        sprintf(cale, "/%s/procs", users.active_users[i]);
        if (strcmp(path, cale) == 0) {
            int* procese = malloc((MAX_PROCESSES + 1) * sizeof(int));
	        get_processes(procese, users.active_users[i]);
            int j;
            for (j = 1; j < procese[0]; j ++) {
                sprintf(file_text + strlen(file_text), "%d\n", procese[j]);
            }
            done = 1;
        }
    }

    if(!done) {
        return -1;
    }

    char* selectedText = file_text;
    // ... //

    memcpy(buffer, selectedText + offset, size);

    return strlen(selectedText) - offset;
}

static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .readdir	= do_readdir,
    .read		= do_read,
};

int main(int argc, char *argv[]) {
    update_users(&users);
    return fuse_main(argc, argv, &operations, NULL);
}

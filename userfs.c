#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

/*
Functie care sa returneze lista de useri
Functie care pentru numele unui user sa returneze lista de procese 
*/

#define MAX_CHARS_USERNAME 512
#define MAX_LOGGED_IN_USERS 128
#define MAX_PROCESSES 65536

typedef struct ActiveUserList{
    char* active_users[MAX_LOGGED_IN_USERS];
    int count;
} ActiveUserList;

ActiveUserList users;

ActiveUserList get_active_users() {
    FILE *pipe_output_comanda;
    char* command = "who | awk '{print $1}' | sort -u";
    char data[MAX_CHARS_USERNAME]; // presupunem ca un nume de user are maxim 512 caractere

    // Setup our pipe for reading and execute our command.
    pipe_output_comanda = popen(command,"r");

    // Error handling

    ActiveUserList x;
    int i = 0;
    // Get the data from the process execution
    while(fgets(data, MAX_CHARS_USERNAME, pipe_output_comanda)) {
        // in data avem fiecare user
        x.active_users[i] = (char*) malloc(strlen(data) * sizeof(char));
        int n = strlen(data);
        data[n - 1] = '\0';
        strcpy(x.active_users[i], data);
        i ++;
    }

    if (pclose(pipe_output_comanda) != 0)
        fprintf(stderr," Error: Failed to close command stream \n");

    x.count = i;
    return x;
}

void get_processes(int pids[MAX_PROCESSES], char* user) {
    FILE *pipe_output_comanda;
    char command[40 + MAX_CHARS_USERNAME];
    sprintf(command, "ps -u %s | awk '{print $1}' | tail -n +2", user);

    // Setup our pipe for reading and execute our command.
    pipe_output_comanda = popen(command,"r");

    int pid_curent;
    //int pids[MAX_PROCESSES];
    //int* pids = malloc((MAX_PROCESSES + 1) * sizeof(int));
    memset(pids, 0, sizeof pids);
    while(fscanf(pipe_output_comanda, "%d", &pids[++ pids[0]]) != EOF);

    if (pclose(pipe_output_comanda) != 0)
        fprintf(stderr," Error: Failed to close command stream \n");
    //return pids;
}


static int do_getattr( const char *path, struct stat *st ) {
    users = get_active_users();
    int setat = 0; 

	printf( "[getattr] Called\n" );
	printf( "\tAttributes of %s requested\n", path );

	// GNU's definitions of the attributes (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
	// 		st_uid: 	The user ID of the file’s owner.
	//		st_gid: 	The group ID of the file.
	//		st_atime: 	This is the last access time for the file.
	//		st_mtime: 	This is the time of the last modification to the contents of the file.
	//		st_mode: 	Specifies the mode of the file. This includes file type information (see Testing File Type) and the file permission bits (see Permission Bits).
	//		st_nlink: 	The number of hard links to the file. This count keeps track of how many directories have entries for this file. If the count is ever decremented to zero, then the file itself is discarded as soon
	//						as no process still holds it open. Symbolic links are not counted in the total.
	//		st_size:	This specifies the size of a regular file in bytes. For files that are really devices this field isn’t usually meaningful. For symbolic links this specifies the length of the file name the link refers to.

	st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

	if (strcmp( path, "/" ) == 0) {
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2 + users.count; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
        setat = 1;
	}

    int i;
    for (i = 0; i < users.count && !setat; i ++) {
        char cale_user[2 + MAX_CHARS_USERNAME];
        sprintf(cale_user, "/%s", users.active_users[i]);
        if (strcmp(path, cale_user) == 0) {
            st->st_mode = S_IFDIR | 0755;
		    st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
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

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ) {
    users = get_active_users();
	printf( "--> Getting The List of Files of %s\n", path );

	filler( buffer, ".", NULL, 0 ); // Current Directory
	filler( buffer, "..", NULL, 0 ); // Parent Directory

    int i;

	if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
	{
        int i;
        for (i = 0; i < users.count; i ++) {
            char cale_user[10 + MAX_CHARS_USERNAME];
            sprintf(cale_user, "%s", users.active_users[i]);
            filler(buffer, cale_user, NULL, 0);
        }
		//filler( buffer, "file54", NULL, 0 );
		//filler( buffer, "file349", NULL, 0 );
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

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    users = get_active_users();
	printf( "--> Trying to read %s, %u, %u\n", path, offset, size );

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

	memcpy( buffer, selectedText + offset, size );

	return strlen( selectedText ) - offset;
}

static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .readdir	= do_readdir,
    .read		= do_read,
};

int main( int argc, char *argv[] ) {
    users = get_active_users();
	return fuse_main( argc, argv, &operations, NULL );
}

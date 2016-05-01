//Forrest Tappan
//Prject 1
//27-Apr-2016

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fuse.h"
#include "cfuhash.h"

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION  26
#include <fuse.h>

static char  *file_path      = NULL;
static char  *file_content   = NULL;
static size_t file_size      = 0;

BLOCK *memory[NUM_OF_BLOCKS];
SYSTEM_TABLE systemOpenTable;
PROCESS_TABLE processes[100];
char bitVector[NUM_OF_BLOCKS] = {0};


static int file_getattr(const char *path, struct stat *stbuf)
{
    fprintf(stderr, "GETATTR\n");

    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) { /* The root directory of our file system. */
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = (file_path == NULL ? 2 : 3); // if we created the file
    } else {
        if (file_path == NULL)
            return -ENOENT;

        if (strcmp(path, file_path) == 0) {
            stbuf->st_mode = S_IFREG | 0666;
            stbuf->st_nlink = 1;
            stbuf->st_size = file_size;
        } else { /* We reject everything else. */
            return -ENOENT;
      }
    }

    return 0;
}

static int
single_open(const char *path, struct fuse_file_info *fi)
{
    fprintf(stderr, "OPEN\n");

    if (file_path == NULL)
        return -ENOENT;
        
    if (strcmp(path, file_path) != 0)
    { // we only recognize one file.
        return -ENOENT;
    }

    int i;
    int count = 0;
    int found = 0;
    int processIndex;
    PROCESS_ENTRY *process = malloc(sizeof(PROCESS_ENTRY));
    INODE *temp = malloc(sizeof(INODE));
    INODE search;

// find correct process
    for (i = 0; i < 100 && found == 0; i++)
    {
        if (processes[i].processId == id)
        {
            found = 1;
            processIndex = i;
        }
    }
    found = 0;

// search system table for entry
    for (i = 0; i < DIR_SIZE && found == 0; i++)
    {
        if (systemOpenTable.entries[i].name != NULL)
        {
            if (strcmp(systemOpenTable.entries[i].name, name) == 0)
            {
                found = 1;
                process->systemTableEntry = malloc(sizeof(INODE));
                process->systemTableEntry = &systemOpenTable.entries[i];
                process->processId = id;
                processes[processIndex].entries[processes[processIndex].count] = malloc(sizeof(PROCESS_ENTRY));
                processes[processIndex].entries[processes[processIndex].count] = process;
                processes[processIndex].count++;
            }
        }
    }

    if (found == 0)
    {
        temp = cfuhash_get(table, name);
        search = systemOpenTable.entries[count];

        while (search.name != NULL)
        {
            search = systemOpenTable.entries[count];
            count++;
        }

        systemOpenTable.entries[count] = *temp;
        systemOpenTable.count++;

        process->systemTableEntry = malloc(sizeof(INODE));
        process->access = malloc(sizeof(char) * strlen(systemOpenTable.entries[count].accessRights));
        process->systemTableEntry = &systemOpenTable.entries[count];
        process->access = systemOpenTable.entries[count].accessRights;
        process->processId = id;
        process->systemTableIndex = count;
        processes[processIndex].entries[processes[processIndex].count] = malloc(sizeof(PROCESS_ENTRY));
        processes[processIndex].entries[processes[processIndex].count] = process;
        processes[processIndex].count++;
    }

    return process;
}

    return 0;
}

static struct fuse_operations file_system = {
    .getattr  = file_getattr,
    .getxattr = file_getxattr,
    .create   = file_create,
    .mknod    = file_mknod,
    .utime    = file_utime,
    .unlink   = file_unlink,
    .truncate = file_truncate,
    .flush    = file_flush,
    .fsync    = file_fsync,
    .access   = file_access,
    .open     = file_open,
    .statfs   = file_statfs,
    .release  = file_release,
    .read     = file_read,
    .write    = file_write,
    .readdir  = file_readdir,
}

int main(int argc, char **argv)
{
    return fuse_main(argc, argv, &file_system, NULL);
}

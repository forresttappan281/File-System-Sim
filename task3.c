//Forrest Tappan
//Project1
//27-Apr-2016

#include "task3.h"
#include "cfuhash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BLOCK *memory[NUM_OF_BLOCKS];
char bitVector[NUM_OF_BLOCKS] = {0};

void *create_file(cfuhash_table_t *table, char *name, int size, char *accessRights, char *data)
{
    int j, k, freeBlockIndex, dataBlockIndex;
    int found = 0;
    INODE *i = malloc(sizeof(INODE));
    i->name = malloc(sizeof(char) * strlen(name));
    i->accessRights = malloc(sizeof(char) * strlen(accessRights));

    strcpy(i->name, name);
    strcpy(i->accessRights, accessRights);
    i->size = size;

//  find free block using bit vector

    for (j = 0; j < NUM_OF_BLOCKS && found == 0; j++)
    {
        if (!BITOP(bitVector, j, &))
        {
            found = 1;
            freeBlockIndex = j;
            memory[j]->type = INDEX;  //make index block
            BITOP(bitVector, j, |=); //set bit not free
            i->dataIndex = j;

            for (k = 0; k < BLOCK_SIZE / sizeof(int) - sizeof(BLOCK_TYPE); k++)
            {
                memory[j]->content.indexBlock[k] = -1;
            }
        }
    }

    found = 0;
    for (j = 0; j < NUM_OF_BLOCKS && found == 0; j++)
    {
        if (!BITOP(bitVector, j, &))
        {
            found = 1;
            dataBlockIndex = j;
            memory[j]->type = DATA; //make data block
            strcpy(memory[j]->content.data, data);
            BITOP(bitVector, j, |=);
        }
    }


    cfuhash_put(table, name, i); // add to directory

// put data block index into index block
    found = 0;
    for (j = 0; j < j < BLOCK_SIZE / sizeof(int) - sizeof(BLOCK_TYPE) && found == 0; j++)
    {
        if (memory[freeBlockIndex]->content.indexBlock[j] == -1)
        {
            found = 1;
            memory[freeBlockIndex]->content.indexBlock[j] = dataBlockIndex;
        }
    }
}

void delete_file(cfuhash_table_t *table, char *name)
{
    int i;
    INODE *inode = malloc(sizeof(INODE));
    inode = cfuhash_get(table, name);
    cfuhash_delete(table, name);

    if (inode == NULL)
    {
        fprintf(stderr, "File does not exist");
    }
    else
    { // erase index from index block
        for (i = 0; i < BLOCK_SIZE / sizeof(int) - sizeof(BLOCK_TYPE); i++)
        {
            if (memory[inode->dataIndex]->content.indexBlock[i] != -1)
            {
                memory[memory[inode->dataIndex]->content.indexBlock[i]] = NULL;
                BITOP(bitVector, i, &=~); // set bit to free
            }
        }
        // deallocate data block
        memory[inode->dataIndex] = NULL;
        BITOP(bitVector, inode->dataIndex, &=~);
    }
}

int main(int argc, char **argv)
{
    int i;
    for (i = 0; i < NUM_OF_BLOCKS; i++)
    {
        memory[i] = malloc(sizeof(BLOCK));
    }

    cfuhash_table_t *directory = cfuhash_new_with_initial_size(DIR_SIZE);

    cfuhash_set_flag(directory, CFUHASH_FROZEN_UNTIL_GROWS);

    printf("\nCreated file hello.txt\nPrinting directory:\n\n");

    create_file(directory, "hello.txt", 9, "444", "hello");
    cfuhash_pretty_print(directory, stdout);

    printf("\nDeleting file hello.txt\nDirectory after deletion:\n\n");
    delete_file(directory, "hello.txt");
    cfuhash_pretty_print(directory, stdout);
}

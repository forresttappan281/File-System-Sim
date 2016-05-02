

#include "system_func.h"
#include "cfuhash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    for (j = 0; j < NUM_OF_BLOCKS && found == 0; j++)
    {
        if (!BITOP(bitVector, j, &))
        {
            found = 1;
            freeBlockIndex = j;
            memory[j].type = INDEX;
            BITOP(bitVector, j, |=);
            i->dataIndex = j;

            for (k = 0; k < BLOCK_SIZE / sizeof(int) - sizeof(BLOCK_TYPE); k++)
            {
                memory[j].content.indexBlock[k] = -1;
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
            memory[j].type = DATA;
            strcpy(memory[j].content.data, data);
            BITOP(bitVector, j, |=);
        }
    }


    cfuhash_put(table, name, i);

    found = 0;
    for (j = 0; j < j < BLOCK_SIZE / sizeof(int) - sizeof(BLOCK_TYPE) && found == 0; j++)
    {
        if (memory[freeBlockIndex].content.indexBlock[j] == -1)
        {
            found = 1;
            memory[freeBlockIndex].content.indexBlock[j] = dataBlockIndex;
        }
    }
}

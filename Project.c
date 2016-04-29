//Forrest Tappan
//Project 1


#include "Project.h"
#include "cfuhash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BLOCK *memory[NUM_OF_BLOCKS];
SYSTEM_TABLE systemOpenTable;
PROCESS_TABLE processes[100];
char bitVector[NUM_OF_BLOCKS] = {0};

void print_menu()
{
    printf("1. Create a file\n2. Delete a file\n3. Read a file\n4. Write to a file\n5. Close a file\n6. Print directory\n7. Exit\n");
}

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
            memory[j]->type = INDEX;
            BITOP(bitVector, j, |=);
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
            memory[j]->type = DATA;
            strcpy(memory[j]->content.data, data);
            BITOP(bitVector, j, |=);
        }
    }


    cfuhash_put(table, name, i);

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
    {   // erase index from index block
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

PROCESS_ENTRY *open_file(cfuhash_table_t *table, char *name, int id)
{
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
// if found, create entry in system table and create entry in process table
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

// if file is not already open by a process
    if (found == 0)
    {
        temp = cfuhash_get(table, name);
        if (temp == NULL)
        {
            fprintf(stderr, "File does not exist");
        }
        else
        {
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
    }

    return process;
}

void read_file(PROCESS_ENTRY *p)
{
    int j;
    int found = 0;
    int indexBlock = p->systemTableEntry->dataIndex;
    int dataBlock = memory[indexBlock]->content.indexBlock[0];

// check permission
    if (p->access[0] != '4' && p->access[0] != '5' && p->access[0] != '6' && p->access[0] != '7')
    {
        fprintf(stderr, "Cannot read file\n");
    }
    else
    {
        for (j = 0; j < NUM_OF_BLOCKS && found == 0; j++)
        {
            if (!BITOP(bitVector, j, &))
            {
                found = 1;
                memory[j]->type = META;
                memory[j]->content.inode = malloc(sizeof(INODE));
                memory[j]->content.inode = p->systemTableEntry;
                BITOP(bitVector, j, |=);
            }
        }

// read from data block
        printf("%s\n", memory[dataBlock]->content.data);
    }
}

void write_file(PROCESS_ENTRY *p)
{

    int indexBlock = p->systemTableEntry->dataIndex;
    int dataBlock = memory[indexBlock]->content.indexBlock[0];
    char *newData = malloc(sizeof(char) * 250);


    if (p->access[0] != '6' && p->access[0] != '7')
    {
        fprintf(stderr, "You do not have permission to write to file\n");
    }
    else
    {
// copy new data into data block
        scanf("%s", newData);
        strcpy(memory[dataBlock]->content.data, newData);
    }
}

void close_file(PROCESS_ENTRY *p, int id)
{
    int i, j, found, found1 = 0;

// find file opened by process and remove entry from process table

    for (i = 0; i < 100 && found == 0; i++)
    {
        if (processes[i].processId == id)
        {
            found = 1;

            for (j = 0; j < DIR_SIZE && found1 == 0; j++)
            {
                if (strcmp(processes[i].entries[j]->systemTableEntry->name, p->systemTableEntry->name) == 0)
                {
                    found1 = 1;
                    processes[i].entries[j] = NULL;
                }
            }
        }
    }

// remove system table entry
    systemOpenTable.entries[p->systemTableIndex].name = NULL;
    free(p);
}

int main(int argc, char **argv)
{
    int i, selection;
    char *name = malloc(sizeof(char) * 20);
    char *content = malloc(sizeof(char) * (BLOCK_SIZE - sizeof(BLOCK_TYPE)));
    PROCESS_ENTRY *process = malloc(sizeof(PROCESS_ENTRY));
    process->systemTableEntry = malloc(sizeof(INODE));

    for (i = 0; i < NUM_OF_BLOCKS; i++)
    {
        memory[i] = malloc(sizeof(BLOCK));
    }

    cfuhash_table_t *directory = cfuhash_new_with_initial_size(DIR_SIZE);

    cfuhash_set_flag(directory, CFUHASH_FROZEN_UNTIL_GROWS);

    systemOpenTable.count = 0;

    printf("Creating file hello.txt\n\nPrinting directory\n");
    create_file(directory, "hello.txt", 34, "644", "hello");
    cfuhash_pretty_print(directory, stdout);
    printf("Reading file hello.txt\n");
    process = open_file(directory, "hello.txt", 1);
    read_file(process);

    printf("Write to hello.txt ");
    write_file(process);

    printf("File contents after writing: ");
    read_file(process);

    printf("Deleting file hello.txt\n\nPrinting directory\n");

    delete_file(directory, "hello.txt");

    cfuhash_pretty_print(directory, stdout);
}

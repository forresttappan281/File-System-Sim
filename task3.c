#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#define BLOCK_SIZE 256
#define MAX_NAME_LENGTH 128
#define DATA_SIZE 254
#define INDEX_SIZE 127

#define NUM_BLOCKS 65536
#define BIT_VECTOR_LEN 8192

typedef enum
{
   DIREC, 
   FYLE, 
   INDEX,
   DATA
} NODE_TYPE;

typedef struct fs_node
{
   char name[MAX_NAME_LENGTH];
   time_t creat_t; // creation time
   time_t access_t; // last access
   time_t mod_t; // last modification
   mode_t access; // access rights for the file
   unsigned short owner; // owner ID
   unsigned short size;
   unsigned short block_ref; // reference to the data or index block
} FS_NODE;

typedef struct node
{
   NODE_TYPE type;
   union
   {
      FS_NODE fd;
      //data_t data[DATA_SIZE];
      char data[DATA_SIZE];
      unsigned short index[INDEX_SIZE];
   } content; 
} NODE;

// storage blocks
NODE *memory; // allocate 2^16 blocks (in init)

// bit vector
char *bitvector; // allocate space for managing 2^16 blocks (in init)

short find_open_block()
{
  int i;
  short idx = -1;
  short bitmask = 1;

  short stuff;
  for(i = 0; i < sizeof(bitvector); i++)
  {
    stuff =  bitvector[i/sizeof(char)] & (1 << (i % sizeof(char)));
    if(stuff == 0)
    {
      idx = i;
      // marks occupied
      bitvector[i/sizeof(char)] = bitvector[i/sizeof(char)] | (1 << (i % sizeof(char)));
      break;
    }
  }
  return idx;
}

void create_dir(NODE *parent,char *name)
{
  // quit if your name is stupid long
  if(strlen(name) > MAX_NAME_LENGTH)
  {
    printf("You came to the wrong neighborhood\n");
    exit(0);
  }

  short idx_open_block = find_open_block();
  // quit if there are no blocks available
  if(idx_open_block == -1)
  {
    printf("You can't cee John Cena\n");
    exit(0);
  }
  NODE *f = calloc(1,sizeof(NODE));
  f->type = DIREC;
  strcpy(f->content.fd.name,name);
  f->content.fd.creat_t = time(NULL);
  f->content.fd.access_t = time(NULL);
  f->content.fd.mod_t = time(NULL);
  f->content.fd.access = 0x644;
  f->content.fd.owner = getuid();
  f->content.fd.size = 0;
  if(parent != NULL)
  {
    unsigned short snapcaster = parent->content.fd.block_ref;
    memory[snapcaster].content.index[parent->content.fd.size] = idx_open_block; 
    parent->content.fd.size++;
  }
  memory[idx_open_block] = *f;
  unsigned short idx_block_loc = find_open_block();
  memory[idx_open_block].content.fd.block_ref = idx_block_loc; 

  if(idx_open_block == -1)
  {
    printf("YOU CAN'T SEE JOHN CEEEEEEEEENAAAAAAA\n");
    exit(0);
  }

  NODE *idx_block = calloc(1,sizeof(NODE));
  idx_block->type = INDEX;
  memory[idx_block_loc] = *idx_block;

}

void create_file(NODE *parent,char *name,unsigned short size)
{
  // quit if your name is stupid long
  if(strlen(name) > MAX_NAME_LENGTH)
  {
    printf("You came to the wrong neighborhood\n");
    exit(0);
  }

  short idx_open_block = find_open_block();
  // quit if there are no blocks available
  if(idx_open_block == -1)
  {
    printf("You can't see John Cena\n");
    exit(0);
  }
  NODE *f = calloc(1,sizeof(NODE));
  f->type = FYLE;
  strcpy(f->content.fd.name,name);
  f->content.fd.creat_t = time(NULL);
  f->content.fd.access_t = time(NULL);
  f->content.fd.mod_t = time(NULL);
  f->content.fd.access = 0x644;
  f->content.fd.owner = getuid();
  f->content.fd.size = size;
  unsigned short snapcaster = parent->content.fd.block_ref;
  memory[snapcaster].content.index[parent->content.fd.size] = idx_open_block; 
  parent->content.fd.size++;
  memory[idx_open_block] = *f;
  // create index block if necessary
  if(size > DATA_SIZE)
  {
    unsigned short idx_block_loc = find_open_block();
    memory[idx_open_block].content.fd.block_ref = idx_block_loc; 
    if(idx_open_block == -1)
    {
      printf("YOU CAN'T SEE JOHN CEEEEEEEEENAAAAAAA\n");
      exit(0);
    }
    NODE *idx_block = calloc(1,sizeof(NODE));
    idx_block->type = INDEX;
    memory[idx_block_loc] = *idx_block;
    int i;
    unsigned short data_block_loc;
    for(i = 0; i < size/BLOCK_SIZE; i++)
    {
      data_block_loc = find_open_block();
      if(data_block_loc == -1) exit(0);
      NODE *dat_node = calloc(1,sizeof(NODE));
      dat_node->type = DATA;
      memory[data_block_loc] = *dat_node;
      memory[idx_block_loc].content.index[i] = data_block_loc; 
    }
  }
  else //create data block
  {
    unsigned short idx_block_loc = find_open_block();
    memory[idx_open_block].content.fd.block_ref = idx_block_loc;
    NODE *data = calloc(1,sizeof(NODE));
    data->type = DATA; 
    memory[idx_block_loc] = *data;
  }
}

int find_file(char *name)
{
  int idx,i;
  for(i = 0; i < NUM_BLOCKS; i++)
  {
    if(strcmp(memory[i].content.fd.name,name) == 0)
    {
      idx = i;
    }
  }
  return idx;
}

void delete_dir(char *name)
{
}

void delete_file(char *name)
{
}

void file_info(char *name)
{
  int i;
  for(i = 0; i < NUM_BLOCKS; i++)
  {
    if(strcmp(memory[i].content.fd.name,name) == 0)
    {
      printf("Name - %s\n",memory[i].content.fd.name);
      printf("Location in Memory - %hi\n",memory[i].content.fd.block_ref);
      printf("Created - %ld\n",memory[i].content.fd.creat_t);
      printf("Last accessed - %lu\n",memory[i].content.fd.access_t);
      printf("Last modified - %lu\n",memory[i].content.fd.mod_t);
      printf("Owner - %d\n",memory[i].content.fd.owner);
      printf("Permissions - %x\n",memory[i].content.fd.access);
      printf("Size - %d\n\n",memory[i].content.fd.size);
      break;
    }
  }
}

void create_system()
{
  memory = calloc(NUM_BLOCKS,sizeof(NODE));
  bitvector = calloc(BIT_VECTOR_LEN,sizeof(char));
  create_dir(NULL,"/");
}

int main()
{
  create_system();
  create_file(&memory[find_file("/")],"fogfrog",0);
  file_info("/");
  file_info("fogfrog");
  
  return 0;
}

//Forrest Tappan
//Project 1 Task 3
//18-Apr-2016

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BLOCK_SIZE 256
#define MAX_NAME_LENGTH 128
#define DATA_SIZE 254
#define INDEX_SIZE 127

#define NUM_BLOCKS 65536
#define VECT_LEN 8192

void create_dir(char*);
void create_FILE(NODE,char*,unsigned short);
short get_block_ref(char *name,NODE_TYPE type);

typedef enum
{
  DIR_TYPE,
  FILE_TYPE,
  IDX_TYPE,
  DAT_TYPE,
} NODE_TYPE;

typedef struct fs_node
{
  char name[MAX_NAME_LENGTH];
  time_t creat_t;
  time_t access_t;
  time_t mod_t;
  mode_t access;
  unsigned short owner;
  unsigned short size;
  unsigned short block_ref;
} FS_NODE;

typedef struct node
{
  NODE_TYPE type;
  union
  {
    FS_NODE fd;
    
    char data[DATA_SIZE];
    unsigned short index[INDEX_SIZE];
  } content;
} NODE;

NODE *memory;
char *bitvector/*[mem_size/8]*/;

void create_system()
{
  memory = calloc(NUM_BLOCKS,sizeof(NODE));
  bitvector = calloc(VECT_LEN,sizeof(char));
  create_dir("/");
}

short find_open_block()
{
  int i;
  short idx = NUM_BLOCKS;
  short mask = 1;

  for(i = 0; i < sizeof(bitvector); i++)
  {
    if(bitvector[i/sizeof(char)] & (1 << (i % sizeof(char))) == 0)
    {
      idx = i;
      bitvector[i/sizeof(char)] = bitvector[i/sizeof(char)] | (1 << (sizeof(char)));
      break;
    }
  }
  return idx;
}

void create_dir(NODE *parent,char *name)
{
  if(strlen(name) > MAX_NAME_LENGTH) exit(0); 
  short open_idx = find_open_block();
  if(open_idx == NUM_BLOCKS) exit(0); 

  NODE *dir_node = calloc(1,sizeof(NODE));
  dir_node->type = DIR_TYPE;
  strcpy(dir_node->content.fd.name,name);

  dir_node->content.fd.creat_t = time(NULL); 
  dir_node->content.fd.access_t = time(NULL);
  dir_node->content.fd.mod_t = time(NULL);
  dir_node->content.fd.access = 0x644;
  dir_node->content.fd.owner = getuid();
  dir_node->content.fd.size = 0;

  if(parent != NULL)
  {
    unsigned short idx = parent->content.fd.block_ref;
    memory[idx].content.index[parent->content.fd.size] = open_idx;
    parent->content.fd.side++;
  }
  memory[open_idx] = *dir_node;
  unsigned short idx_block_loc = find_open_block();
  memory[idx_open_block].content.fd.block_ref = idx_block_loc;

  if(open_idx == NUM_BLOCKS) exit(0); 

  NODE *idx_block = calloc(1,sizeof(NODE));
  idx_block->type = IDX_TYPE;
  memory[idx_block_loc] = *idx_block;
}

short get_block_ref(char *name,NODE_TYPE type) 
{
  short idx = NUM_BLOCKS;
  int i = 0;
  bool found = false;

  while(!found) 
  {
    if(memory[i].type == type && (strcmp(memory[i].content.fd.name, name) == 0)) 
    {
      idx = i;
      found = true;
    }
    i++;
  }
  return idx;
}

void create_file(NODE *parent,char *name,unsigned short size)
{
  if(strlen(name) > MAX_NAME_LENGTH) exit(0); 
  short open_idx = find_open_block();
  if(open_idx == NUM_BLOCKS) exit(0); 
  NODE *file_node = calloc(1,sizeof(NODE));
  file_node->type = FILE_TYPE;
  strcpy(file_node->content.fd.name,name);

  file_node->content.fd.creat_t = time(NULL); 
  file_node->content.fd.access_t = time(NULL);
  file_node->content.fd.mod_t = time(NULL);
  file_node->content.fd.access = 0x644;
  file_node->content.fd.owner = getuid();
  file_node->content.fd.size = 0;

  unsigned short idx = parent->content.fd.block_ref;
  memory[idx].content.index[parent->content.fd.size] = open_idx;
  parent->content.fd.size++;
  memory[idx_open_block] = *file_node;
  unsigned short idx_block_loc = find_open_block();
  if(idx_block_loc == NUM_BLOCKS) exit(0); 

  if(size > DATA_SIZE)
  {
    memory[open_idx].content.fd.block_ref = idx_block_loc;
    NODE *idx_block = calloc(1,sizeof(NODE));
    idx_block->type = IDX_TYPE;
    memory[idx_block_loc] = *idx_block;
    int i;
    unsigned short data_block_loc;
    for(i = 0; i < size/BLOCK_SIZE; i++)
    {
      data_block_loc = find_open_block();
      if(data_block_loc == NUM_BLOCKS) exit();
      NODE *data = calloc(1,sizeof(NODE));
      data->type = DAT_TYPE;
      memory[data_block_loc] = *data;
      memory[idx_block_loc].content.index[i] = data_block_loc;
    }
  }
  else
  {
    memory[open_idx].content.fd.block_ref = idx_block_loc;
    NODE *data = calloc(1,sizeof(NODE));
    data->type = DAT_TYPE;
    memory[idx_block_loc] = *data;
  }
}

void open_block(unsigned short block_ref) 
{
  bitvector[block_ref/sizeof(char)] = bitvector[block_ref/sizeof(char)] & ~(1 << (block_ref % sizeof(char)));
}

void delete_dir(unsigned short file_descr_ref)
{
  int i;
  unsigned short file_descr_ref;
  unsigned short idx_block_ref = memory[dir_ref].content.fd.block_ref;
  unsigned short dir_size = memory[dir_ref].content.fd.size; 
  for (i = 0; i < dir_size; i++)
  { 
    file_descr_ref = memory[idx_block_ref].content.index[i];
    delete_file(file_descr_ref); 
  }
  open_block(idx_block_ref); 
  open_block(dir_ref); 
}

void delete_file(unsigned short dir_ref)
{
  unsigned short block_ref = memory[file_descr_ref].content.fd.block_ref;
  if (memory[block_ref].type == DAT_TYPE) 
    open_block(block_ref);

  else if (memory[block_ref].type == IDX_TYPE) 
  {
    int i;
    unsigned short file_size = memory[file_descr_ref].content.fd.size;
    for (i = 0; i < file_size / DATA_SIZE; i++) 
    {
      open_block(memory[block_ref].content.index[i]); 
    }
    open_block(block_ref); 
  }
  open_block(file_descr_ref); 
}

void file_info(char *name)
{
  int i;
  for(i = 0; i < NUM_BLOCKS; i++)
  {
    if(strcmp(memory[i].content.fd.name,name) == 0)
    {
      printf("%s\n",memory[i].content.fd.name);
      printf("%hi\n",memory[i].content.fd.block_ref);
    }
  }
}

int main()
{
  create_system();
  create_file(get_block_ref("/",DIR_TYPE),"hello");
  file_info("hello");
  return 0;
}


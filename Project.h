//Forrest Tappan
//Project 1 


#define DIR_SIZE 65551
#define NUM_OF_BLOCKS 65536
#define BLOCK_SIZE 256
#define BITOP(a,b,op) \
 ((a)[(size_t)(b)/(8*sizeof *(a))] op ((size_t)1<<((size_t)(b)%(8*sizeof *(a)))))

typedef enum {INDEX = 0, DATA, META} BLOCK_TYPE;

typedef struct inode
{
    char *name;
    int size;
    char *accessRights;
    int dataIndex;
}INODE;


typedef struct block
{
    BLOCK_TYPE type;
    union
    {
        INODE *inode;
        int indexBlock[BLOCK_SIZE / sizeof(int) - sizeof(BLOCK_TYPE)];
        char data[BLOCK_SIZE - sizeof(BLOCK_TYPE)];
    }content;
}BLOCK;

typedef struct systemTable
{
    INODE entries[DIR_SIZE];
    int count;
}SYSTEM_TABLE;

typedef struct processEntry
{
    INODE *systemTableEntry;
    int systemTableIndex;
    int processId;
    char *access;
}PROCESS_ENTRY;

typedef struct processTable
{
    PROCESS_ENTRY *entries[DIR_SIZE];
    int processId;
    int count;
}PROCESS_TABLE;


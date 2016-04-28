//Forrest Tappan
//Project1
//27-Apr-2016

#define DIR_SIZE 65551
#define NUM_OF_BLOCKS 65536
#define BLOCK_SIZE 256
#define BITOP(a,b,op) \
 ((a)[(size_t)(b)/(8*sizeof *(a))] op ((size_t)1<<((size_t)(b)%(8*sizeof *(a)))))

typedef enum {INDEX = 0, DATA} BLOCK_TYPE;

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
//void create_file();
//void delete_file(char *);



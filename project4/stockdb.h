#define MAXARGS 128

#define SUCCESS 1
#define FAIL -1

#define DONE 2

#define NOINPUT -5
#define NOTDONE -2
#define EXIT -3

#define SBUFSIZE 20
#define MAXTHREAD 100
#include "csapp.h"


typedef struct _item{
    int ID;
    int left_stock;
    int price;
    int readcnt;
    sem_t mutex;
    struct _item *lchild;
    struct _item *rchild;
}item;

typedef struct _sbuf_t{
    int *buf;
    int slot_max;
    int front;
    int rear;
    sem_t mutex;
    sem_t slots;
    sem_t items;
}sbuf_t;

typedef struct _itemTree{
    item* tree_ptr;
    unsigned long item_tree_cnt;
}itemTree;


extern itemTree stocktree;

sbuf_t sbuf;
static int byte_cnt;
static sem_t mutex;
/*
item* alloc_item();
void free_item();
void free_item_pool();
*/
int insert(int ID, int number_of_item,int price);
item* search_insert_pos(item* root,int ID,int* status);
//int delete_item(int ID, int number_of_del);
int do_update_stock_table(int connfd);
int parseline(char *buf, char **argv);

int cmdfunc(int argc,char* argv[],char output[]);
item* search_item(item* root,int ID);
int buy(int ID,int num_of_buy);
int sell(int ID,int num_of_sell);
void show_inorder(item* cur_node,char output[]);

void writedbtxt(item* root);
void write_inorder(item* cur_node,FILE* fp);

void sbuf_init(sbuf_t *shared_buf_ptr, int n);
void sbuf_deinit(sbuf_t *shared_buf_ptr);
void sbuf_insert(sbuf_t *shared_buf_ptr, int item);
int sbuf_remove(sbuf_t *shared_buf_ptr);

static void init_echo_cnt(void){
    Sem_init(&mutex,0,1);
    byte_cnt=0;
};
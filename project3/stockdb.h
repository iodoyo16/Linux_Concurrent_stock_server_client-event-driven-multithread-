#define MAXARGS 128

#define SUCCESS 1
#define FAIL -1

#define DONE 2
#define NOTDONE -2
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

/*
typedef struct _itemPool{
    item* pool_ptr;
    unsigned long item_pool_cnt;
    unsigned long item_alloc_cnt;
}itemTree;
*/
typedef struct _itemTree{
    item* tree_ptr;
    unsigned long item_tree_cnt;
}itemTree;

extern itemTree stocktree;

item* alloc_item();
void free_item();
void free_item_pool();
int insert(int ID, int number_of_item,int price);
item* search_insert_pos(item* root,int ID,int* status);
int delete_item(int ID, int number_of_del);
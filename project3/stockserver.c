/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "stockdb.h"

itemTree stocktree={NULL,0};
void echo(int connfd);
void initDB();
void inorder(item* cur_node);

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }
    initDB(&stocktree.tree_ptr);
    //inorder(stocktree.tree_ptr);
    listenfd = Open_listenfd(argv[1]);
    while (1) {
	clientlen = sizeof(struct sockaddr_storage); 
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, 
                    client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
	echo(connfd);
	Close(connfd);
    }
    exit(0);
}
void initDB(){
    FILE* fp;
    int id,stocknum,price;
    fp=Fopen("stock.txt","r");
    while(EOF!=fscanf(fp,"%d %d %d",&id,&stocknum,&price)){
        int status;
        if(feof(fp))
            break;
        status=insert(id,stocknum,price);
        if(status==SUCCESS)
            printf("insert success\n");
        else
            printf("insert fail!\n");
    }
}
void inorder(item* cur_node){
    if(cur_node==NULL)
         return;
    inorder(cur_node->lchild);
    printf("id: %d left_stock: %d price: %d\n",cur_node->ID,cur_node->left_stock,cur_node->price);
    inorder(cur_node->rchild);
}
/*
item* alloc_item(){
    item* current_item;
    printf("alloc item\n");
    if(itemPool.pool_ptr==NULL){
        current_item=(item*)Malloc(sizeof(item));
        itemPool.item_alloc_cnt++;
        current_item->lchild=current_item->rchild=NULL;
    }
    else{
        current_item=itemTree.item_pool;
        itemTree.tree_ptr=current_item->rchild;
        itemPool.item_pool_cnt--;
    }
    current_item->rchild=current_item->lchild=NULL;
    itemTree.item_tree_cnt++;
    printf("alloc cnt: %d\n",itemPool.item_alloc_cnt);
    printf("pool cnt: %d\n",itemPool.item_pool_cnt);
    printf("tree cnt: %d\n",itemTree.item_tree_cnt);
    return(current_item);
}
void free_item(item* del_item){
    printf("return item to pool\n");
    if(del_item==NULL)
        unix_error("This item is already freed");
    del_item->ID=-1;
    del_item->rchild=itemPool.pool_ptr;
    itemPool.pool_ptr=del_item;
    itemPool.item_pool_cnt++;
    itemTree.item_tree_cnt--;
}
void free_item_pool(){
    item* cur_item=itemPool.pool_ptr;
    printf("free item pool\n");
    while(cur_item!=NULL){
        itemPool.pool_ptr=cur_item->rchild;
        free(cur_item);
        itemPool.item_alloc_cnt--;
        itemPool.item_pool_cnt--;
        printf("alloc cnt: %d\n",itemPool.item_alloc_cnt);
        printf("pool cnt: %d\n",itemPool.item_pool_cnt);
        printf("tree cnt: %d\n",itemTree.item_tree_cnt);
    }
}*/


/* $end echoserverimain */



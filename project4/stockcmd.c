/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "stockdb.h"

static sem_t mutex;
static int byte_cnt;
static void init_echo_cnt(void){
    Sem_init(&mutex,0,1);
    byte_cnt=0;
};
void sbuf_init(sbuf_t *shared_buf_ptr, int slot_max){
    shared_buf_ptr->buf=Calloc(slot_max,sizeof(int));
    shared_buf_ptr->slot_max=slot_max;
    shared_buf_ptr->front=shared_buf_ptr->rear=0;
    Sem_init(&shared_buf_ptr->mutex,0,1);
    Sem_init(&shared_buf_ptr->slots,0,slot_max);
    Sem_init(&shared_buf_ptr->items,0,0);
}
void sbuf_deinit(sbuf_t *shared_buf_ptr){
    Free(shared_buf_ptr->buf);
}
void sbuf_insert(sbuf_t *shared_buf_ptr, int item){
    P(&(shared_buf_ptr->slots));
    P(&(shared_buf_ptr->mutex));
    shared_buf_ptr->buf[(++(shared_buf_ptr->rear))%(shared_buf_ptr->slot_max)]=item;
    V(&(shared_buf_ptr->mutex));
    V(&(shared_buf_ptr->items));
}
int sbuf_remove(sbuf_t *shared_buf_ptr){
    int item;
    P(&(shared_buf_ptr->items));
    P(&(shared_buf_ptr->mutex));
    item=shared_buf_ptr->buf[(++shared_buf_ptr->front)%(shared_buf_ptr->slot_max)];
    V(&shared_buf_ptr->mutex);
    V(&shared_buf_ptr->slots);
    return item;
}

void echo_cnt(int connfd) 
{
    int n;
    char input[MAXLINE]; 
    rio_t rio;
    static pthread_once_t once= PTHREAD_ONCE_INIT;
    Pthread_once(&once, init_echo_cnt);
    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, input, MAXLINE)) != 0) {
        int rn=1;
        char buf[MAXLINE];
        char* argv[10];
        char output[MAXLINE];
        int exit_flag=0,argc=0,ret=SUCCESS;
        memset(output,0,MAXLINE);
        if(n==0)
            break;
        strcpy(buf,input);
        argc=parseline(buf,argv);
        if(argc==1){
            if(!strcmp(argv[0],"show")){
                show_inorder(stocktree.tree_ptr,output);
            }
            else if(!strcmp(argv[0],"exit"))
                exit_flag=1;
        }
        else if(argc==3){
            int ID=atoi(argv[1]);
            int num_of_deal=atoi(argv[2]);
            if(!strcmp(argv[0],"buy"))
                ret=buy(ID,num_of_deal);
            else if(!strcmp(argv[0],"sell"))
                ret=sell(ID,num_of_deal);
            if(ret==SUCCESS)
                sprintf(output,"[%s] success",argv[0]);
            else
                strcpy(output,"Not enough left stock");
        }
        strcat(output,"\n");
        P(&mutex);
        byte_cnt +=n;
	    printf("thread %d received %d (%d total) bytes on fd %d\n", (int)pthread_self(),n,byte_cnt,connfd);
        V(&mutex);
        rn=strlen(output);
	    Rio_writen(connfd, output, rn);
        if(exit_flag==1)
            return;
    }
}
/* $end echo */

int sell(int ID,int num_of_sell){
    item* ptr=search_item(stocktree.tree_ptr,ID);
    if(ptr==NULL)
        return FAIL;
    else{
        P(&(ptr->wmutex));
        ptr->left_stock+=num_of_sell;
        V(&(ptr->wmutex));
        return SUCCESS;
    }
}
int buy(int ID,int num_of_buy){
    item* ptr=search_item(stocktree.tree_ptr,ID);
    int ret;
    if(ptr==NULL){
        printf("no item\n");
        return FAIL;
    }
    else{
        P(&(ptr->mutex));// READ_CNT LOCK
        (ptr->readcnt)++;
        if(ptr->readcnt==1)// FIRST IN
            P(&(ptr->wmutex));
        V(&(ptr->mutex));// READ_CNT UNLOCK

        // read part
        int avail_flag=(ptr->left_stock)<num_of_buy?-1:1;   
        // end read

        P(&(ptr->mutex));// READ_CNT LOCK
        (ptr->readcnt)--;
        if(ptr->readcnt==0)
            V(&(ptr->wmutex));// LAST OUT
        V(&(ptr->mutex));// 

        if(avail_flag==1){
            P(&(ptr->wmutex));
            ptr->left_stock-=num_of_buy;
            V(&(ptr->wmutex));
            ret=SUCCESS;
        }
        else
            ret=FAIL;
    }
    return ret;
}
item* search_item(item* root,int ID){
    item* temp=root;
    if(root==NULL)
        return NULL;
    while(temp){
        if(ID<(temp->ID))
            temp=temp->lchild;
        else if(ID>(temp->ID))
            temp=temp->rchild;
        else
            return temp;
    }
    return NULL;
}

void show_inorder(item* cur_node, char output[]){
    char buf[MAXLINE];
    if(cur_node==NULL)
         return;
    show_inorder(cur_node->lchild, output);
    P(&(cur_node->mutex));  // READ CNT LOCK
    (cur_node->readcnt)++;
    if(cur_node->readcnt==1)
        P(&(cur_node->wmutex)); // WRITE LOCK
    V(&(cur_node->mutex)); // READ CNT UNLOCK

    // READ PART
    sprintf(buf,"%d %d %d",cur_node->ID,cur_node->left_stock,cur_node->price);
    // READ PART END

    P(&(cur_node->mutex));  // READ CNT LOCK
    (cur_node->readcnt)--;
    if(cur_node->readcnt==0)
        V(&(cur_node->wmutex)); // WRITE UNLOCK
    V(&(cur_node->mutex)); // READ CNT UNLOCK


    if(strlen(output)!=0)
        strcat(output," ");
    strcat(output,buf);
    show_inorder(cur_node->rchild, output);
}

int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	    buf++;
    /* Build the argv list */
    argc = 0;
    delim=strchr(buf,' ');
    while (delim!=NULL) {
	    argv[argc++] = buf;
	    *delim = '\0';
	    buf = delim + 1;
	    while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
        delim=strchr(buf,' ');
    }
    argv[argc] = NULL;
    
    //if (argc == 0)  /* Ignore blank line */
	return argc;
}
/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#define MAXCONNECT 100
#include "stockdb.h"

static sem_t clientcnt_mutex;
itemTree stocktree={NULL,0};
volatile int active_client_cnt=0;

void echo_cnt(int connfd);
void initDB();
void inorder(item* cur_node);


int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];
    pthread_t tid;

    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }

    initDB();
    listenfd = Open_listenfd(argv[1]);
    sbuf_init(&sbuf,SBUFSIZE);
    Sem_init(&clientcnt_mutex,0,1);
    for(int i=0;i<MAXTHREAD;i++){
        Pthread_create(&tid,NULL,thread,NULL);
    }
    while (1) {
	    clientlen = sizeof(struct sockaddr_storage); 
	    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, 
                    client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        P(&clientcnt_mutex);
        active_client_cnt++;
        V(&clientcnt_mutex);
        sbuf_insert(&sbuf,connfd);
    }
    exit(0);
}
void *thread(void *vargp){
    Pthread_detach(pthread_self());
    while(1){
        int connfd = sbuf_remove(&sbuf);
        echo_cnt(connfd);
        Close(connfd);
        P(&clientcnt_mutex);
        active_client_cnt--;
        printf("client: %d\n",active_client_cnt);
        V(&clientcnt_mutex);
    }
}

void initDB(){
    FILE* fp;
    int id,stocknum,price;
    fp=Fopen("stock.txt","r");
    while(EOF!=fscanf(fp,"%d %d %d",&id,&stocknum,&price)){
        //int status;
        if(feof(fp))
            break;
        insert(id,stocknum,price);
    }
    Fclose(fp);
    return;
}
void inorder(item* cur_node){
    if(cur_node==NULL)
         return;
    inorder(cur_node->lchild);
    //printf("id: %d left_stock: %d price: %d\n",cur_node->ID,cur_node->left_stock,cur_node->price);
    inorder(cur_node->rchild);
}

/* $end echoserverimain */



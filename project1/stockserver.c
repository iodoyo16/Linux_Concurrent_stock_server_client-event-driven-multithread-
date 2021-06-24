/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#define MAXCONNECT 100
#include "stockdb.h"

itemTree stocktree={NULL,0};
void echo(int connfd);
void initDB();
void inorder(item* cur_node);


int main(int argc, char **argv) 
{
    int listenfd, connfd,maxfdnum, status,active_client_num=0;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }

    initDB(&stocktree.tree_ptr);

    listenfd = Open_listenfd(argv[1]);
    clientlen = sizeof(struct sockaddr_storage);
    maxfdnum=listenfd;
    fd_set readfds,preserve_readfds;
    FD_ZERO(&readfds);
    FD_ZERO(&preserve_readfds);
    FD_SET(listenfd,&preserve_readfds);
    while (1) {
        readfds=preserve_readfds;
        Select(maxfdnum+1,&readfds,NULL,NULL,NULL);
        for(int i_fd=0;i_fd<=maxfdnum;i_fd++){
            if(FD_ISSET(i_fd,&readfds)){
                if(i_fd==listenfd){
                    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                    Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
                    printf("Connected to (%s, %s)\n", client_hostname, client_port);
                    active_client_num++;
                    FD_SET(connfd,&preserve_readfds);
                    if(connfd>maxfdnum)
                        maxfdnum=connfd;
                }
                else{
                    status=do_update_stock_table(i_fd);
                    if(status==EXIT){
                        FD_CLR(i_fd,&preserve_readfds);
                        Close(i_fd);
                        active_client_num--;
                        if(active_client_num==0){
                            writedbtxt(stocktree.tree_ptr);
						}
                    }
                }
            }
        }
    }
    exit(0);
}


void initDB(){
    FILE* fp;
    int id,stocknum,price;
    fp=Fopen("stock.txt","r");
    while(EOF!=fscanf(fp,"%d %d %d",&id,&stocknum,&price)){
        insert(id,stocknum,price);
    }
    Fclose(fp);
    return;
}
void inorder(item* cur_node){
    if(cur_node==NULL)
         return;
    inorder(cur_node->lchild);
    inorder(cur_node->rchild);
}

/* $end echoserverimain */



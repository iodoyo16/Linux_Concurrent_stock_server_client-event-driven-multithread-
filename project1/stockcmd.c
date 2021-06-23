/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "stockdb.h"

int do_update_stock_table(int connfd) 
{
    int n,ret,rn=1;
    int argc;
    char input[MAXLINE];
    char buf[MAXLINE];
    char output[MAXLINE];
    char*argv[10];
    rio_t rio;
    memset(output,0,MAXLINE);
    Rio_readinitb(&rio, connfd);
    n = Rio_readlineb(&rio, input, MAXLINE);
    strcpy(buf,input);
    argc=parseline(buf,argv);
    if(n==0)
        return EXIT;
	printf("server received %d bytes\n", n);

    ret=cmdfunc(argc,argv,output);
    rn=strlen(output);
    //printf("%s rn:%d\n",output,rn);
    
    Rio_writen(connfd, output, rn);
    return ret;
}
/* $end echo */
int cmdfunc(int argc,char* argv[],char output[]){
    int ret;
    if(argc==1){
        if (!strcmp(argv[0],"show")){
            show_inorder(stocktree.tree_ptr,output);
            ret=DONE;
        }
        else if(!strcmp(argv[0],"exit"))
            ret=EXIT;
        else
            ret=DONE;
    }
    else if(argc==3){
        int ID=atoi(argv[1]);
        int number_of_deal=atoi(argv[2]);
        if(!strcmp(argv[0],"buy"))
            ret=buy(ID,number_of_deal);
        else if(!strcmp(argv[0],"sell"))
            ret=sell(ID,number_of_deal);
        else
            ret=DONE;
        if(ret==SUCCESS)
            sprintf(output,"[%s] success",argv[0]);
        else
            strcpy(output,"Not enough left stock");
    }
    else
        ret=DONE;
    strcat(output,"\n");
    return ret;
}
int sell(int ID,int num_of_sell){
    item* ptr=search_item(stocktree.tree_ptr,ID);
    if(ptr==NULL)
        return FAIL;
    else{
        ptr->left_stock+=num_of_sell;
        return SUCCESS;
    }
}
int buy(int ID,int num_of_buy){
    item* ptr=search_item(stocktree.tree_ptr,ID);
    if(ptr==NULL){
        printf("no item\n");
        return FAIL;
    }
    else{
        if((ptr->left_stock)<num_of_buy)
            return FAIL;
        ptr->left_stock-=num_of_buy;
        return SUCCESS;
    }
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
    sprintf(buf,"%d %d %d",cur_node->ID,cur_node->left_stock,cur_node->price);
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

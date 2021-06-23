
#include "stockdb.h"

int insert(int ID, int number_of_item, int price){
    int status;
    item* ptr=search_insert_pos(stocktree.tree_ptr,ID,&status);
    if(stocktree.tree_ptr==NULL){
        item* newnode=(item*)Malloc(sizeof(item));
        newnode->ID=ID;
        newnode->lchild=newnode->rchild=NULL;
        newnode->left_stock=number_of_item;
        newnode->price=price;
        newnode->readcnt=0;
        stocktree.tree_ptr=newnode;
        stocktree.item_tree_cnt++;
        
        Sem_init(&(newnode->mutex),0,1);
        Sem_init(&(newnode->wmutex),0,1);
        //printf("root node id %d lft %d price%d\n",newnode->ID,newnode->left_stock,newnode->price);
        return SUCCESS;
    }
    else if(ptr==NULL){
        // incremented left stock
        if(status==DONE)
            return SUCCESS;
        else{
            //printf("search problem");
            return FAIL;
        }
    }
    else {
        item* newnode=(item*)Malloc(sizeof(item));
        //if(newnode==NULL)
        //    unix_error("something wrong with storage pool");
        newnode->ID=ID;
        newnode->lchild=newnode->rchild=NULL;
        newnode->left_stock=number_of_item;
        newnode->price=price;
        newnode->readcnt=0;
        Sem_init(&(newnode->mutex),0,1);
        Sem_init(&(newnode->wmutex),0,1);
        if(ID<(ptr->ID))
            ptr->lchild=newnode;
        else ptr->rchild=newnode;
        stocktree.item_tree_cnt++;
        //printf("new node id %d lft %d price%d\n",newnode->ID,newnode->left_stock,newnode->price);
        return SUCCESS;
    }
}

item* search_insert_pos(item* root,int ID,int* status){
    item* temp=root;
    while(temp){
        if(ID==(temp->ID)){
            temp->left_stock++;
            *status=DONE;
            return NULL;
        }
        if(ID<(temp->ID)){
            if(temp->lchild==NULL)return temp;
            else temp=temp->lchild;
        }
        else{
            if(temp->rchild==NULL)return temp;
            else temp=temp->rchild;
        }
    }
    *status=NOTDONE;
    return NULL;
}
void writedbtxt(item* root){
    FILE* fp;
    fp=Fopen("stock.txt","w");
    write_inorder(root,fp);
    Fclose(fp);
}
void write_inorder(item* cur_node, FILE* fp){
    if(cur_node==NULL)
         return;
    write_inorder(cur_node->lchild, fp);
    fprintf(fp,"%d %d %d\n",cur_node->ID,cur_node->left_stock,cur_node->price);
    write_inorder(cur_node->rchild, fp);
}
/*
int delete_item(int ID, int number_of_del){
    item* temp=stocktree.tree_ptr;
    while(temp){
        if(ID==temp->ID){
            if((temp->left_stock)<number_of_del)
                return FAIL;
            temp->left_stock-=number_of_del;
            return SUCCESS;
        }
        else if(ID< temp->ID)
            temp=temp->lchild;
        else
            temp=temp->rchild;
    }
    printf("there is no such stock\n");
    return FAIL;
}
*/
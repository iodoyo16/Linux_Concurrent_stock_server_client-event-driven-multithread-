
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
        stocktree.tree_ptr=newnode;
        stocktree.item_tree_cnt++;
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
        newnode->ID=ID;
        newnode->lchild=newnode->rchild=NULL;
        newnode->left_stock=number_of_item;
        newnode->price=price;
        if(ID<(ptr->ID))
            ptr->lchild=newnode;
        else ptr->rchild=newnode;
        stocktree.item_tree_cnt++;
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
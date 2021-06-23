/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "stockdb.h"

void manage_stock_request(int connfd) 
{
    int n;
    char input[MAXLINE];
    char buf[MAXLINE]; 
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    //while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    n = Rio_readlineb(&rio, buf, MAXLINE);
	printf("server received %d bytes\n", n);
	Rio_writen(connfd, buf, n);
}
/* $end echo */


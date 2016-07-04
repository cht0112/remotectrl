#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "remotectrl.h"
#include "remotehash.h"

struct st_remote g_remote_manager;

int pad_mac(char *buf,const char *strmac)
{
        int i=0,val=0;
        const char *p;
        p=strmac;
        while(*p!=0){
                if(*p=='-'){
                        i++;
                        p++;
                        continue;
                }
                if(isdigit(*p))
                        val=*p-'0';
                else
                        val=toupper(*p)-'A'+10;
                buf[i]=(buf[i]<<4) | val;
                p++;
        }
	return i;
}

void setnonblock(int sock)
{
        int opts;
        opts=fcntl(sock,F_GETFL);
        if(opts<0)
        {
                perror("fcntl(sock,GETFL)");
                exit(1);
        }
        opts = opts|O_NONBLOCK;
        if(fcntl(sock,F_SETFL,opts)<0)
        {
                perror("fcntl(sock,SETFL,opts)");
                exit(1);
        }
}

int send_cmd(int cmd,const char *mac)
{
	struct st_node *pnode=NULL;
	struct st_cmd_head *phead;
	char buf[80];
	struct sockaddr_in addr;

	pnode=remote_hash_search(mac);
	if(pnode==NULL)
		return -1;
	printf("match node addr %lx port %d\n", pnode->m_addr,ntohs(pnode->m_port));
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = pnode->m_addr;
        addr.sin_port = pnode->m_port;
	phead=buf;
	phead->cmd=cmd;
	phead->len=6;
	memcpy(buf+sizeof(struct st_cmd_head),mac,6);
        sendto(g_remote_manager.udpsock,buf,sizeof(struct st_cmd_head)+6,0,( struct sockaddr *)&addr,sizeof(struct sockaddr));
	return 0;
	
}

int proc_packet(char *buf,int len,struct sockaddr_in *addr)
{
        int ret;
	char *p;
	struct st_node *pnode=NULL;
        struct st_cmd_head *phead;
        phead=(struct st_cmd_head *)buf;
        if(phead->cmd == CMD_PING){
		p=buf+sizeof(struct st_cmd_head);
		pnode=remote_hash_search(p);
		if(pnode!=NULL){
			pnode->m_addr=addr->sin_addr.s_addr;
			pnode->m_port=addr->sin_port;
		}else
		{
			pnode=malloc(sizeof(struct st_node));
			memset(pnode,0,sizeof(struct st_node));
			memcpy(pnode->m_mac,p,6);
			pnode->m_addr=addr->sin_addr.s_addr;
			pnode->m_port=addr->sin_port;
			remote_hash_add(pnode);
	printf("save node addr=%lx port=%d\n",pnode->m_addr,ntohs(pnode->m_port));
		}
		return 0;
	}
	return 0;
}

void *proc_post(void *p)
{
	int sock=(int)p;
	char *ptr1,*ptr2,mac[6],tmpbuf[80],*buf;
	int cmd,len,ret;
	buf=g_remote_manager.recvbuf;
	len=read(sock,buf,1024);
	buf[len]=0;
	ptr1=strstr(buf,"\r\n\r\n");
	if(ptr1==NULL)
		goto err;
	ptr1+=4;
	ptr2=strcasestr(ptr1,"cmd=");
	if(ptr2==NULL)
		goto err;
	ptr2+=4;
	ptr1=strstr(ptr2,"&");
	if(ptr2==NULL)
		goto err;
	memset(tmpbuf,0,80);
	memcpy(tmpbuf,ptr2,ptr1-ptr2);
	cmd=atoi(tmpbuf);
	if(cmd==0)
		goto err;
	ptr1++;
	ptr2=strcasestr(ptr1,"mac=");
	if(ptr2==NULL)
		goto err;
	ptr2+=4;
	ptr1=strstr(ptr2,"&");
	if(ptr1==NULL)
		goto err;
	memset(tmpbuf,0,80);
	memcpy(tmpbuf,ptr2,ptr1-ptr2);
	ret=pad_mac(mac,tmpbuf);
	if(ret!=5)
		goto err;

	sprintf(buf,"HTTP/1.1 200 OK\r\nContent-Type: text\/html\r\n\r\nOK");
	write(sock,buf,strlen(buf));
	close(sock);
	send_cmd(cmd,mac);
	return NULL;
err:
	sprintf(buf,"HTTP/1.1 404\r\nContent-Type: text\/html\r\n\r\n");
	write(sock,buf,strlen(buf));
	close(sock);
	return NULL;
}


main(int argc,void **argv)
{
        int ret,maxfd;     
        fd_set rfd;
        struct timeval timeout;
        char buf[200];
        struct sockaddr_in src_addr;
        socklen_t addrlen=sizeof(struct sockaddr);

	g_remote_manager.udpsport=8192;
	g_remote_manager.tcpsport=8192;
        bzero(&src_addr, sizeof(src_addr));
        src_addr.sin_family = AF_INET;
        src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        src_addr.sin_port = htons(g_remote_manager.udpsport);
        g_remote_manager.udpsock = socket(AF_INET, SOCK_DGRAM, 0);
        if(g_remote_manager.udpsock == -1)
        {
        //      perror("Create Socket Failed:");
                exit(1);
        }
        if(-1 == (bind(g_remote_manager.udpsock,(struct sockaddr*)&src_addr,sizeof(src_addr))))
        {
                perror("Server Bind Failed:");
                exit(1);
        }
	setnonblock(g_remote_manager.udpsock);

        bzero(&src_addr, sizeof(src_addr));
        src_addr.sin_family = AF_INET;
        src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        src_addr.sin_port = htons(g_remote_manager.tcpsport);
        g_remote_manager.tcpsock = socket(AF_INET, SOCK_STREAM, 0);
        if(g_remote_manager.tcpsock == -1)
        {
        //      perror("Create Socket Failed:");
                exit(1);
        }
        if(-1 == (bind(g_remote_manager.tcpsock,(struct sockaddr*)&src_addr,sizeof(src_addr))))
        {
                perror("Server Bind Failed:");
                exit(1);
        }
	setnonblock(g_remote_manager.tcpsock);
	
	listen(g_remote_manager.tcpsock,3);

	while(1){
                FD_ZERO(&rfd);
                FD_SET(g_remote_manager.udpsock, &rfd);
		maxfd = g_remote_manager.udpsock;
                FD_SET(g_remote_manager.tcpsock, &rfd);
		if(g_remote_manager.tcpsock>maxfd)
			maxfd = g_remote_manager.tcpsock;
                timeout.tv_sec = 5;
                timeout.tv_usec = 0;
                ret=select(maxfd+1,&rfd,NULL,NULL,&timeout);
                if(ret<0){
                        perror("Server Bind Failed:");
                        exit(1);
                }else
                if(ret==0){
                        continue;
                }else
                {
			if(FD_ISSET(g_remote_manager.udpsock,&rfd)){
                        	int len=recvfrom(g_remote_manager.udpsock,buf,200,0,( struct sockaddr *)&src_addr,&addrlen);
				proc_packet(buf,len,&src_addr);
				continue;
			}
			if(FD_ISSET(g_remote_manager.tcpsock,&rfd)){
				int fd=accept(g_remote_manager.tcpsock,( struct sockaddr *)&src_addr,&addrlen);
				if(fd>=0){
					pthread_t pth;
					pthread_create(&pth,0,proc_post,(void *)fd);
				}
				continue;
			}
                }
        }
}


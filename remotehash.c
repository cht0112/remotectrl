#include <stdio.h>
#include <string.h>
#include "remotehash.h"

struct st_hash_table g_hash_table;
unsigned int remote_hash(unsigned char *c)
{
	return (c[2]+c[3]+c[4]+c[5])%MAX_HASH_BUCK_NUM;
}

int remote_hash_comp(struct st_node *p1,struct st_node *p2)
{
	if(memcmp(p1->m_mac,p2->m_mac,6)==0)
		return 0;
	return -1;
}

int remote_hash_add(struct st_node *p)
{
	unsigned int id=remote_hash(p->m_mac);
	struct st_node *head = g_hash_table.hash_array[id];

	if(head == NULL){
		g_hash_table.hash_array[id] = p;
	}else
	{
		p->pnext=g_hash_table.hash_array[id];
		g_hash_table.hash_array[id]=p;
	}
	g_hash_table.m_num++;
	return 0;
}

int remote_hash_del(struct st_node *node)
{
	unsigned int id=remote_hash(node->m_mac);
	struct st_node *p,*q,*head = g_hash_table.hash_array[id];
	q=head;
	if(q==NULL)
		return -1;
	if(remote_hash_comp(node,q)==0)
	{
		g_hash_table.hash_array[id]=q->pnext;
		free(q);
		g_hash_table.m_num--;
		return 0;
	}
	p=q;
	q=q->pnext;
	while(q!=NULL){
		if(remote_hash_comp(node,q)==0)
		{
			p->pnext=q->pnext;
			free(q);
			g_hash_table.m_num--;
			return 0;
		}
		else{
			p=q;
			q=q->pnext;
		}
	}
	return -1;
}

struct st_node * remote_hash_search(char *mac)
{
	unsigned int id=remote_hash(mac);
	struct st_node p,*q,*head = g_hash_table.hash_array[id];
	memcpy(p.m_mac,mac,6);
	q=head;
	while(q!=NULL){
		if(remote_hash_comp(&p,q)==0)
			return q;
		else
			q=q->pnext;
	}
	return NULL;
}

#if 0
main()
{
	char mac[6];
	int i,num=0;
	struct st_node *p,q;
	memset(&g_hash_table,0,sizeof(g_hash_table));
	while(1){
	num=0;
	for(i=0;i<10000;i++){
		mac[0]=0;
		mac[1]=0;
		mac[2]=0;
		mac[3]=random()%256;
		mac[4]=random()%256;
		mac[5]=random()%256;
//	printf("add node %02x%02x%02x \n",mac[3],mac[4],mac[5]);
		if(remote_hash_search(mac)!=NULL){
			;//printf("node already exist!\n");
		}else
		{
			p=malloc(sizeof(struct st_node));
			memcpy(p->m_mac,mac,6);
			p->pnext=NULL;
			remote_hash_add(p);
			num++;
		}
	} 
	printf("add %d node\n",num);
	num=0;
	for(i=0;i<10000;i++){
		mac[0]=0;
		mac[1]=0;
		mac[2]=0;
		mac[3]=random()%256;
		mac[4]=random()%256;
		mac[5]=random()%256;
		memcpy(q.m_mac,mac,6);
		if(remote_hash_del(&q)==0)
			num++;
	}
	printf("del %d node\n",num);
	sleep(10);
	}
	printf("hash node %d \n",g_hash_table.m_num);
	return 0;
		
}
#endif

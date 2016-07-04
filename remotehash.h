#ifndef REMOTEHSAH_H
#define REMOTEHSAH_H
#include <time.h>
#define MAX_HASH_BUCK_NUM 1024

struct st_node{
	struct st_node *pnext;
	unsigned char m_mac[6];
	unsigned char m_code[64];
	unsigned char m_key[20];
	unsigned long m_addr;
	unsigned short m_port;
	time_t m_stamp;
};

struct st_hash_table{
	int m_num;
	struct st_node *hash_array[MAX_HASH_BUCK_NUM];
};

struct st_node * remote_hash_search(char *mac);
int remote_hash_del(struct st_node *p);
int remote_hash_add(struct st_node *p);

#endif

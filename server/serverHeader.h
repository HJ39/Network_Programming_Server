#ifndef MINISOCKETSERVER_SERVER_H
#define MINISOCKETSERVER_SERVER_H

#include "config.h"

struct params {
    int sockFd;
    int id;
};

int occupied[MAX_CLIENT_NUM];
struct sockaddr_in sa_client_list[MAX_CLIENT_NUM]; // ����� �� Ŭ���̾�Ʈ�� ���� �ּ�
SOCKET s_client_list[MAX_CLIENT_NUM]; //����� �� Ŭ���̾�Ʈ�� ���� �ڵ鸵

void Sort(struct Info* base, int n);
int next_client_id(int* occ);
void* sub_thread(void* params);

#endif // �̴ϼ��ϼ���_server_���
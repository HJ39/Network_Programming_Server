#ifndef MINISOCKETSERVER_SERVER_H
#define MINISOCKETSERVER_SERVER_H

#include "config.h"

struct params {
    int sockFd;
    int id;
};

int occupied[MAX_CLIENT_NUM];
struct sockaddr_in sa_client_list[MAX_CLIENT_NUM]; // 연결된 각 클라이언트의 소켓 주소
SOCKET s_client_list[MAX_CLIENT_NUM]; //연결된 각 클라이언트의 소켓 핸들링

void Sort(struct Info* base, int n);
int next_client_id(int* occ);
void* sub_thread(void* params);

#endif // 미니소켓서버_server_헤더
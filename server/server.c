#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>
#include <pthread.h>
#include "config.h"
#include "serverHeader.h"


#define SERVER_IP "192.168.112.1" // IP 주소
#define PORT_NUM 9241 // 포트 번호
#define MAX_MSG_LEN 256		//메세지 최대 길이
#define WATING_CLIENT 3	//접속 허용 클라이언트 수
#define buf_size (MAX_CLIENT_NUM * 30)

struct Info{
	char nickName[10];
	int point;
};

struct Info info[100];
int infoCount = 0;

int main() {

	WSADATA wsaData;
	WORD wVersionRequested;
	SOCKET sListen;
	SOCKET sClient;
	struct sockaddr_in sa_server;


	wVersionRequested = MAKEWORD(2, 2); //Winsock 초기화
	if (WSAStartup(wVersionRequested, &wsaData) != 0) { //WSAStartup 성공시에는 0을, 실패시에는 -1을 반환	
		perror("Error: WSA start failed\n"); //오류시 WSA 시작 실패 출력
		exit(EXIT_FAILURE);  //비정상 종료
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		perror("Error: Invalid winsock version\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	memset(occupied, 0, sizeof(occupied));

	// Socket 생성
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {			//소켓 검사
		perror("Error: Create_socket failed\n"); //잘못된 소켓 일시 출력
		exit(EXIT_FAILURE); //비정상 종료
	}

	memset(&sa_server, 0, sizeof(sa_server));

	printf("Socket 초기화 완료\n");

	//bind 준비
	sa_server.sin_family = AF_INET;
	sa_server.sin_addr.s_addr = inet_addr(SERVER_IP);
	sa_server.sin_port = htons(PORT_NUM);

	if (bind(sListen, (struct sockaddr*)&sa_server, sizeof(sa_server)) == -1) {
		perror("Error: Bind failed\n"); //잘못된 소켓 일시 출력
		closesocket(sListen);
		WSACleanup();
		exit(EXIT_FAILURE); //비정상 종료
	}

	printf("바인드 완료\n");


	if (listen(sListen, WATING_CLIENT) == -1) {
		perror("Error: Listen faild!\n");
		closesocket(sListen); //소켓 닫기
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	printf("Start listening.\n");


	//accept
	
	int length = sizeof(sa_client_list[0]);
	while (TRUE) {
		int c_id = next_client_id(occupied);
		if (c_id == -1) {
			continue;
		}

		sClient = accept(sListen, (struct sockaddr*)&sa_client_list[c_id], &length);
		s_client_list[c_id] = sClient;
		if (sClient == INVALID_SOCKET) {
			perror("Error: Accept faild!\n");
			closesocket(sClient); //소켓 닫기
			continue;
		}
		printf("Server: New connection from ip:%s, port:%d\n", inet_ntoa(sa_client_list[c_id].sin_addr), ntohs(sa_client_list[c_id].sin_port));

		occupied[c_id] = 1;

		// 새로운 쓰레드 
		pthread_t p_id;
		// 매개변수 초기화
		struct params* p = (struct params*)malloc(sizeof(struct params));
		p->sockFd = sClient;
		p->id = c_id;
		if (pthread_create(&p_id, NULL, sub_thread, (void*)p) != 0)
		{
			perror("pthread create error.\n");
			closesocket(sClient); //소켓 닫기
			occupied[c_id] = 0;
			continue;
		}

	}
	return 0;

}


void* sub_thread(void* p) {
	struct params* pp = (struct params*)p;
	SOCKET sockFd = pp->sockFd;
	int id = pp->id;

	// hello 메세지 보내기
	char hello_msg[BUF_SIZE] = "\0";
	char buf[BUF_SIZE];
	memset(buf, 0, BUF_SIZE);

	//닉네임 수신
	int tail = recv(sockFd, buf, BUF_SIZE, 0);
	if (tail == -1) {
		printf("Client %d disconnected.\n", id);
		occupied[id] = 0;
		closesocket(sockFd);
	}

	char msg[buf_size];
	strcpy(msg, buf);
	strcat(msg, "\n");

	//닉네임 전송
	int res = send(sockFd, msg, strlen(msg), 0);
	if (res == -1) {
		perror("Error: Sending client list failed.\n");
	}

	printf("Sending hello msg to client.\n");

	while (TRUE) {
		
		memset(buf, 0, BUF_SIZE);

		//점수 받는 부분
		int tail = recv(sockFd, buf, BUF_SIZE, 0);
		if (tail == -1) {
			printf("Client %d disconnected.\n", id);
			occupied[id] = 0;
			closesocket(sockFd);
			break;
		}

		// print client request
		buf[tail] = '\0';
		printf("Client: %s\n", buf);

		char* ptr = strtok(buf, "#");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환

		strcpy(info[infoCount].nickName,ptr);
		ptr = strtok(NULL, "#");      // 다음 문자열을 잘라서 포인터를 반환
		info[infoCount++].point = atoi(ptr);
		
		
		Sort(info, sizeof(info) / sizeof(struct Info));

		printf("%s\n",info[0].nickName);
		printf("%d\n",info[0].point);
		printf("%d\n", infoCount);
		//send 준비
		char msg[buf_size];
		char grade[10];
		strcpy(msg, "\n");
		strcat(msg, "\0");
		
		strcpy(msg, info[0].nickName);
		strcat(msg, "#");
		sprintf(grade, "%d", info[0].point);
		strcat(msg, grade);

		for (int i = 1; i < infoCount; i++) {
			strcat(msg, "|");
			strcat(msg, info[i].nickName);
			strcat(msg, "#");
			sprintf(grade, "%d", info[i].point);
			strcat(msg, grade);
		}
	
		strcat(msg, "\n");
		printf("sending Client: %s\n", msg);
	
		int res = send(sockFd, msg, strlen(msg), 0);
		if (res == -1) {
			perror("Error: Sending client list failed.\n");
		}
	}
}

void Sort(struct Info* base, int n){
	struct Info temp;
	int i = 0, j = 0;
	for (i = n; i > 1; i--){
		for (j = 1; j < i; j++) {

			if (base[j - 1].point < base[j].point){
				temp = base[j - 1];
				base[j - 1] = base[j];
				base[j] = temp;
			}
		}
	}

}

int next_client_id(int* occ) {
	int i;
	int res = -1;
	for (i = 0; i < MAX_CLIENT_NUM; i++) {
		if (occ[i] == 0) {
			res = i;
			break;
		}
	}
	return res;
}
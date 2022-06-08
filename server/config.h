#ifndef __CONFIG_H__
#define __CONFIG_H__

#define BUF_SIZE 1024
#define SERVER_PORT 5618
#define WAIT_LIST_LEN 5
#define MAX_CLIENT_NUM 3

enum
{
	GET_TIME,
	GET_NAME,
	GET_CONN_LIST,
	SEND_MSG,
	PRINT_MSG,
	EXIT
};

#endif
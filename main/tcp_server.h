#ifndef __tcp_server_h
#define __tcp_server_h

#define TCP_SERVER_RESTRICTED 0
#define TCP_SERVER_CONFIG 1

void start_tcp_server();
void tcp_server_init(uint8_t mode);

#endif

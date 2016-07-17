#ifndef CLIENT_HPP_INCLUDED
#define CLIENT_HPP_INCLUDED

#define CLIENTCOM_FIRST_COMMAND 'A'
#define CLIENTCOM_LOGIN         CLIENTCOM_FIRST_COMMAND
#define CLIENTCOM_JOIN_ANY       'B'//if we previously agreed to play with someone, join that room
#define CLIENTCOM_SET_STATE     'C'
#define CLIENTCOM_SET_FLAG      'D'
#define CLIENTCOM_UNSET_FLAG    'E'
#define CLIENTCOM_ROOM_DETAILS  'F'
#define CLIENTCOM_PLAYER_DETAIL 'G'
#define CLIENTCOM_MSG_MASK      'H'//a mask that determines where our sends go to
#define CLIENTCOM_GAMEPLAY_DATA 'W'

#define CLIENTCOM_BAD_COMMAND   'X'
#define CLIENTCOM_QUIT          'Z'
#define CLIENTCOM_LAST_COMMAND  CLIENTCOM_QUIT

#include "users.hpp"

class SystemEntry;

class Client{
public:
    Client(){next=prev=NULL;system_base=NULL;user_ent=NULL;}
    ~Client();
    SOCKADDR_IN addr;
    int sock;
    int proto;//whether socket is TCP or UDP
    int latency;
    UserEntry *user_ent;
    Client *next;
    Client *prev;
    SystemEntry *system_base;

    int msg_mask;
    unsigned char packet_buf[2048];
    unsigned char com_buf[2048];
    unsigned char packet_out[2048];//the packet we are currently building, and will send out when flushed
    unsigned int pbuf_out_pos;
    unsigned int pbuf_pos;
    unsigned int cbuf_pos;
    unsigned int com_bytes;
    unsigned int data_bytes;
    unsigned int command_state;
    Client *Add(SystemEntry *base);
    void Del();
    int SendString(char *s);
    int SendChar(char c);
    void PrintIP(SOCKADDR_IN *sock_address);
    void Disco();
    void Disco(char *msg);
    int NetSend();
};


#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#ifdef _WIN32
    #include <winsock.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
    #include <unistd.h> /* Needed for close() */
#endif // _WIN32

#include "client.hpp"

void Client::PrintIP(SOCKADDR_IN *sock_address){
   printf("%d.%d.%d.%d:%d",sock_address->sin_addr.S_un.S_un_b.s_b1,sock_address->sin_addr.S_un.S_un_b.s_b2,sock_address->sin_addr.S_un.S_un_b.s_b3,sock_address->sin_addr.S_un.S_un_b.s_b4,sock_address->sin_port);
}

void Client::Disco(){
    PrintTime();
    if(user_ent == NULL){
        printf(":Unknown user(no login):");
        PrintIP(&addr);
        printf(" disconnected.\n");
    }else
        printf(":%s(%s) disconnected.\n",user_ent->name,user_ent->realname);


    closesocket(sock);
    sock = -1;
}

void Client::Disco(char *msg){
    SendString(msg);
    closesocket(sock);
    sock = -1;
}

int Client::SendString(char *s){

    return 0;
}

int Client::SendChar(char c){

    return 0;
}

int Client::NetSend(){

    return 0;
}

Client *Client::Add(SystemEntry *base){
    if(next != NULL)
        return next->Add(base);

    next = new Client;
    next->prev = this;
    next->system_base = base;
    next->command_state = CLIENTCOM_LOGIN;
    return next;
}


#endif // CLIENT_HPP_INCLUDED

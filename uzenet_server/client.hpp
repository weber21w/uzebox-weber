#ifndef CLIENT_HPP_INCLUDED
#define CLIENT_HPP_INCLUDED

#define CLIENTCOM_FIRST_COMMAND 'A'
#define CLIENTCOM_LOGIN         CLIENTCOM_FIRST_COMMAND
#define CLIENTCOM_JOIN_ANY      'B'//if we previously agreed to play with someone, join that room
#define CLIENTCOM_SET_STATE     'C'
#define CLIENTCOM_SET_FLAG      'D'
#define CLIENTCOM_UNSET_FLAG    'E'
#define CLIENTCOM_ROOM_DETAILS  'F'
#define CLIENTCOM_PLAYER_DETAIL 'G'
#define CLIENTCOM_MSG_MASK      'H'//a mask that determines where our sends go to
#define CLIENTCOM_SET_GAME      'I'
#define CLIENTCOM_REQUEST_BOT   'J'
#define CLIENTCOM_GET_TIME      'K'
#define CLIENTCOM_ECHO          'L'//echo the next length specified bytes, so the client knows connection is up and it didn't miss an error message somewhere
#define CLIENTCOM_GET_USER_NO   'M'//what user number are we(0-8), can be used to determine role/authority of clients
#define CLIENTCOM_READY_CUE     'N'//request "++" when specified number of players are ready
#define CLIENTCOM_GET_NAME      'O'
#define CLIENTCOM_SET_READY     'R'
#define CLIENTCOM_GAMEPLAY_DATA 'W'

#define CLIENTCOM_BAD_COMMAND   'X'
#define CLIENTCOM_QUIT          'Z'
#define CLIENTCOM_LAST_COMMAND  CLIENTCOM_QUIT


//CLIENTCOM_ROOM_DETAILS
#define ROOM_DETAIL_NUM_PLAYERS 'A'

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
    char current_game[16+1];
    unsigned int pbuf_out_pos;
    unsigned int pbuf_pos;
    unsigned int cbuf_pos;
    unsigned int com_bytes;
    unsigned int data_bytes;
    unsigned int command_state;
    uint8_t     notify_on_ready;
    bool ready;//signifies the player or program is ready to proceed communicating with something, somewhere else
    //Client *Add(SystemEntry *base);//done in system class
    void Del();
    int SendString(char *s);
    int SendChar(char c);
    void PrintIP(SOCKADDR_IN *sock_address);
    void Disco();
    void Disco(char *msg);
    int NetSend();
    void RemoveCommandBytes(int num);
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
    }else{
        printf(":%s(%s):",user_ent->name,user_ent->realname);
        PrintIP(&addr);
        printf(" disconnected.\n");
    }


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

void Client::RemoveCommandBytes(int num){
    int total = com_bytes-num;
    if(total <= 0){
     //   printf("ERROR:RemoveCommandBytes() <= 0\n");
       // return;
    }
    int i;
    for(i=0;i<total;i++)
        com_buf[i] = com_buf[i+num];
    com_bytes -= num;
    com_buf[com_bytes] = '\0';

}


#endif // CLIENT_HPP_INCLUDED

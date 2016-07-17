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



void Client::Disco(){
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

int Client::Update(){

    if(system_base->critical_err)
        return system_base->critical_err;

    int32_t bytes;
    if((bytes = recv(sock,(char *)packet_buf,sizeof(packet_buf),0)) == SOCKET_ERROR){
        if(WSAGetLastError() != WSAEWOULDBLOCK){//critical error
            Disco();
            return 1;
        }
    }

    data_bytes += bytes;

    if(com_bytes+data_bytes < sizeof(com_buf)){//copy packet data to command buffer if it will ALL fit
        memcpy(com_buf+com_bytes,packet_buf,bytes);
        com_bytes += data_bytes;
        data_bytes = 0;
    }

    if(!com_bytes)//anything to process?
        return 0;

//PACKET_PROCESS_TOP:

    if(command_state == 0){//next byte should be a command
        command_state = com_buf[pbuf_pos++];//get a new command, we will only process it if the required arguments are completely here, else buffer and wait
        data_bytes--;
        if(command_state < CLIENTCOM_FIRST_COMMAND || command_state > CLIENTCOM_LAST_COMMAND){//bad command, client probably missed a UART bytes or logic failed
            Disco((char *)"ZZZZZZZZ");
            return 1;
        }
    }


    int32_t t;
    char buf[128];
    while(com_bytes){

        if(command_state == CLIENTCOM_LOGIN){//get universally unique ID, UUID, or GUID, all the same thing basically, 16 bytes
            if(data_bytes < 16)//don't have it all, wait
                return 0;

            UserEntry *u = system_base->users->Find((char *)&com_buf[cbuf_pos]);
            if(u == NULL){//no user is registered with this ID
                printf("Failed login attempt, ID is not registered.\n");
                Disco();
                return 1;
            }

            user_ent = u;
            sprintf(buf,"A1%s",user_ent->name);
            SendString(buf);

            printf("%s(%s) logged in.\n",user_ent->name,user_ent->realname);

            return 0;
        }



        if(command_state == CLIENTCOM_SET_STATE){
            if(com_bytes < 2)
                return 0;

        }


        if(command_state == CLIENTCOM_QUIT){
            SendChar(CLIENTCOM_QUIT);
            Disco();
            return 1;
        }


        if(command_state == CLIENTCOM_JOIN_ANY){//must specify length of rom name followed by rom name

            t = com_buf[cbuf_pos]-'0';//rom name length is sent as ASCII char for ease on Uzebox side
            if(com_bytes < (unsigned int)(t+1))//we do not have all the bytes of the rom name yet
                return 0;

        //create a new room
      //  Room *r = master->rooms
            return 0;
        }

    }//while(com_bytes)



    return 0;
}

Client *Client::Add(SystemEntry *base){
    if(next != NULL)
        return next->Add(base);

    next = new Client;
    next->prev = this;
    next->system_base = base;

    return next;
}

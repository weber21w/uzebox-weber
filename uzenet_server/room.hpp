#ifndef ROOM_HPP_INCLUDED
#define ROOM_HPP_INCLUDED

#ifdef _WIN32
    #include <winsock.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
    #include <unistd.h> /* Needed for close() */
#endif // _WIN32

#include "client.hpp"

class Client;

class Room{
public:
    Room();
    ~Room(){};


    Client *clients;
    Room *next;
    Room *prev;

    Room *Create();
    void Delete(Room *target);
    int Update();

};


int Room::Update(){
//return 1;
    Client *c = clients->next;
    while(c != NULL){

        if(c->sock == -1){//user is disconnected, but keep it around in case he reconnects?!?
            c = c->next;
            continue;
        }

   int32_t bytes;
    if((bytes = recv(c->sock,(char *)c->packet_buf,sizeof(c->packet_buf),0)) == SOCKET_ERROR){
        if(WSAGetLastError() != WSAEWOULDBLOCK){//critical error
            c->Disco();
            return 1;
        }
    }

    if(bytes > 0){
        c->packet_buf[bytes] = 0;
        printf("%s",c->packet_buf);
        continue;
    }
    c->data_bytes += bytes;

    if(c->com_bytes+c->data_bytes < sizeof(c->com_buf)){//copy packet data to command buffer if it will ALL fit
        memcpy(&c->com_buf[c->com_bytes],c->packet_buf,bytes);
        c->com_bytes += c->data_bytes;
        c->data_bytes = 0;
    }

    if(!c->com_bytes)//anything to process?
        return 0;

//PACKET_PROCESS_TOP:

    if(c->command_state == 0){//next byte should be a command
        c->command_state = c->com_buf[c->pbuf_pos++];//get a new command, we will only process it if the required arguments are completely here, else buffer and wait
        c->data_bytes--;
        if(c->command_state < CLIENTCOM_FIRST_COMMAND || c->command_state > CLIENTCOM_LAST_COMMAND){//bad command, client probably missed a UART bytes or logic failed
            c->Disco((char *)"ZZZZZZZZ");
            return 1;
        }
    }


    int32_t t;
    char buf[128];
    while(c->com_bytes){

        if(c->command_state == CLIENTCOM_LOGIN){//get universally unique ID, UUID, or GUID, all the same thing basically, 16 bytes
            if(c->user_ent != NULL){//attempting to login when already logged in
                c->Disco();
                break;
            }


            if(c->com_bytes < 12+5)//don't have it all, wait
                break;
printf("ohyeahyeahyeahyeahyeah");
            UserEntry *u;// = c->system_base->users->Find((char *)&com_buf[0]);
            if(u == NULL){//no user is registered with this ID
                c->com_buf[c->com_bytes] = 0;
                printf("Failed login attempt, ID %s is not registered.\n",c->com_buf);
                c->Disco();
                return 1;
            }

            c->user_ent = u;
            sprintf(buf,"A1%s",c->user_ent->name);
            c->SendString(buf);

            printf("%s(%s):",c->user_ent->name,c->user_ent->realname);
            c->PrintIP(&c->addr);
            printf(" logged in.\n");

            return 0;
        }



        if(c->command_state == CLIENTCOM_SET_STATE){
            if(c->com_bytes < 2)
                return 0;

        }


        if(c->command_state == CLIENTCOM_QUIT){
            c->SendChar(CLIENTCOM_QUIT);
            c->Disco();
            return 1;
        }


        if(c->command_state == CLIENTCOM_JOIN_ANY){//must specify length of rom name followed by rom name

            t = c->com_buf[c->cbuf_pos]-'0';//rom name length is sent as ASCII char for ease on Uzebox side
            if(c->com_bytes < (unsigned int)(t+1))//we do not have all the bytes of the rom name yet
                return 0;

        //create a new room
      //  Room *r = master->rooms
            return 0;
        }

    }//while(com_bytes)








        c = c->next;
    }
    return 1;
}

Room::Room(){
    next = prev = NULL;
    clients = new Client;
}

#endif // ROOM_HPP_INCLUDED

#ifndef ROOM_HPP_INCLUDED
#define ROOM_HPP_INCLUDED

#ifdef _WIN32
    #include <winsock.h>
    #include <process.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
    #include <unistd.h> /* Needed for close() */
#endif // _WIN32

#include "client.hpp"
#include "bots.hpp"

#define ROOM_OPEN       0
#define ROOM_RESERVED   1

class Client;
class RomEntry;

class Room{
public:
    Room();
    ~Room(){};


    Client *clients;
    Room *next;
    Room *prev;

    UserEntry *userlist;//hack
    RomEntry *romlist;//hack
    BotEntry *botlist;//hack
    UserEntry *guestlist;//list of user entries we are waiting for
    uint32_t state;
    uint32_t type;
    char game[32];
    Room *Create(char *gn, uint32_t t);
    void Delete(Room *target);
    int Update();
    UserEntry *FindUserEntry(char *pw);

};

Room *Room::Create(char *gn, uint32_t t){
    if(next)
        return next->Create(gn,type);
    next = new Room;
    next->prev = this;
    next->userlist = NULL;
    next->romlist = romlist;
    next->guestlist = NULL;
    next->state = 0;
    next->type = t;
    strcpy(next->game,gn);
    return next;
}
UserEntry *Room::FindUserEntry(char *pw){
    UserEntry *u = userlist;
    while(u){
        if(!strncmp(pw,u->id,12+5))
            break;
        u = u->next;
    }
    return u;
}

int Room::Update(){

    Client *c = clients;
    while(1){

        c = c->next;
        if(c == NULL)
            break;

        if(c->sock == -1)//user is disconnected, but keep it around in case he reconnects?!?
            continue;

        int32_t bytes;
        if((bytes = recv(c->sock,(char *)c->packet_buf,sizeof(c->packet_buf),0)) == SOCKET_ERROR){
            if(WSAGetLastError() != WSAEWOULDBLOCK){//critical error
                c->Disco();
                continue;
            }
        }

        if(bytes > 0){
            c->packet_buf[bytes] = '\0';
            printf("Got:%s,",c->packet_buf);
            // continue;

            c->data_bytes += bytes;

            if(c->com_bytes+c->data_bytes < sizeof(c->com_buf)){//copy packet data to command buffer if it will ALL fit
                memcpy(&c->com_buf[c->com_bytes],c->packet_buf,bytes);
                c->com_bytes += c->data_bytes;
                c->data_bytes = 0;
                c->com_buf[c->com_bytes] = '\0';
            }else{
                //overflow?!
            }
        }
        if(!c->com_bytes)
            continue;
        while(1){//process until a command cannot complete(because not enough bytes here)

//PACKET_PROCESS_TOP:
            if(c->command_state == CLIENTCOM_LOGIN){//the very first bytes the Uzebox must send is the login password
                if(c->com_bytes < 12+5)
                    break;//nothing we can do until all the data is here
              //  if(c->com_buf[0] == '+' && c->com_bytes < 12+5+11)//uzebox is sending the entire result, like "+CIPAPMAC:"01:23:45:67:89:AB"
                //    continue;

                //if here, they sent something we are going to interpret as a password
                printf("Interpreting as password:%s\n",c->com_buf);
                UserEntry *ue;

                char buf[64];

                if(c->com_buf[0] == '+')
                    strncpy(buf,(char *)&c->com_buf[11],12+5);
                else
                    strncpy(buf,(char *)&c->com_buf[0],12+5);
                ue = FindUserEntry(buf);
                if(ue == NULL){
                    printf("Failed to find user with password:%s\n",buf);
                    c->Disco();
                    break;
                }else{
                    c->user_ent = ue;
                    printf("User %s(%s) logged in\n",ue->name,ue->realname);
                    Client *oc = clients;
                    while(oc->next!= NULL){
                        oc = oc->next;
                        if(!oc->user_ent->allow_multiple_instances && oc != c && oc->user_ent != NULL && !strcmp(oc->user_ent->id,ue->id)){//was previously logged in
                            oc->Disco();
                            printf("%s was already logged in, disconnected prior instance\n",ue->name);
                        }
                    }
                    c->command_state = 1;
                    memset(buf,'\0',sizeof(ue->name));
                    sprintf(buf,"%s",ue->name);
                    send(c->sock,buf,sizeof(ue->name),0);
                    printf("sent:%d:%s,",sizeof(ue->name),buf);
                    c->RemoveCommandBytes(12+5);//c->RemoveCommandBytes((c->com_buf[0] == '+')?12+5+11:12+5);//get rid of the password data
                }
                continue;
            }


            if(c->command_state == 1){//next byte should be a command
                if(c->com_bytes < 1)
                    break;
                char com = c->com_buf[0];
                printf("\nfound command:%c\n",com);
                if(com < CLIENTCOM_FIRST_COMMAND || com > CLIENTCOM_LAST_COMMAND){//sent a bad command
                    printf("%s specified bad command:%c\n",c->user_ent->name,com);
                    c->Disco();
                    break;
                }
                c->command_state = com;
                c->RemoveCommandBytes(1);//TODO IMPLEMENT CIRCULAR BUFFER INSTEAD??
                continue;
            }

            if(c->command_state == CLIENTCOM_SET_GAME){//specify the game we are playing
                int len = c->com_buf[0]-'0';
                if(len > 16){
                    printf("%s specified bad game name length\n",c->user_ent->name);
                    c->Disco();
                    break;
                }
                if(c->com_bytes < 1+len)
                    break;
                memcpy(c->current_game,c->com_buf+1,len);
                c->current_game[len] = '\0';
                printf("%s is running:%s\n",c->user_ent->name,c->current_game);
                c->RemoveCommandBytes(1+len);
                c->command_state = 1;
                continue;
            }

            if(c->command_state == CLIENTCOM_JOIN_ANY){
                printf("%s sent CLIENTCOM_JOIN_ANY:%s\n",c->user_ent->name,c->com_buf);
                //If there is a reserved room we agreed to play in with someone else, join it.
                //Else find any open room playing the same game. Create one if none exists.

                Room *r = this;
                UserEntry *ue = NULL;
                while(r->prev != NULL)//find beginning of list
                    r = r->prev;
                while(1){
                    if(r == NULL || r->next == NULL)
                        break;
                    r = r->next;

                    ue = NULL;
                    if(r->type == ROOM_RESERVED){//see if we are on the guest list
                        ue = r->guestlist;
                        while(ue != NULL){
                            if(!strcmp(ue->id,c->user_ent->id))//we are on the guest list
                                break;
                            ue = ue->next;
                        }
                    }
                    if(strcmp(r->game,c->current_game))//wrong game
                        continue;
                    if(ue != NULL)//found a reserved room we belong to
                        break;
                }
                if(r == NULL){//no room found, create one
                    printf("%s created a new room\n",c->user_ent->name);
                    Create(c->current_game,ROOM_OPEN);
                }else{//some existing game meets requirements
                    if(ue != NULL){
                        printf("%s joined an reserved game\n",c->user_ent->name);
                    }else{
                        printf("%s joined an existing open game\n",c->user_ent->name);
                    }
                }

                c->command_state = 1;//no arguments for this one, byte was already removed when determining the command
                continue;
            }

            if(c->command_state == CLIENTCOM_REQUEST_BOT){
              printf("%s requested bot for game \n",c->user_ent->name);
                //create a bot instance in this room to play against, if one is available for this game
                //c->RemoveCommandBytes(1);
                //_spawnvp(_P_NOWAIT,"calc.exe",NULL);//_P_DETACH
                char p[128];
              // sprintf(p,"modules/%s",game);
                popen(p,"r");
                c->command_state = 1;
                continue;
            }

            printf("\nReached undefined state for %s\n",c->user_ent->name);
            c->Disco();
            break;


        }//while(c->com_bytes
    }//while(1)
    return 1;
}

Room::Room(){
    next = prev = NULL;
    clients = new Client;
    userlist = NULL;
    romlist = NULL;
    guestlist = NULL;
}

#endif // ROOM_HPP_INCLUDED

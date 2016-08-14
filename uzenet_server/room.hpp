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

uint32_t rolling_room_id = 0;
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
    Bot *bots;//hack
    SystemEntry *systembase;
    UserEntry *currentusers;//list of users currently in the room
    UserEntry *guestlist;//list of user entries we are waiting for
    uint32_t state;
    uint32_t type;
    uint32_t id;
    uint8_t num_players;
    uint8_t num_ready;
    char game[32];
    Room *Create(char *gn, uint32_t t, uint32_t i);
    void Delete(Room *target);
    int Update();
    UserEntry *FindUserEntry(char *pw);

};

Room *Room::Create(char *gn, uint32_t t, uint32_t i){
    if(next)
        return next->Create(gn,t,i);
    next = new Room;
    next->prev = this;
    next->userlist = userlist;
    next->romlist = romlist;
    next->guestlist = NULL;
    next->botlist = botlist;
    next->currentusers = NULL;
    next->state = 0;
    next->type = t;
    next->id = i;
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

    Client *c = clients->next;

    num_players = 0;
    num_ready = 0;
    while(c != NULL){//determine how many players and how many are ready so that queries for this info are up to date
        num_players++;
        if(c->ready)
            num_ready++;
        c = c->next;
    }

    c = clients;
    while(1){

        c = c->next;
        if(c == NULL)
            break;


        if(c->sock == -1)//user is disconnected, but keep it around in case he reconnects?!?
            continue;
        //TODO IF A USER IS CONNECTED TWICE, THINGS GET ALL CRAZY!!!!!!!!!!!!!!!

        int32_t bytes;
        if((bytes = recv(c->sock,(char *)c->packet_buf,sizeof(c->packet_buf),0)) == SOCKET_ERROR){
            if(WSAGetLastError() != WSAEWOULDBLOCK){//critical error
                printf("Got socket error:%d for %s\n",WSAGetLastError(),c->user_ent->name);
                c->Disco();
                continue;
            }
        }

        if(bytes > 0){
            c->packet_buf[bytes] = '\0';
            printf("Got packet:%s,",c->packet_buf);
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

        if(num_ready && num_ready >= c->notify_on_ready){//CLIENTCOM_READY_CUE handler
            char buf[16];
            buf[0] = CLIENTCOM_READY_CUE;
            buf[1] = '+';
            c->notify_on_ready = 0;//disable the alert
            send(c->sock,buf,2,0);
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
                if(c->com_bytes < (unsigned int)(1+len))
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
                Room *firstmatch = NULL;
                UserEntry *ue = NULL;
                while(r->prev != NULL)//find beginning of list
                    r = r->prev;

                while(r != NULL){//for each room
                    ue = NULL;
                    if(!strcmp(r->game,c->current_game)){//right game

                        if(firstmatch != NULL){//if we don't find a room we agreed to play in, choose the first open room with the right game
                            if(r->type == ROOM_OPEN)//it is possible to have a guest list on an open room as well
                                firstmatch = r;
                        }

                        ue = r->guestlist;
                        while(ue != NULL){//for each guest
                            if(!strcmp(ue->id,c->user_ent->id))//we previously arranged to be in this room, perhaps in the IRC client
                                break;
                            ue = ue->next;
                        }
                    }
                    if(ue != NULL)//found a reserved slot we setup
                        break;
                    r = r->next;
                }//while rooms

                if(r == NULL){//no room found, create one
                    printf("%s created a new room\n",c->user_ent->name);
                    Create(c->current_game,ROOM_OPEN,++rolling_room_id);
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

            if(c->command_state == CLIENTCOM_READY_CUE){//send the client "++" when a certain amount of clients in the room are ready
                if(c->com_bytes < 1)
                    break;

                c->notify_on_ready = c->com_buf[0]-'0';
                if(c->notify_on_ready > 8){
                    printf("Got bad notify on ready number: %d\n",c->notify_on_ready);
                    c->Disco();
                    break;
                }
                printf("%s set notify on ready active\n",c->user_ent->name);
                c->command_state = 1;
                c->RemoveCommandBytes(1);
                continue;
            }

            if(c->command_state == CLIENTCOM_REQUEST_BOT){
                    //TODO when spawning bot, lock the room so no one can join until the bot does....or something
                if(c->com_bytes < 2)
                    break;
                int len = c->com_buf[0]-'0';
                if(c->com_bytes < (uint8_t)(2+len))
                    break;
                if(len < 0 || len > 16){
                    printf("Got bad length on bot name from %s\n",c->user_ent->name);
                    c->Disco();
                    break;
                }
                int num = c->com_buf[1]-'0';//client can spawn multiple instances if the game requires
                if(num < 1 || num > 8){
                    printf("Got bad number of instances for bot from %s\n",c->user_ent->name);
                    c->Disco();
                }
                char nm[32];
                memcpy(nm,c->com_buf+2,len);
                nm[len] = '\0';
                if(num == 1)
                    printf("%s requested 1 instance of bot %s\n",c->user_ent->name,nm);
                else
                    printf("%s requested %d instances of bot %s\n",c->user_ent->name,num,nm);
                //create a bot instance in this room to play against, if one is available for this game

                BotEntry *be = botlist;
                while(be != NULL){
                    if(!strcmp(be->name,nm))
                        break;
                    be = be->next;
                }

                if(be == NULL){
                    printf("Cannot find bot %s for %s\n",nm,c->user_ent->name);
                    c->Disco();
                    break;
                }
                printf("Found registered bot: %s, with command %s\n",nm,be->command);

                UserEntry *ue = userlist;
                while(ue != NULL){//find super user "BOT_SUDO" so we can pass the password/id to the program, so it can then login as such
                    if(!strcmp(ue->name,"SUDO_BOT"))
                        break;
                    ue = ue->next;
                }
                if(ue == NULL){
                    printf("Cannot find BOT_SUDO to login external program\n");
                    c->Disco();
                    break;
                }
                printf("Found SUDO_BOT\n");
                char p[96];
                sprintf(p,"piped\\%s %s %d %d",be->command,ue->id,num,id);//the program will login with BOT_SUDO user and join by the passed instances and user ID
                printf("Creating pipe with:%s\n",p);


                FILE *phandle = popen(p,"w");
                //_spawnvp(_P_NOWAIT,"calc.exe",NULL);//_P_DETACH
                if(phandle == NULL){
                    printf("Cannot open pipe for %s\n",p);
                   // bots->Delete(b);//automatically closes pipe
                    c->Disco();
                    break;
                }
                //TODO MAKE A TASK QUEUE SO ANOTHER THREAD CAN OPEN PIPES TO PREVENTS I/O STALL FOR REST OF CLIENTS
                printf("Bot created for %s, using super user ID\n",ue->id);
                c->command_state = 1;
                c->RemoveCommandBytes(2+len);
                continue;
            }

            if(c->command_state == CLIENTCOM_GET_TIME){
                printf("%s requested time\n",c->user_ent->name);
                if(c->com_bytes < 2)
                    break;
//                int tz = c->com_buf[0];
                int len = c->com_buf[1]-'0';
                char fmt[32];
                char tbuf[64];
                if(c->com_bytes < (uint8_t)(1+len))
                    break;

                SPrintTime(fmt,tbuf);//precedes time string with 1 byte indicating string length
                send(c->sock,tbuf,strlen(tbuf),0);
            }

            if(c->command_state == CLIENTCOM_SET_READY){
                c->ready = true;
                c->command_state = 1;
                printf("%s set ready flag\n",c->user_ent->name);
                continue;
            }
            if(c->command_state == CLIENTCOM_ROOM_DETAILS){
                char buf[16];
                printf("%s requested room details\n",c->user_ent->name);
                buf[0] = CLIENTCOM_ROOM_DETAILS;
                buf[1] = num_players;
                buf[2] = num_ready;
                send(c->sock,buf,3,0);
                c->command_state = 1;
                continue;
            }

            if(c->command_state == CLIENTCOM_GET_USER_NO){
                printf("%s requested user number\n",c->user_ent->name);

                Client *cno = c;
                while(cno->prev){
                    cno = cno->prev;
                }
                cno = cno->next;//there is 1 dummy client in every room
                int num = 0;
                while(cno != c){
                    num++;
                    cno = cno->next;
                }
                printf("%s is user number %d\n",c->user_ent->name,num);
                char buf[16];
                buf[0] = CLIENTCOM_GET_USER_NO;
                buf[1] = num;
                send(c->sock,buf,2,0);
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

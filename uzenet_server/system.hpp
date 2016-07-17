//#ifndef SYSTEM_HPP_INCLUDED
//#define SYSTEM_HPP_INCLUDED
#ifdef _WIN32
    #include <winsock.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
    #include <unistd.h> /* Needed for close() */
#endif // _WIN32

#define SOCKETS_VERSION_MAJOR 2
#define SOCKETS_VERSION_MINOR 2

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

#define LISTEN_PORT 58000

class UserEntry;
class Room;

#include "users.hpp"
#include "room.hpp"


void db(){
while(1){

}
}

class SystemEntry{
public:
    SystemEntry();
    ~SystemEntry();

    int critical_err;
    unsigned int listen_sock;
    int sa_size;
    SOCKADDR_IN sock_addr;
    WSADATA WSAData;

    Room *rooms;
    UserEntry *users;
    int num_rooms;
    int num_users;
    char console_title[64];
    char old_console_title[64];
    FILE *flog;
    unsigned int listen_port;

    int Update();
    int LoadUserData();//load information with user names, password, etc for every registers Uzenet account
    int SaveUserData();
    void PrintIP(SOCKADDR_IN *sock_address);
    void PrintTime();

};


void SystemEntry::PrintIP(SOCKADDR_IN *sock_address){
    printf("%d.%d.%d.%d:%d",sock_address->sin_addr.S_un.S_un_b.s_b1,sock_address->sin_addr.S_un.S_un_b.s_b2,sock_address->sin_addr.S_un.S_un_b.s_b3,sock_address->sin_addr.S_un.S_un_b.s_b4,sock_address->sin_port);
}


SystemEntry::~SystemEntry(){
    SetConsoleTitleA(old_console_title);
}

SystemEntry::SystemEntry(){//constructor does all socket setup, etc

    users = new UserEntry;
    rooms = new Room;

    num_users = 0;
    num_rooms = 0;
    listen_port = LISTEN_PORT;

    printf(logo_string);
    GetConsoleTitleA(old_console_title,64);//doesn't work?

    FILE *flog = fopen("log.txt","w");
    if(flog == NULL){
        printf("---!!Failed to create log file.\n");
    }

    critical_err = 0;
    unsigned long block_mode = 1;//used to set sockets to non-blocking
//    int i = 0;
    sa_size = sizeof(SOCKADDR);


    if(WSAStartup(MAKEWORD(SOCKETS_VERSION_MAJOR,SOCKETS_VERSION_MINOR),&WSAData)){
        printf("-Failed on WSAStartup():%d for sockets version %d.%d\n",WSAGetLastError(),SOCKETS_VERSION_MAJOR,SOCKETS_VERSION_MINOR);
        critical_err = 1;
        return;
    }

    sock_addr.sin_port = htons(LISTEN_PORT);
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;

    if((listen_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET){
        printf("-Failed to create listen socket\n");
        critical_err = 2;
        return;
    }

    if((bind(listen_sock,(LPSOCKADDR)(&sock_addr),sizeof(sock_addr))) == SOCKET_ERROR){
        printf("-Failed to bind listen socket\n");
        critical_err = 3;
        return;
    }


    if(ioctlsocket(listen_sock,FIONBIO,&block_mode) != NO_ERROR){//Windows and BSD: accepted connections inherit this...not so on Linux?
        printf("-ioctlsocket() failed to set non-blocking mode on listen socket\n");
        critical_err = 4;
        return;
    }

    if(listen(listen_sock,SOMAXCONN)){
        printf("-Failed on listen(): %d\n",WSAGetLastError());
        critical_err = 5;
        return;
    }

    if(LoadUserData()){
        printf("-User file failure.\n");
        critical_err = 6;
    }

}

int SystemEntry::LoadUserData(){

    FILE *userf = fopen("users.dat","r");

    if(userf == NULL){
        printf("-Failed to open users.dat for reading.\n");
        userf = fopen("users.dat","w");
        if(userf == NULL){
            printf("--Failed to create users.dat, critical error.\n");
            return 1;
        }
        fclose(userf);
        userf = fopen("users.dat","r");
        if(userf == NULL){
            printf("--Failed to create users.dat, critical error.\n");
            return 1;
        }
        printf("--Created file users.dat\n");
        fclose(userf);
        return 0;
    }

    UserEntry *ue = users;
    ue = ue->next;

    int total_users = 0;

    while(!feof(userf)){

        if(ue == NULL)
            ue = users->Add();
       // if(ue->prev != NULL)//not the head

        if( !fread(ue->name,sizeof(ue->name),1,userf))//end of file reached
            break;

        if( !fread(ue->id,sizeof(ue->id),1,userf)                               ||
            !fread(ue->realname,sizeof(ue->realname),1,userf)                   ||
            !fread(&ue->time_zone,sizeof(ue->time_zone),1,userf)                ||
            (ue->name[sizeof(ue->name)-1] != ue->id[sizeof(ue->id)-1])          ||
            (ue->id[sizeof(ue->id)-1] != ue->realname[sizeof(ue->realname)-1])  ||
            (ue->realname[sizeof(ue->realname)-1] != 0)){
                printf("!!**CRITICAL ERROR: USER FILE IS CORRUPTED AT ENTRY %d**!!\n",total_users);
                fclose(userf);
                return 1;
        }

        total_users++;
        ue = ue->next;

    }

    fclose(userf);
    printf("Read users.data, %d registered users.\n",total_users);
    return 0;
}

int SystemEntry::SaveUserData(){

    FILE *userf = fopen("users.dat","w");

    if(userf == NULL){
        printf("-Failed to open users.dat for writing.\n");
        return 1;
    }

    UserEntry *ue = users;
    ue = ue->next;

    int total_users = 0;

    while(ue != NULL){
        fwrite(ue->name,sizeof(ue->name),1,userf);
        fwrite(ue->id,sizeof(ue->id),1,userf);
        fwrite(ue->realname,sizeof(ue->realname),1,userf);
        fwrite(&ue->time_zone,sizeof(ue->time_zone),1,userf);

        total_users++;
        ue = ue->next;
    }

    fclose(userf);
    printf("Wrote users.dat, total users %d.\n",total_users);
    ///////////////
    //////////////
    ///////////
    ////////
    ////
    //
    return 0;//LoadUserData();
}

int SystemEntry::Update(){

    sprintf(console_title,"UZENET V1.0  (%d rooms/%d users)",num_rooms,num_users);
    SetConsoleTitleA(console_title);
    sa_size = sizeof(SOCKADDR);

    int s = accept(listen_sock,(LPSOCKADDR)&sock_addr,&sa_size);
    if(s == SOCKET_ERROR){
        if(WSAGetLastError() == WSAEWOULDBLOCK){//asynchronous sockets will return this VERY often

        }else{
            printf("WSAGetLastError():%d\n",WSAGetLastError());
        }
    }else{//new connection
        printf("Got new connection request from ");
        PrintIP(&sock_addr);
        printf("\n");

        Client *c = rooms->clients->Add(this);
        c->sock = s;
        memcpy(&c->addr,&sock_addr,sizeof(SOCKADDR_IN));

    }

    Room *r = rooms;
    while(r != NULL){
        r->Update();
        r = r->next;
    }

    if(!num_users)
        Sleep(16);
    return 0;
}


//#endif // SYSTEM_HPP_INCLUDED

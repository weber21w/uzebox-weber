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
#include "score.hpp"


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
    HighScore *scores;
    RecentHighScore *recent;
    Bot *bots;
    int num_rooms;
    int num_users;
    int num_bot_entries;
    char console_title[64];
    char old_console_title[64];
    FILE *flog;
    unsigned int listen_port;

    int LockScores;//do not allow any modification until the webpage generating thread is finished

    int Update();
    int LoadUserData();//load information with user names, password, etc for every registers Uzenet account
    int SaveUserData();
    int LoadBotData();
    void PrintIP(SOCKADDR_IN *sock_address);
    void PrintTime();
    int GenerateWebPage();
    int FPrintCurrentClients(FILE *f, bool blackcomma, char *altcolor);
    Client *AddClient(Room *r);
    Client *AddClient(Room *r, UserEntry *u);
    UserEntry *FindUserEntry(char *pw);

};

UserEntry *SystemEntry::FindUserEntry(char *ps){

    return NULL;
}

Client *SystemEntry::AddClient(Room *r){
    if(r == NULL)//default room everyone joins at beginning
        r = rooms;

    Client *c = r->clients;
    while(c->next != NULL){
        c = c->next;
    }
    printf("Created client\n");
    c->next = new Client;
    c->next->prev = c;
    c->next->next = NULL;
    c->next->system_base = this;
    c->next->command_state = CLIENTCOM_LOGIN;
    c->next->user_ent = users;//first entry is the anonymous one until actually logged in
    c->next->notify_on_ready = 0;
    return c->next;
}

Client *SystemEntry::AddClient(Room *r, UserEntry *u){
    Client *c = AddClient(r);
    c->user_ent = u;
    return c;
}


void SystemEntry::PrintIP(SOCKADDR_IN *sock_address){
    printf("%d.%d.%d.%d:%d",sock_address->sin_addr.S_un.S_un_b.s_b1,sock_address->sin_addr.S_un.S_un_b.s_b2,sock_address->sin_addr.S_un.S_un_b.s_b3,sock_address->sin_addr.S_un.S_un_b.s_b4,sock_address->sin_port);
}


SystemEntry::~SystemEntry(){
    SetConsoleTitleA(old_console_title);
}

SystemEntry::SystemEntry(){//constructor does all socket setup, etc

    users = new UserEntry;
    rooms = new Room;
    scores = new HighScore;
    recent = new RecentHighScore;
    bots = new Bot;

    rooms->userlist = users;//hack
    rooms->romlist = scores->roms;//hack
    rooms->systembase = this;
    rooms->botlist = new BotEntry;

    strcpy(users->name,"UzenetBOT");
    strcpy(users->realname,"CPU Player");
    rooms->clients->user_ent = users;//always a default base client, a bot which does implements some features like AI, or something.
    num_users = 1;
    num_rooms = 0;
    listen_port = LISTEN_PORT;
    printf(logo_string);
    system("COLOR 0A");//make green/black color scheme
    GetConsoleTitleA(old_console_title,64);//doesn't work?

    FILE *flog = fopen("log.txt","w");
    if(flog == NULL){
        printf("---!!Failed to create log file.\n");
        critical_err = 1;
        return;
    }

    critical_err = 0;
    unsigned long block_mode = 1;//used to set sockets to non-blocking
//    int i = 0;
    sa_size = sizeof(SOCKADDR);


    if(WSAStartup(MAKEWORD(SOCKETS_VERSION_MAJOR,SOCKETS_VERSION_MINOR),&WSAData)){
        printf("-Failed on WSAStartup():%d for sockets version %d.%d\n",WSAGetLastError(),SOCKETS_VERSION_MAJOR,SOCKETS_VERSION_MINOR);
        critical_err = 2;
        return;
    }

    sock_addr.sin_port = htons(LISTEN_PORT);
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;

    if((listen_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET){
        printf("-Failed to create listen socket\n");
        critical_err = 3;
        return;
    }

    if((bind(listen_sock,(LPSOCKADDR)(&sock_addr),sizeof(sock_addr))) == SOCKET_ERROR){
        printf("-Failed to bind listen socket\n");
        critical_err = 4;
        return;
    }


    if(ioctlsocket(listen_sock,FIONBIO,&block_mode) != NO_ERROR){//Windows and BSD: accepted connections inherit this...not so on Linux?
        printf("-ioctlsocket() failed to set non-blocking mode on listen socket\n");
        critical_err = 5;
        return;
    }

    if(listen(listen_sock,SOMAXCONN)){
        printf("-Failed on listen(): %d\n",WSAGetLastError());
        critical_err = 6;
        return;
    }

    if(LoadUserData()){
        printf("-User file failure.\n");
        critical_err = 7;
    }

    if(LoadBotData()){
        printf("-Bot file failure.\n");
        critical_err = 8;
    }

}

int SystemEntry::LoadBotData(){
    printf("Loading pipe bot database...\n");

    FILE *userf = fopen("pipebot.dat","r");

    if(userf == NULL){
        printf("-Failed to open pipebot.dat for reading.\n");
        userf = fopen("pipebot.dat","w");
        if(userf == NULL){
            printf("--Failed to create users.dat, critical error.\n");
            return 1;
        }
        fclose(userf);
        userf = fopen("pipebot.dat","r");
        if(userf == NULL){
            printf("--Failed to create pipebot.dat, critical error.\n");
            return 1;
        }
        printf("--Created file pipebot.dat\n");
        fclose(userf);
        return 0;
    }

    BotEntry *be = rooms->botlist;


    int total_entries = 0;

    while(!feof(userf)){

        if(be == NULL)
            be = rooms->botlist->Create((char *)"BLANK",0,0);
       // if(ue->prev != NULL)//not the head

        if( !fread(be->name,sizeof(be->name),1,userf))//end of file reached
            break;
        if(!fread(be->command,sizeof(be->command),1,userf)){
                printf("!!**CRITICAL ERROR: PIPE BOT FILE IS CORRUPTED AT ENTRY %d**!!\n",total_entries);
                fclose(userf);
                return 1;
        }

        printf("+%s:%s\n",be->name,be->command);
        total_entries++;
        be = be->next;

    }

    fclose(userf);
    printf("Read pipebot.dat, %d registered bots.\n\n",total_entries);

    return 0;
}

int SystemEntry::LoadUserData(){

    printf("Loading user database...\n");

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

    strcpy(ue->name,"anonymous");//this entry never gets searched by login methods, so attackers cannot use it even with knowledge of the "uuid"...
    strcpy(ue->realname,"not logged in");
    strcpy(ue->id,"00:00:00:00:00:00");
    ue->time_zone = 0;
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

        printf("+%s(%s):%s\n",ue->name,ue->realname,ue->id);
        total_users++;
        ue = ue->next;

    }

    fclose(userf);
    printf("Read users.dat, %d registered users.\n\n",total_users);


   userf = fopen("romcatdef.dat","r");

    if(userf == NULL){
        printf("-Failed to open romcatdef.dat for reading.\n");
        userf = fopen("romcatdef.dat","w");
        if(userf == NULL){
            printf("--Failed to create romcatdef.dat, critical error.\n");
            return 1;
        }
        fclose(userf);
        userf = fopen("romcatdef.dat","r");
        if(userf == NULL){
            printf("--Failed to create romcatdef.dat, critical error.\n");
            return 1;
        }
        printf("--Created file romcatdef.dat\n");
        fclose(userf);
        return 0;
    }
    fclose(userf);
    printf("Read romcatdef.dat, %d registered roms with %d categories\n\n",scores->num_roms,scores->num_categories);

    return 0;
}

int SystemEntry::SaveUserData(){

    FILE *userf = fopen("users.dat","w");

    if(userf == NULL){
        printf("-Failed to open users.dat for writing.\n");
        return 1;
    }

    UserEntry *ue = users->next;//skip over the first "anonymous" entry which is manually inserted and not stored in file

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

 //   sprintf(console_title,"UZENET V1.0  (%d rooms/%d users)",num_rooms,num_users);
  //  SetConsoleTitleA(console_title);
    sa_size = sizeof(SOCKADDR);

    if(num_users < MAX_USERS){
        int s = accept(listen_sock,(LPSOCKADDR)&sock_addr,&sa_size);
        if(s == SOCKET_ERROR){
            if(WSAGetLastError() == WSAEWOULDBLOCK){//asynchronous sockets will return this VERY often

            }else{
                printf("WSAGetLastError():%d\n",WSAGetLastError());
            }
        }else{//new connection
            if(++num_users == MAX_USERS)
                printf("Reached maximum server load of %d\n",MAX_USERS);

            printf("Got new connection request from ");
            PrintIP(&sock_addr);
            printf("\n");

            Client *c = AddClient(NULL);//start off in the default room
            c->sock = s;
            bool rval = true;
            setsockopt(c->sock,IPPROTO_TCP,TCP_NODELAY,(char *)&rval,sizeof(int));//disable Nagle algorithm's mandatory 200ms delay
            memcpy(&c->addr,&sock_addr,sizeof(SOCKADDR_IN));

        }
    }

    Room *r = rooms;
    while(r != NULL){

        r->Update();
        r = r->next;
    }

   // if(!num_users)
        Sleep(1);
    return 0;
}

int SystemEntry::FPrintCurrentClients(FILE *f, bool blackcomma, char *altcolor){

    Room *r = rooms;
    Client *c;
    int w;
    while(r != NULL){//FOR EACH ROOM
        c = r->clients;
        w = 0;
        while(c != NULL){//FOR EACH CLIENT IN ROOM
            if(!blackcomma)
                fprintf(f,"%s",c->user_ent->name);
            else{
                fprintf(f,"<a href=\"http://uzebox.org/forums/memberlist.php?mode=viewprofile&u=%s\">%s</a>",c->user_ent->profile_id,c->user_ent->name);
            }
            if(++w > 3){
                w = 0;
                fprintf(f,"\n");
            }
            c = c->next;
            if(c != NULL){
                if(blackcomma)//writing to an html file
                    fprintf(f,"<font color=\"black\">,<font color=\"%s\"",altcolor);
                else//writing to a log file etc., or an html where we are leaving a single color...not graceful...TODO
                    fprintf(f,",");
            }
        }
        r = r->next;
    }
    return 0;
}


int SystemEntry::GenerateWebPage(){


scores->updated = true;
//    RomEntry *rtemp = scores->CreateRom((const char *)"RCPROAM",(const char *)"R.C. Pro Am");
    RomCategory *ctemp = scores->roms->CreateCategory((char *)"RCPROAM",(char *)"Track 1");
    ctemp->CreateEntry((char *)"D3thAdd3r",999,rawtime);

//    RomEntry *slt = scores->CreateRom((char *)"SOLITAIR",(char *)"Solitaire");
//   slt->CreateCategory("SOLITAIR","cards");
 //   slt = scores->CreateRom((char *)"FRGFEAST",(char *)"Frog Feast");
   // slt = scores->CreateRom((char *)"MEGATRIS",(char *)"Megatris");
   // ScoreEntry *stemp = ctemp->CreateEntry("D3thAdd3r",1001,rawtime);

  //  RomCategory *c2 = scores->roms->CreateCategory("RCPROAM","Track 2");
   // c2->CreateEntry("Alec",2002,rawtime);

  //  printf("%s\n",scores->roms->romname);
  //  recent->Add(rtemp,ctemp,stemp);
    //scores->CreateCategory("RCPROAM","Track 2");

    RomEntry *h;
    RomCategory *c;
    while(1){
        if(!scores->updated){
            Sleep(500);
            continue;
        }
        FILE *f = fopen("index.html","w");
        if(f == NULL){
            Sleep(500);
            continue;
        }
        LockScores = 1;//do not allow scores to be modified until we are done

        fprintf(f,html_head,uzenetlogo_image_dat);
        fprintf(f,"<b>");
        fprintf(f,"%d Users Registered<br>",4);
        if(num_users == 1)
            fprintf(f,"%d User Playing:<br>",num_users);
        else
            fprintf(f,"%d Users Playing:<br>",num_users);
        fprintf(f,"<font color=\"blue\">&nbsp;&nbsp;");
        FPrintCurrentClients(f,true,(char *)"blue");
        fprintf(f,"</b>");
        fprintf(f,"<font color=\"black\">");

        //show the most recent high scores made, for any rom
        fprintf(f,"<br><br>\n<b><i><font size=\"6\">Recent Achievements</font></i></b>");
        fprintf(f,"<table class=\"names\"><tr>");
        fprintf(f,"<th>Player</th><th>Game</th><th>Category</th><th>Score</th><th>Date</th></tr>");
        RecentHighScore *rhs = recent->next;

        int i=0;
        while(rhs != NULL){//FOR EACH RECENT RECORD
            i++;
            fprintf(f,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%d</td><td>12 Jan 2016</td>",rhs->score->name,rhs->rom->fullname,rhs->category->name,rhs->score->mag);
            rhs = rhs->next;
        }

            if(!i)
                fprintf(f,"<tr><td colspan=5><b>No Records, Yet!</b></td></tr><br><br>");
            else
                fprintf(f,"<br>");
            fprintf(f,"</table><br>");
        fprintf(f,(const char *)"<br><br><br>\n<b><i><font size=\"6\">World Records</font></i></b>\n<br><br>");


        h = scores->roms->next;
        while(h != NULL){//FOR EACH ROM

            //make table header
            fprintf(f,"<table class=\"names\"><tr>");
            fprintf(f,"<caption font-family=\"Time New Roman\" font-size=\"18\"><b>%s</b></caption>",h->fullname);
            fprintf(f,"<th>Category</th><th>Player</th><th>Score</th><th>Date</th></tr>");

            c = h->categories->next;
            bool labelled = false;
            i = 0;
            while(c != NULL){//FOR EACH CATEGORY IN EACH ROM
                i++;
                fprintf(f,"<tr>");
                if(!labelled){
                    labelled = true;

                    fprintf(f,"<td>%s</td>",c->name);
                }else
                    fprintf(f,"<td></td>");

                fprintf(f,"<td>D3thAdd3r</td><td>1000 pts.</td><td>10 Jan 2016</td>");


                fprintf(f,"</tr>");
                c = c->next;
            }

            if(!i)
                fprintf(f,"<tr><td colspan=4><b>No Records, Yet!</b></td></tr><br><br>");
            else
                fprintf(f,"<br>");
            fprintf(f,"</table><br>");
            h = h->next;
        }
        fprintf(f,"All UzeNet source code is available on <a href=\"https://github.com/weber21w/uzebox-weber\">GitHub</a><br>UzeNet is licensed under <a href=\"https://www.gnu.org/licenses/gpl-3.0-standalone.html\">GPL v3<br>");

        fprintf(f,"<img src=\"data:image/png;base64,%s\" align=\"middle\"></a>",gpl_image_dat);
        fprintf(f,(const char *)"</body>");

        fclose(f);
        printf("Updated web page.\n");
        scores->updated = false;
        LockScores = 0;
        Sleep(500);
    }
    return 0;
}

//#endif // SYSTEM_HPP_INCLUDED

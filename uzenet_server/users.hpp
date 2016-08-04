#ifndef USERS_HPP_INCLUDED
#define USERS_HPP_INCLUDED

#include <stdio.h>

class UserEntry{
public:
    UserEntry(){next=prev=NULL;sprintf(profile_id,"1779");};
    ~UserEntry(){};

    char name[13+1];
    char id[12+5+1];
    char profile_id[8];

    char realname[30+1];
    unsigned char time_zone;
    bool allow_multiple_instances;//for game bots

    UserEntry *next;
    UserEntry *prev;

    UserEntry *Add();
    UserEntry *CreateNew(char *nm, char *id, char *rn, int tz);
    UserEntry *Find(char *sought_id);
    int LoadUserDatabase();
};



UserEntry *UserEntry::Add(){

    if(next != NULL)
        return next->Add();

    next = new UserEntry;
    next->prev = this;

    memset(next->name,0,sizeof(next->name));
    memset(next->id,0,sizeof(next->id));
    memset(next->realname,0,sizeof(next->realname));
    next->time_zone = 0;

    return next;
}

UserEntry *UserEntry::CreateNew(char *nm, char *id, char *rn, int tz){
    UserEntry *t = Add();

    strcpy(t->name,nm);
    strcpy(t->id,id);
    strcpy(t->realname,rn);
    t->time_zone = tz;

    return t;
}


UserEntry *UserEntry::Find(char *sought_id){

    if(next == NULL)
        return NULL;

    UserEntry *u = next;//protect against the first entry which is "anonymous" and can't be used for anything good

    while(u != NULL){
        if(!strncmp(u->id,sought_id,12+5))
            return next;
        u = u->next;
    }

    return NULL;
}

int UserEntry::LoadUserDatabase(){


    return 0;
}
#endif // USERS_HPP_INCLUDED

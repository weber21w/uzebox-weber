#include <stdio.h>
#include <string.h>

#include "users.hpp"

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

    UserEntry *u = next;

    while(u != NULL){
        if(!strncmp(u->id,sought_id,12+5))
            return next;
        u = u->next;
    }

    return NULL;
}

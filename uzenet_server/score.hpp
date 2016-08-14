#ifndef SCORE_HPP_INCLUDED
#define SCORE_HPP_INCLUDED

class ScoreEntry{
public:
    ScoreEntry(){};
    ~ScoreEntry(){};

    char name[13+1];
    uint32_t mag;
    time_t date;

    ScoreEntry *next;
    ScoreEntry *prev;
};

class RomCategory{
public:
    RomCategory(){next=prev=NULL;};
    ~RomCategory();

    char name[15+1];
    char suffix[8];//like "pts" in "1000pts"
    char prefix[8];//like "par" in "par-4"...golf game example..
    uint32_t max_entries;
    uint32_t num_entries;

    ScoreEntry *entries;
    RomCategory *next;
    RomCategory *prev;
    ScoreEntry *CreateEntry(char *n, uint64_t m, time_t d);
};

class RomEntry{
public:
    RomEntry(){next=prev=NULL;categories=new RomCategory;};
    ~RomEntry();

    char romname[8+1];
    char fullname[31+1];

    uint32_t num_categories;
    uint32_t num_entries;

    RomCategory *categories;
    RomEntry *next;
    RomEntry *prev;
    RomCategory *CreateCategory(char *rn, char *cn);
};

class RecentHighScore{
public:
    RecentHighScore(){rom=new RomEntry;category = new RomCategory; score = new ScoreEntry;next=NULL;prev=NULL;};
    ~RecentHighScore(){};

    RomEntry *rom;
    RomCategory *category;
    ScoreEntry *score;

    RecentHighScore *next;
    RecentHighScore *prev;
    RecentHighScore *Add(RomEntry *r, RomCategory *c, ScoreEntry *s);
};

RecentHighScore *RecentHighScore::Add(RomEntry *r, RomCategory *c, ScoreEntry *s){

    RecentHighScore *t = new RecentHighScore;
    t->rom = r;
    t->category = c;
    t->score = s;

    //insert it at the top of the list
    RecentHighScore *n = next;
    if(next == NULL){//no entries exist yet
        next = t;
        next->prev = this;
    }else{
        next->prev = t;//point the old top reversal to the new entry
        next = t;//make the new entry the top of the list
        t->next = n;
    }
    return t;
    //TODO, ELIMINATE EXCESS HIGH SCORES PAST SOME AMOUNT...
}

class HighScore{
public:
    HighScore(){roms = new RomEntry;num_roms=num_categories=0;updated=false;};
    ~HighScore(){};

    uint32_t num_roms;
    uint32_t num_categories;

    bool updated;
    RomEntry *roms;
    RomEntry *FindRomByName(char *rn);
    RomCategory *FindCategoryByName(char *rn, char *cn);

    RomEntry *CreateRom(char *rn, char *fn);
    RomCategory *CreateCategory(char *rn, char *cn, char *pf, char *sf);
    ScoreEntry *CreateEntry(char *rn, char *cn, UserEntry *ue);
    UserEntry *FindUserEntryByName(char *name);
    int LoadScoreDatabase();
    int SaveScoreDatabase();

};

RomEntry *HighScore::FindRomByName(char *rn){

    RomEntry *re = roms;
    while(re != NULL){
        if(!strcmp(rn,re->romname))//this is the rom
            break;
        re = re->next;
    }
    return re;
}

RomCategory *HighScore::FindCategoryByName(char *rn, char *cn){
    RomEntry * re = FindRomByName(rn);
    if(re == NULL)
        return NULL;

    RomCategory *rc = re->categories;
    while(rc != NULL){
        if(!strcmp(cn,rc->name))//this is the category
            break;
        rc = rc->next;
    }
    return rc;
}

RomEntry *HighScore::CreateRom(char *rn, char *fn){
    RomEntry *r = roms;
    while(r->next != NULL){
        r = r->next;
    }

    r->next = new RomEntry;
    r->next->categories = new RomCategory;
    r->next->prev = r;
    strcpy(r->next->romname,rn);
    strcpy(r->next->fullname,fn);

    num_roms++;
    return r->next;
}

RomCategory *HighScore::CreateCategory(char *rn, char *cn, char *pf, char *sf){
    RomEntry *re = FindRomByName(rn);

    if(re == NULL){
        printf("can't find rom name %s, cannot create category %s\n",rn,cn);
        return NULL;
    }

    RomCategory *rc = FindCategoryByName(rn,cn);
    if(rc != NULL){
        printf("can't create category %s for rom %s, it already exists\n",cn,rn);
        return NULL;
    }

    rc = re->categories;
    RomCategory *last = NULL;
    while(rc != NULL){
        last = rc;
        rc = rc->next;
    }
    if(last == NULL)
        return NULL;
    last->next = new RomCategory;
    last->prev = last;
    last->next->next = NULL;
    strcpy(last->name,cn);//TODO USE STRNCMP FOR ALL THIS STUFF
    strcpy(last->prefix,pf);
    strcpy(last->suffix,sf);

    return last->next;
}

ScoreEntry *HighScore::CreateEntry(char *rn, char *cn, UserEntry *ue){
    RomCategory *rc = FindCategoryByName(rn,cn);

    if(rc == NULL){
        printf("can't find category %s and/or rom %s\n",cn,rn);
        return NULL;
    }

 //   return rc;
    return NULL;
}

UserEntry *HighScore::FindUserEntryByName(char *name){
    return NULL;
}


RomCategory *RomEntry::CreateCategory(char *rn, char *cn){
    RomEntry *r = next;
    while(r != NULL){
        if(!strcmp(rn,r->romname)){//found the rom this category belongs to
            break;
        }
        r = r->next;
    }
    if(r == NULL){//didn't find it
        printf("Failed to find rom \"%s\"\n",rn);
        return NULL;
    }
    printf("%s\n",r->fullname);

    RomCategory *c = r->categories;
    while(c->next != NULL){
        c = c->next;
    }
//return NULL;
    c->next = new RomCategory;
    c->next->prev = c;
    strcpy(c->next->name,cn);
    num_categories++;
    return c->next;
}

ScoreEntry *RomCategory::CreateEntry(char *n, uint64_t m, time_t d){

    //these are always inserted in a sorted order, based on magnitude and in the case of a tie, by earliest date
    ScoreEntry *t = new ScoreEntry;
    strcpy(t->name,n);
    t->mag = m;
    t->date = d;

    ScoreEntry *s = entries;
    do{
        if(t->mag > s->mag){//insert new score here, pushing all the rest back

            break;
        }
        s = s->next;
    }while(s != NULL);

    return t;
}

int HighScore::SaveScoreDatabase(){

    updated = false;//don't write unless something has changed
    FILE *f = fopen("hiscore.dat","w");
    if(f == NULL){
        printf("Cannot open hiscore.dat for writing.\n");
        return 1;
    }

    RomEntry *r = roms;
    RomCategory *c;
    ScoreEntry *s;

    unsigned int rcount,ccount,ecount;
    rcount = 0;
    //HighScore(1)->Roms(many)->Categories(many,many)->Entries(many,many,many)
    fwrite(&num_roms,sizeof(num_roms),1,f);
    while(r != NULL){//FOR EACH ROM IN HIGH SCORES
        rcount++;
        fwrite(r->romname,sizeof(r->romname),1,f);
        fwrite(r->fullname,sizeof(r->fullname),1,f);

        ccount = 0;
        fwrite(&r->num_categories,sizeof(r->num_categories),1,f);
        c = r->categories;
        while(c != NULL){//FOR EACH CATEGORY IN ROM
            ccount++;
            fwrite(&c->num_entries,sizeof(c->num_entries),1,f);
            fwrite(c->name,sizeof(c->name),1,f);

            ecount = 0;
            s = c->entries;
            while(s != NULL){//FOR EACH ENTRY IN CATEGORY
                ecount++;
                fwrite(s->name,sizeof(s->name),1,f);
                fwrite(&s->mag,sizeof(s->mag),1,f);
                fwrite(&s->date,sizeof(s->date),1,f);

                s = s->next;
            }
            if(ecount != c->num_entries){
                printf("----!!!Score database is corrupt on write. ecount != num_entries:%d:%d:%d",rcount,ccount,ecount);
            }
            c = c->next;

        }
        if(ccount != r->num_categories){
            printf("---!!!Score database is corrupt on write. ccount != num_categories:%d:%d!!!---\n",rcount,ccount);
        }
        r = r->next;
    }

    if(rcount != num_roms){
        printf("--!!!Score database is corrupted on write. rcount != num_roms:%d!!!--\n",rcount);
    }

    return 0;
}

int HighScore::LoadScoreDatabase(){

    FILE *f = fopen("hiscore.dat","r");
    if(f == NULL){
        printf("Failied to open hiscore.dat.\n");
        f = fopen("hiscore.dat","w");
        if(f == NULL){
            printf("Cannot create file hiscore.dat.\n");
            return 1;
        }
        printf("Created file hiscore.dat.\n");
        return 0;
    }
    return 0;
}
#endif // SCORE_HPP_INCLUDED

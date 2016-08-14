class BotEntry{//list of authorized programs
public:
    BotEntry(){next=prev=NULL;method=0;command[0] = '\0';};
    ~BotEntry(){};

    uint32_t method;
    uint32_t flags;
    char command[32];
    char name[32];
    BotEntry *next;
    BotEntry *prev;

    BotEntry *Create(char *c, int m, int f);
    void Delete(BotEntry *b);
};

BotEntry *BotEntry::Create(char *c, int m, int f){
    if(next != NULL)
        return next->Create(c,m,f);

    next = new BotEntry;
    next->next = NULL;
    next->prev = this;
    next->method = m;
    next->flags = f;
    return next;
}

class Bot{
public:
    Bot(){phandle=NULL;next=prev=NULL;method=flags=0;};
    ~Bot(){};

    FILE *phandle;
    uint32_t method;
    uint32_t flags;
    Bot *next;
    Bot *prev;

    Bot *Create(char *c, int m, int f);
    void Delete(Bot *b);
};

Bot *Bot::Create(char *c, int m, int f){
    if(next != NULL)
        return next->Create(c,m,f);
    next = new Bot;
    next->next = NULL;
    next->prev = this;
    next->method = m;
    next->flags = f;
    return next;
}

void Bot::Delete(Bot *b){
    pclose(b->phandle);
    //Bot *t = b;
    b->next->prev = b->prev;
    if(b->prev)
        b->prev->next = b->next;
    delete b;
}

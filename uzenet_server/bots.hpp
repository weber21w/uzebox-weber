class BotEntry{
public:
    BotEntry(){next=prev=NULL;method=0;command[0] = '\0';};
    ~BotEntry(){};

    int method;
    int flags;
    char command[128];
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


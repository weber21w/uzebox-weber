const char *logo_string =
"===========================================================================\n\n"
"\t ...    ::::::::::::.,:::::::::.    :::.,::::::::::::::::::\n"
"\t ;;     ;;;'`````;;;;;;;''''`;;;;,  `;;;;;;'''';;;;;;;;''''\n"
"\t[['     [[[    .n[[' [[cccc   [[[[[. '[[[[cccc      [[\n"
"\t$$      $$$  ,$$P\"   $$\"\"\"\"   $$$ \"Y$c$$$$\"\"\"\"      $$\n"
"\t88    .d888,888bo,_  888oo,__ 888    Y88888oo,__    88,\n"
"\t \"YmmMMMM\"\" `\"\"*UMMMm\"\"\"\"YUMMMMMM     YM\"\"\"\"YUMMM   MMM\n\n"
"         \"Connecting your world, 8 bits at a time!\"   [GPL V3.0]\n\n"
"===========================================================================\n\n";


class SystemEntry;
class Client;
class Room;
class UserEntry;

time_t rawtime;
struct tm * timeinfo;
time_t time_val;
char time_buffer[64];

void PrintTime(){
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_buffer,64,"%x %H:%M:%S",timeinfo);//"%a %b %d %Y %X %z",timeinfo);
    printf(time_buffer);
}

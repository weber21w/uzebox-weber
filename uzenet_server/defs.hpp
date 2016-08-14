const char *logo_string =
"===========================================================================\n\n"
"\t ...    ::::::::::::.,:::::::::.    :::.,::::::::::::::::::\n"
"\t ;;     ;;;'`````;;;;;;;''''`;;;;,  `;;;;;;'''';;;;;;;;''''\n"
"\t[['     [[[    .n[[' [[cccc   [[[[[. '[[[[cccc      [[\n"
"\t$$      $$$  ,$$P\"   $$\"\"\"\"   $$$ \"Y$c$$$$\"\"\"\"      $$\n"
"\t88    .d888,888bo,_  888oo,__ 888    Y88888oo,__    88,\n"
"\t \"YmmMMMM\"\" `\"\"*UMMMm\"\"\"\"YUMMMMMM     YM\"\"\"\"YUMMM   MMM\n\n"
"         \"Connecting your world, 8-bits at a time!\"   [GPL V3.0]\n\n"
"===========================================================================\n\n";


const char *gpl_image_dat =
"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";

const char *uzenetlogo_image_dat =
"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";

const char *html_head =
"<!DOCTYPE html><html>"
"<head><title>UzeNet Multiplayer Gaming</title>"
"<style type=\"text/css\">"
"table {"
" width:100%;"
//"  text-align: center;"
"}"
"table, th, td {"
"   border: 1px solid black;"
"	border-collapse: collapse;"
//"   float: centered;"
"   font-size: 28px;"
"   text-align: center;"
"   text-color: white;"
"    margin-left:auto;"
"    margin-right:auto;"
"    width:100%;"
"}"
"th, td {"
"	padding: 35px;"
"	text-align: center;"
"    margin-left:auto;"
"    margin-right:auto;"
"}"
"table.names tr:nth-child(even) {"
"	background-color: #eee;"
"	text-align: center;"
"    margin-left:auto;"
"    margin-right:auto;"
"}"
"	table.names tr:nth-child(odd) {"
"	background-color:#fff;"
"	text-align: center;"
"    margin-left:auto;"
"    margin-right:auto;"
"}"
"	table.names th  {"
"	background-color: black;"
"	color: white;"
"    margin-left:auto;"
"    margin-right:auto;"
"}"

"ul {"
"    list-style-type: none;"
"    margin: 0;"
"    padding: 0;"
"    overflow: hidden;"
"    background-color: #333;"
"	text-align: center;"
"}"

"li {"
"    float: left;"
"}"

"li a {"
"    display: block;"
"    color: white;"
"    text-align: center;"
"    padding: 14px 16px;"
"    text-decoration: none;"
"}"

"li a:hover:not(.active) {"
"    background-color: #111;"
"}"

".active {"
"    background-color: #4CAF50;"
"}"

"body {"
"    text-align: center;"
"    color: black;"
"    font-size: 16px;"
"    background-color: #FFFFFF;"
"}"
"</style></head>"





"<body>"
//"<p style=\"font-family:'Courier New' font-size:32px;\"><img src=\"uzenetlogo.png\"><b><i>\"Connecting your world, 8-bits at a time!\"</b></i></p><br><br>"

  "<p style=\"font-family:'Serif' font-size:32px;\"><img src=\"data:image/png;base64,%s\"><b><i>\"Connecting your world, 8-bits at a time!\"</b></i></p><br><br>"
//"<div class=\"tc_div_45175\" style=\"width:100%;height:auto;border:0px solid #C0C0C0\"><a title=\"Uzebox Coding Competition 2016\" href=\"http://www.tickcounter.com/widget/countdown/1483264800000/america-montreal/dhms/FFFFFF0000000B30F90028FF/0/C0C0C00/Uzebox_Coding_Competition_2016\">Uzebox Coding Competition 2016</a><a title=\"Countdown\" href=\"http://www.tickcounter.com/\">Countdown</a></div><script type=\"text/javascript\">(function(){ var s=document.createElement('script');s.src=\"http://www.tickcounter.com/loader.js\";s.async='async';s.onload=function() { tc_widget_loader('tc_div_45175', 'Countdown', 650, [\"1483264800000\",\"america-montreal\",\"dhms\",\"FFFFFF0000000B30F90B30F9\",\"0\",\"C0C0C00\",\"Uzebox Coding Competition 2016\"]);};s.onreadystatechange=s.onload;var head=document.getElementsByTagName('head')[0];head.appendChild(s);}());</script>"

"<ul><li><a href=\"http://uzebox.org/forums\">Forums</a></li>"
"<li><a href=\"http://uzebox.org\">News</a></li>"
"<li><a href=\"http://uzebox.org/shop/catalog\">Get Hardware!</a></li>"
"<li><a href=\"http://uzebox.org/shop/catalog\">Get Emulator!</a></li>"
"<li><a href=\"http://uzebox.net/bugz\">Play In Browser!</a></li>"
"<li><a href=\"http://uzebox.org/wiki/uzenet\">WIKI</a></li>"
"<li><a href=\"http://uzebox.org/wiki/uzenet_faq\">FAQ</a></li></ul>"
//"  <li style=\"float:right\"><a class=\"active\" href=\"#about\">About</a></li>"
//"<b><i>Chat with other users</i></b><br>"
"<iframe src=\"https://kiwiirc.com/client/irc.uzebox.net/?nick=Anonymous|?&theme=basic#lobby\" style=\"border:0; width:800px; height:320px;\"></iframe><br>";

#define MAX_USERS 128

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

void SPrintTime(char *fmt,char *buf){
    time(&rawtime);
    //TODO
    char buf2[64];
    sprintf(buf2,"1:11am");
    sprintf(buf,"%d%s",strlen(buf2),buf2);
}

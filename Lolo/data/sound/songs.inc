#include "lolo1song.inc"
#include "lolo2song.inc"
#include "lolo3song.inc"
#include "passwordsong.inc"
#include "lolodiesong.inc"
#include "eggerland1song.inc"
#include "eggerland2song.inc"
//#include "intermissionsong.inc"
#include "giveupsong.inc"
//#include "victorysong.inc"

#define NUMSONGS 6
const char *LoloMusic[] PROGMEM = {
(const char *)&Lolo1Song,
(const char *)&Lolo2Song,
(const char *)&Lolo3Song,
(const char *)&Eggerland1Song,
(const char *)&Eggerland2Song,
(const char *)&GiveUpSong,
(const char *)&LoloDieSong,
(const char *)&PasswordSong,
};

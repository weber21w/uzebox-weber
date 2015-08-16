#define MINE_SWEEPER_ID 68
//#define DEBUGGING 1

#define MAXWIDTH 28
#define MAXHEIGHT 20//16
#define MINWIDTH 9
#define MINHEIGHT 9
#define MAXMINES 99
#define MINMINES 10

#define OPEN 0
#define CLICKED 1
#define MINE 10
#define MINERED 11
#define MINEX 12
#define FLAG 13
#define QUESTION 14

#define GMAIN 1
#define GLOSE 10

#define BLANKT 13

#define NUMSONGS 5
const char *musicData[] PROGMEM={
NULL,
Song1,
Song2,
Song3,
Song4,
Song5,
};


extern int abs();
extern unsigned char vram[];

uint8_t mx,my;
bool mlb,mlbwd,mrb,mrbwd;
uint8_t mgridx,mgridy;
unsigned int padstate;
unsigned int oldpadstate;
uint8_t movemag;
char fx,fy;
uint8_t xoff,yoff;

uint8_t field[MAXWIDTH/2][MAXHEIGHT];
char searchgrid[MAXWIDTH+2][MAXHEIGHT+2];
uint8_t fwidth, fheight;
uint8_t nummines;
uint8_t numflags;
char name[7]="PLAYER ";
unsigned short time;
//uint8_t lastx,lasty;

uint8_t flags;
#define LOST		0b00000001
#define WON			0b00000010
#define STARTED		0b00000100
#define SOUND		0b00001000
#define MARKS		0b00010000
#define GUIOPEN		0b00100000
#define BIGFIELD	0b01000000
#define DEBUG		0b10000000

uint8_t skill=0;
uint8_t music=1;
uint8_t customwidth, customheight, custommines;

uint8_t fracs;
int seconds;

void InitField();
inline void SetGrid(uint8_t x, uint8_t y, uint8_t g);
inline uint8_t GetGrid(uint8_t x, uint8_t y);
void SetVidGrid(uint8_t x, uint8_t y, uint8_t g);
uint8_t GetVidGrid(uint8_t x, uint8_t y);


uint16_t prand();

void Input();
void MainLoop();
void NewGame();
void CustomMenu();
void Gui();
void DrawFace(uint8_t x, uint8_t y, uint8_t override);
void DrawTimer();
void DrawMineCount();
void DrawMenu(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void DrawCursor();
void MainMenu();
void InGameMenu();
void ScoreMenu();
void AboutMenu();
void HelpMenu();
void VictoryMenu();
void Win();
void Loss();
void FloodFill(uint8_t xs, uint8_t ys);
void ResetBoard();
bool CheckVictory();
void Draw();

void SaveHighScore(uint8_t slot, unsigned short time, char *name);
void LoadHighScore(uint8_t slot);
extern bool snesMouseEnabled;

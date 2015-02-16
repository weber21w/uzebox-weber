#define RTLFONTSKIPBG 1
#define RTLFONTSKIPFG 2

#define RTLRED 16
#define RTLGREEN 32
#define RTLBLUE 64

#ifdef RTL_FOREGROUND
	#ifndef RTL_FOREGROUND_COLOR_KEY
	#define RTL_FOREGROUND_COLOR_KEY 0xFE
	#endif
#endif

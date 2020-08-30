/* mgr menus for se */

#include "term.h"

char item[30] = "save =>\0";

struct menu_entry menu[] = {
   "play =>","\rP",
   "scale =>","\rs",
   "cut","\rc",
   "paste=>","\rp",
	 item,"\rf",
	"fetch", "\rg",
   "debug","\rd",
	"filter=>","\rX",
	"hist=>","\rF",
	"iconify","\rII\r",
   "quit","\rq\r",
	NULL, NULL
   };

struct menu_entry scale_menu[] = {
   "scale","",
   "1"," 1",
   "2"," 2",
   "3"," 3",
   "10"," 10",
   "30"," 30",
   "100"," 100",
   "300"," 300",
   "1000"," 1000",
   "3000"," 3000",
   "10000"," 10000",
   };

struct menu_entry paste_menu[] = {
	"insert", " 1",
	"sum", " 2",
	};

struct menu_entry filter_menu[] = {
	"Silence", " 1",
	"Fade up", " 2",
	"Fade down", " 3",
	"Up 3db",	" 4",
	"Down 3db",  " 5",
	};

struct menu_entry file_menu[] = {
	"whole file", " 1",
	"cut buffer", " 2",
	"mark",       " 3",
	"mark (append)", " 4",
	};

struct menu_entry play_menu[] = {
	"mark",       " 3",
	"cut buffer", " 2",
	"whole file", " 1",
	};

struct menu_entry icon_menu[] = {
	"activate",		"\rIA\r",
	"quit",			"\rq\r",
	};

struct menu_entry fft_menu[] = {
	"   FFT    ",	"",
	"256 point",	" 8",
	"512 point",	" 9",
	"1024 point",	" 10",
	"2048 point",	" 11",
	"4096 point",	" 12",
	"8192 point",	" 13",
	};

/* Catacomb Apocalypse Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// C3_MAIN.C

#define CATALOG

#include <time.h>
#include <stdarg.h>

#include "DEF.H"
#include "GELIB.H"
#pragma hdrstop
#include <dir.h>

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

typedef enum demo_screens {sc_logo,sc_title,sc_credits1,sc_credits2,sc_credits3,sc_credits4,sc_end} demo_screens;
struct Shape   shape,
					SdLogoShp,
					TitleShp,
					CreditBKShp,
					Credit1Shp,
					Credit2Shp,
					Credit3Shp,
					Credit4Shp,
					Credit5Shp,
					Credit6Shp,
					Credit7Shp,
					Credit8Shp,
					Credit9Shp,
					Credit10Shp;


PresenterInfo MainHelpText;

GameDiff restartgame;
boolean loadedgame,abortgame,ingame;


memptr		scalesegs[NUMPICS];
char		str[80],str2[20];
unsigned	tedlevelnum;
boolean		tedlevel;
gametype	gamestate;
exittype	playstate;
char	SlowMode = 0;
int starting_level;

short NumGames=0;
unsigned Flags=0;

boolean LoadShapes = true;
boolean EASYMODEON = false;

void DisplayIntroText(void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/



//===========================================================================

#if 0
// JAB Hack begin
#define	MyInterrupt	0x60
void interrupt (*intaddr)();
void interrupt (*oldintaddr)();
	char	*JHParmStrings[] = {"no386",nil};

void
jabhack(void)
{
extern void far jabhack2(void);
extern int far	CheckIs386(void);

	int	i;

	oldintaddr = getvect(MyInterrupt);

	for (i = 1;i < _argc;i++)
		if (US_CheckParm(_argv[i],JHParmStrings) == 0)
			return;

	if (CheckIs386())
	{
		jabhack2();
		setvect(MyInterrupt,intaddr);
	}
}

void
jabunhack(void)
{
	setvect(MyInterrupt,oldintaddr);
}
//	JAB Hack end
#endif

//===========================================================================

/*
=====================
=
= NewGame
=
= Set up new game to start from the beginning
=
=====================
*/

void NewGame (void)
{
	if (!loadedgame)
	{
		memset (&gamestate,0,sizeof(gamestate));
		gamestate.mapon = starting_level;
		gamestate.body = MAXBODY;
	}

	BGFLAGS = BGF_NOT_LIGHTNING;
	Flags &= FL_CLEAR;

	boltsleft = bolttimer = 0;

//	memset (gamestate.levels,-1,sizeof(gamestate.levels));
}

//===========================================================================

#define RLETAG	0xABCD

/*
==================
=
= SaveTheGame
=
==================
*/

boolean	SaveTheGame(int file)
{
	word	i,compressed,expanded;
	objtype	*o;
	memptr	bigbuffer;

	// save the sky and ground colors
	if (!CA_FarWrite(file,(void far *)&skycolor,sizeof(skycolor)))
		return(false);
	if (!CA_FarWrite(file,(void far *)&groundcolor,sizeof(groundcolor)))
		return(false);

	if (!CA_FarWrite(file,(void far *)&FreezeTime,sizeof(FreezeTime)))
		return(false);

	if (!CA_FarWrite(file,(void far *)&gamestate,sizeof(gamestate)))
		return(false);

	if (!CA_FarWrite(file,(void far *)&EASYMODEON,sizeof(EASYMODEON)))
		return(false);

	expanded = mapwidth * mapheight * 2;
	MM_GetPtr (&bigbuffer,expanded);

	for (i = 0;i < 3;i+=2)	// Write planes 0 and 2
	{
//
// leave a word at start of compressed data for compressed length
//
		compressed = (unsigned)CA_RLEWCompress ((unsigned huge *)mapsegs[i]
			,expanded,((unsigned huge *)bigbuffer)+1,RLETAG);

		*(unsigned huge *)bigbuffer = compressed;

		if (!CA_FarWrite(file,(void far *)bigbuffer,compressed+2) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}
	}

	for (o = player;o;o = o->next)
		if (!CA_FarWrite(file,(void far *)o,sizeof(objtype)))
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}

	MM_FreePtr (&bigbuffer);

	return(true);
}

//===========================================================================


/*
==================
=
= LoadTheGame
=
==================
*/

boolean	LoadTheGame(int file)
{
	unsigned	i,x,y;
	objtype		*obj,*prev,*next,*followed;
	unsigned	compressed,expanded;
	unsigned	far *map,tile;
	memptr		bigbuffer;

	screenpage = 0;
	FreeUpMemory();

	playstate = ex_loadedgame;
	// load the sky and ground colors
	if (!CA_FarRead(file,(void far *)&skycolor,sizeof(skycolor)))
		return(false);
	if (!CA_FarRead(file,(void far *)&groundcolor,sizeof(groundcolor)))
		return(false);

	if (!CA_FarRead(file,(void far *)&FreezeTime,sizeof(FreezeTime)))
		return(false);

	if (!CA_FarRead(file,(void far *)&gamestate,sizeof(gamestate)))
		return(false);

	if (!CA_FarRead(file,(void far *)&EASYMODEON,sizeof(EASYMODEON)))
		return(false);

	SetupGameLevel ();		// load in and cache the base old level

	if (!FindFile(Filename,"SAVE GAME",-1))
		Quit("Error: Can't find saved game file!");

	expanded = mapwidth * mapheight * 2;
	MM_GetPtr (&bigbuffer,expanded);

	for (i = 0;i < 3;i+=2)	// Read planes 0 and 2
	{
		if (!CA_FarRead(file,(void far *)&compressed,sizeof(compressed)) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}

		if (!CA_FarRead(file,(void far *)bigbuffer,compressed) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}

		CA_RLEWexpand ((unsigned huge *)bigbuffer,
			(unsigned huge *)mapsegs[i],expanded,RLETAG);
	}

	MM_FreePtr (&bigbuffer);
//
// copy the wall data to a data segment array again, to handle doors and
// bomb walls that are allready opened
//
	memset (tilemap,0,sizeof(tilemap));
	memset (actorat,0,sizeof(actorat));
	map = mapsegs[0];
	for (y=0;y<mapheight;y++)
		for (x=0;x<mapwidth;x++)
		{
			tile = *map++;
			if (tile<NUMFLOORS)
			{
				if (tile != INVISIBLEWALL)
					tilemap[x][y] = tile;
				if (tile>0)
					(unsigned)actorat[x][y] = tile;
			}
		}


	// Read the object list back in - assumes at least one object in list

	InitObjList ();
	new = player;
	while (true)
	{
		prev = new->prev;
		next = new->next;
		if (!CA_FarRead(file,(void far *)new,sizeof(objtype)))
			return(false);
		followed = new->next;
		new->prev = prev;
		new->next = next;
		actorat[new->tilex][new->tiley] = new;	// drop a new marker

		if (followed)
			GetNewObj (false);
		else
			break;
	}

	return(true);
}

//===========================================================================

/*
==================
=
= ResetGame
=
==================
*/

void ResetGame(void)
{
	NewGame ();

	ca_levelnum--;
	ca_levelbit>>=1;
	CA_ClearMarks();
	ca_levelbit<<=1;
	ca_levelnum++;
}

//===========================================================================


/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
  US_Shutdown ();
#ifndef PROFILE
  SD_Shutdown ();
  IN_Shutdown ();
#endif
  VW_Shutdown ();
  CA_Shutdown ();
  MM_Shutdown ();
}


//===========================================================================

/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/

void InitGame (void)
{
	unsigned	segstart,seglength;
	int			i,x,y;
	unsigned	*blockstart;

//	US_TextScreen();

	MM_Startup ();
	VW_Startup ();
#ifndef PROFILE
	IN_Startup ();
	SD_Startup ();
#endif
	US_Startup ();

	CA_Startup ();

	US_Setup ();

	US_SetLoadSaveHooks(LoadTheGame,SaveTheGame,ResetGame);


//
// load in and lock down some basic chunks
//

	CA_ClearMarks ();

	CA_MarkGrChunk(STARTFONT);
	CA_MarkGrChunk(STARTTILE8);
	CA_MarkGrChunk(STARTTILE8M);
	CA_MarkGrChunk(HAND1PICM);

	CA_MarkGrChunk(NORTHICONSPR);
	CA_CacheMarks (NULL);

	MM_SetLock (&grsegs[STARTFONT],true);
	MM_SetLock (&grsegs[STARTTILE8],true);
	MM_SetLock (&grsegs[STARTTILE8M],true);
	MM_SetLock (&grsegs[HAND1PICM],true);

	fontcolor = WHITE;


//
// build some tables
//
	for (i=0;i<MAPSIZE;i++)
		nearmapylookup[i] = &tilemap[0][0]+MAPSIZE*i;

	for (i=0;i<PORTTILESHIGH;i++)
		uwidthtable[i] = UPDATEWIDE*i;

	blockstart = &blockstarts[0];
	for (y=0;y<UPDATEHIGH;y++)
		for (x=0;x<UPDATEWIDE;x++)
			*blockstart++ = SCREENWIDTH*16*y+x*TILEWIDTH;

	BuildTables ();			// 3-d tables

	SetupScaling ();

#ifndef PROFILE
//	US_FinishTextScreen();
#endif

#if 0
//
// reclaim the memory from the linked in text screen
//
	segstart = FP_SEG(&introscn);
	seglength = 4000/16;
	if (FP_OFF(&introscn))
	{
		segstart++;
		seglength--;
	}
	MML_UseSpace (segstart,seglength);
#endif

	VW_SetScreenMode (GRMODE);
	ge_textmode = false;
//	VW_ColorBorder (3);
	VW_ClearVideo (BLACK);

//
// initialize variables
//
	updateptr = &update[0];
	*(unsigned *)(updateptr + UPDATEWIDE*PORTTILESHIGH) = UPDATETERMINATE;
	bufferofs = 0;
	displayofs = 0;
	VW_SetLineWidth(SCREENWIDTH);
}

//===========================================================================

void clrscr (void);		// can't include CONIO.H because of name conflicts...

/*
==========================
=
= Quit
=
==========================
*/

void Quit (char *error, ...)
{
	short exit_code=0;
	unsigned	finscreen;

	va_list ap;

	va_start(ap,error);

#ifndef CATALOG
	if (!error)
	{
		CA_SetAllPurge ();
		CA_CacheGrChunk (PIRACY);
		finscreen = (unsigned)grsegs[PIRACY];
	}
#endif

	ShutdownId ();

	if (error && *error)
	{
		vprintf(error,ap);
		exit_code = 1;
	}
#ifndef CATALOG
	else
	if (!NoWait)
	{
		movedata (finscreen,0,0xb800,0,4000);
		bioskey (0);
	}
#endif

	va_end(ap);

#ifndef CATALOG
	if (!error)
	{
		_argc = 2;
		_argv[1] = "LAST.SHL";
		_argv[2] = "ENDSCN.SCN";
		_argv[3] = NULL;
		if (execv("LOADSCN.EXE", _argv) == -1)
		{
			clrscr();
			puts("Couldn't find executable LOADSCN.EXE.\n");
			exit(1);
		}
	}
#endif

	exit(exit_code);
}

//===========================================================================

/*
==================
=
= TEDDeath
=
==================
*/

void	TEDDeath(void)
{
	ShutdownId();
	execlp("TED5.EXE","TED5.EXE","/LAUNCH",NULL);
}

//===========================================================================

/*
====================
=
= DisplayDepartment
=
====================
*/
void DisplayDepartment(char *text)
{
	short temp;

//	bufferofs = 0;
	PrintY = 5;
	WindowX = 17;
	WindowW = 168;

	VW_Bar(WindowX,PrintY+1,WindowW,7,0);
	temp = fontcolor;
	fontcolor = 10;
	US_CPrintLine (text);
	fontcolor = temp;
}



/*
=====================
=
= DemoLoop
=
=====================
*/

static	char *ParmStrings[] = {"easy","normal","hard",""};

void	DemoLoop (void)
{

/////////////////////////////////////////////////////////////////////////////
// main game cycle
/////////////////////////////////////////////////////////////////////////////

	displayofs = bufferofs = 0;
	VW_Bar (0,0,320,200,0);
	VW_SetScreen(0,0);

//
// Read in all the graphic images needed for the title sequence
//
		VW_WaitVBL(1);
		IN_ReadControl(0,&control);

//	set EASYMODE
//
	if (stricmp(_argv[2], "1") == 0)
		EASYMODEON = true;
	else
		EASYMODEON = false;

// restore game
//
	if (stricmp(_argv[3], "1") == 0)
	{
		VW_FadeOut();
		bufferofs = displayofs = 0;
		VW_Bar(0,0,320,200,0);
		if (GE_LoadGame())
		{
			loadedgame = true;
			playstate = ex_loadedgame;
			Keyboard[sc_Enter] = true;
			VW_Bar(0,0,320,200,0);
			ColoredPalette();
		}
		VW_Bar(0,0,320,200,0);
		VW_FadeIn();
	}

	// Play a game
	//
		restartgame = gd_Normal;
		NewGame();
		GameLoop();
}

//-------------------------------------------------------------------------
// DisplayIntroText()
//-------------------------------------------------------------------------
void DisplayIntroText()
{
	PresenterInfo pi;

#ifdef TEXT_PRESENTER
	char *toptext = "You stand before the gate leading into the Towne "
						 "of Morbidity.... "
						 "^XX";

	char *bottomtext = "Enter now boldly to defeat the evil Nemesis "
							 "deep inside the catacombs."
							 "
							 "^XX";
#endif

	char oldfontcolor=fontcolor;

	fontcolor = 14;


#ifdef TEXT_PRESENTER
	pi.xl = 0;
	pi.yl = 0;
	pi.xh = 319;
	pi.yh = 1;
	pi.bgcolor = 0;
	pi.script[0] = (char far *)toptext;
	Presenter(&pi);

	pi.yl = 160;
	pi.yh = 161;
	pi.script[0] = (char far *)bottomtext;
	Presenter(&pi);

#else
	PrintY = 1;
	PrintX = 0;
	WindowX = 0;
	WindowW = 320;


	US_Print ("      A chilling wind greets you at the entrance\n");
	US_Print ("            to the Sanctuary of the Dead.\n");

	PrintY = 180;

	fontcolor = 9;
	US_Print ("                   Shall you proceed as\n");
	fontcolor = 14;
	US_Print ("                  N");
	fontcolor = 9;
	US_Print ("ovice   or");
	fontcolor = 14;
	US_Print ("   W");
	fontcolor = 9;
	US_Print ("arrior ?");

#endif

	fontcolor = oldfontcolor;
}

#if 0
boolean ChooseGameLevel()
{
	char choices[] = {sc_Escape,sc_E,sc_N,sc_H,0},ch;

	CenterWindow(20,10);

	US_Print("\n   Choose difficulty level:\n");
	US_Print("       (E)asy\n");
	US_Print("       (N)ormal\n");
	US_Print("       (H)ard\n");
	US_Print("\n      (ESC)ape aborts\n");

//	VW_UpdateScreen();
	if ((ch=GetKeyChoice(choices)) == sc_Escape)
	{
		while (Keyboard[sc_Escape]);
		LastScan = 0;
		return(false);
	}

	if (ch == sc_E)
		restartgame = gd_Easy;
	else
	if (ch == sc_N)
		restartgame = gd_Normal;
	else
		restartgame = gd_Hard;

	return(true);
}
#endif


//===========================================================================

/*
==========================
=
= SetupScalePic
=
==========================
*/

void SetupScalePic (unsigned picnum)
{
	unsigned	scnum;

	if (picnum == 1)
		return;

	scnum = picnum-FIRSTSCALEPIC;

	if (shapedirectory[scnum])
	{
		MM_SetPurge (&(memptr)shapedirectory[scnum],0);
		return;					// allready in memory
	}

	CA_CacheGrChunk (picnum);
	DeplanePic (picnum);
	shapesize[scnum] = BuildCompShape (&shapedirectory[scnum]);
	grneeded[picnum]&= ~ca_levelbit;
	MM_FreePtr (&grsegs[picnum]);
}

//===========================================================================

/*
==========================
=
= SetupScaleWall
=
==========================
*/

void SetupScaleWall (unsigned picnum)
{
	int		x,y;
	unsigned	scnum;
	byte	far *dest;

	if (picnum == 1)
		return;

	scnum = picnum-FIRSTWALLPIC;

	if (walldirectory[scnum])
	{
		MM_SetPurge (&walldirectory[scnum],0);
		return;					// allready in memory
	}

	CA_CacheGrChunk (picnum);
	DeplanePic (picnum);
	MM_GetPtr(&walldirectory[scnum],64*64);
	dest = (byte far *)walldirectory[scnum];
	for (x=0;x<64;x++)
		for (y=0;y<64;y++)
			*dest++ = spotvis[y][x];
	grneeded[picnum]&= ~ca_levelbit;
	MM_FreePtr (&grsegs[picnum]);
}

//===========================================================================

/*
==========================
=
= SetupScaling
=
==========================
*/

void SetupScaling (void)
{
	int		i,x,y;
	byte	far *dest;

//
// build the compiled scalers
//
	for (i=1;i<=VIEWWIDTH/2;i++)
		BuildCompScale (i*2,&scaledirectory[i]);
}

//===========================================================================

int	showscorebox;

void RF_FixOfs (void)
{
}

void HelpScreens (void)
{
}


#if 0
/*
==================
=
= CheckMemory
=
==================
*/

#define MINMEMORY	400000l

void	CheckMemory(void)
{
	unsigned	finscreen;

	if (Flags & FL_NOMEMCHECK)
		return;

	if (mminfo.nearheap+mminfo.farheap+mminfo.EMSmem+mminfo.XMSmem
		>= MINMEMORY)
		return;

	CA_CacheGrChunk (OUTOFMEM);
	finscreen = (unsigned)grsegs[OUTOFMEM];
	ShutdownId ();
	movedata (finscreen,7,0xb800,0,4000);
	gotoxy (1,24);
	exit(1);
}
#endif

//===========================================================================


/*
==========================
=
= main
=
==========================
*/

char			*MainParmStrings[] = {"q","l","ver","nomemcheck","helptest",nil};

void main (void)
{
	short i;

	starting_level = 0;

	for (i = 1;i < _argc;i++)
	{
		switch (US_CheckParm(_argv[i],MainParmStrings))
		{
			case 0:
				Flags |= FL_QUICK;
			break;

			case 1:
				starting_level = atoi(_argv[i]+1);
				if ((starting_level < 0) || (starting_level > LASTMAP-1))
					starting_level = 0;
			break;

			case 2:
				printf("%s\n", GAMENAME);
				printf("Copyright 1992-93 Softdisk Publishing\n");
				printf("%s %s\n",VERSION,REVISION);
				printf("\n");
				exit(0);
			break;

			case 3:
				Flags |= FL_NOMEMCHECK;
			break;

			case 4:
				Flags |= (FL_HELPTEST|FL_QUICK);
			break;

		}
	}

	if (stricmp(_argv[1], "^(a@&r`"))
		Quit("You must type CATAPOC to run CATACOMB APOCALYPSE\n");


#if 0
	MainHelpText.xl = 0;
	MainHelpText.yl = 0;
	MainHelpText.xh = 639;
	MainHelpText.yh = 199;
	MainHelpText.bgcolor = 7;
	MainHelpText.ltcolor = 15;
	MainHelpText.dkcolor = 8;
#endif

//	jabhack();

	randomize();

	InitGame ();
//	CheckMemory ();
	LoadLatchMem ();

//	if (!LoadTextFile("MAINHELP."EXT,&MainHelpText))
//		Quit("Can't load MAINHELP."EXT);

#ifdef PROFILE
	NewGame ();
	GameLoop ();
#endif

	DemoLoop();
	Quit(NULL);
}

//-------------------------------------------------------------------------
// Display640()
//-------------------------------------------------------------------------
void Display640(void)
{
// Can you believe it takes all this just to change to 640 mode!!???!
//
	VW_ScreenToScreen(0,FREESTART-STATUSLEN,40,80);
	VW_SetLineWidth(80);
	MoveScreen(0,0);
	VW_Bar (0,0,640,200,0);
	VW_SetScreenMode(EGA640GR);
	VW_SetLineWidth(80);
	BlackPalette();
}

//-------------------------------------------------------------------------
// Display320()
//-------------------------------------------------------------------------
void Display320(void)
{
// Can you believe it takes all this just to change to 320 mode!!???!
//
	VW_ColorBorder(0);
	VW_FadeOut();
	VW_SetLineWidth(40);
	MoveScreen(0,0);
	VW_Bar (0,0,320,200,0);
	VW_SetScreenMode(EGA320GR);
	VW_SetLineWidth(40);
	BlackPalette();
	VW_ScreenToScreen(FREESTART-STATUSLEN,0,40,80);
}

void PrintHelp(void)
{
	char oldfontcolor = fontcolor;
	PrintY = 1;
	WindowX = 135;
	WindowW = 640;

	VW_FadeOut();
	bufferofs = displayofs = screenloc[0];
	VW_Bar(0,0,320,200,0);

	Display640();

	VW_Bar(0, 0, 640, 200, 7);

	fontcolor = (7 ^ 1);
	US_Print ("\n\n                    SUMMARY OF GAME CONTROLS\n\n");

	fontcolor = (7 ^ 4);
	US_Print ("         ACTION\n\n");

	US_Print ("Arrow keys, joystick, or mouse\n");
	US_Print ("TAB or V while turning\n");
	US_Print ("ALT or Button 2 while turning\n");
	US_Print ("CTRL or Button 1\n");
	US_Print ("Z\n");
	US_Print ("X or Enter\n");
	US_Print ("F1\n");
	US_Print ("F2\n");
	US_Print ("F3\n");
	US_Print ("F4\n");
	US_Print ("F5\n");
	US_Print ("ESC\n\n");
	fontcolor = (7 ^ 0);
#ifndef CATALOG
	US_Print ("          (See complete Instructions for more info)\n");
#endif
	US_Print ("\n           copyright (c) 1992-93 Softdisk Publishing\n");

	fontcolor = (7 ^ 8);
	PrintX = 400;
	PrintY = 37;
	WindowX = 400;
	US_Print ("   REACTION\n\n");
	US_Print ("Move and turn\n");
	US_Print ("Turn quickly (Quick Turn)\n");
	US_Print ("Move sideways\n");
	US_Print ("Shoot a Missile\n");
	US_Print ("Shoot a Zapper\n");
	US_Print ("Shoot an Xterminator\n");
	US_Print ("Help (this screen)\n");
	US_Print ("Sound control\n");
	US_Print ("Save game position\n");
	US_Print ("Restore a saved game\n");
	US_Print ("Joystick control\n");
	US_Print ("System options\n\n\n");

	VW_UpdateScreen();
	VW_FadeIn();
	VW_ColorBorder(8 | 56);
	IN_Ack();
	Display320();
	fontcolor = oldfontcolor;
}
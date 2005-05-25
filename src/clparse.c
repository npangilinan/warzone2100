/*
 * clParse.c
 *
 * Parse command line arguments
 *
 */

#ifdef WIN32
#include <direct.h>
#endif

#include "frame.h"
#include "widget.h"

#include "winmain.h"
#include "frontend.h"

#include "pieclip.h"
#include "warzoneconfig.h"
#include "configuration.h"

#include "clparse.h"
#include "piestate.h"
#include "loadsave.h"
#include "objects.h"
#include "advvis.h"
#include "multiplay.h"
#include "multiint.h"
#include "netplay.h"
#include "wrappers.h"
#include "cheat.h"

BOOL scanGameSpyFlags(LPSTR gflag,LPSTR value);

extern char	SaveGamePath[];

// whether to start windowed
BOOL	clStartWindowed;
// whether to play the intro video
BOOL	clIntroVideo;

// let the end user into debug mode....
BOOL	bAllowDebugMode = FALSE;


// note that render mode must come before resolution flag.
BOOL ParseCommandLine(int argc, char** argv)
{
	char			seps[] = " ,\t\n";
	char			*tokenType;
	char			*token;
	BOOL			bCrippleD3D = FALSE; // Disable higher resolutions for d3D
	char			seps2[] ="\"";
	char			cl[255];
	char			cl2[255];
	unsigned char		*pXor;
	unsigned int		w, h;
	int i;

	// for cheating
	sprintf(cl,"%s","VR^\\WZ^KVQXL\\^SSFH^XXZM");
	pXor = xorString(cl);
	sprintf(cl2,"%s%s","-",cl);

	/* loop through command line */
	for( i = 1; i < argc; ++i) {
		tokenType = argv[i];

		if ( stricmp( tokenType, "-window" ) == 0 )
		{
#ifdef	_DEBUG
			clStartWindowed = TRUE;
#else
#ifndef COVERMOUNT
			clStartWindowed = TRUE;
#endif
#endif
		}
		else if ( stricmp( tokenType, "-fullscreen" ) == 0 )
		{
			clStartWindowed = FALSE;
		}
		else if ( stricmp( tokenType, "-intro" ) == 0 )
		{
			SetGameMode(GS_VIDEO_MODE);
		}
		else if ( stricmp( tokenType, "-software" ) == 0 )
		{
			war_SetRendMode(REND_MODE_SOFTWARE);
			war_SetFog(FALSE);
			pie_SetVideoBuffer(640, 480);

		}
		else if ( stricmp( tokenType, "-D3D" ) == 0 )
		{
			war_SetRendMode(REND_MODE_HAL);
			pie_SetDirect3DDeviceName("Direct3D HAL");
//			bCrippleD3D = TRUE;
			pie_SetVideoBuffer(640, 480);
		}

		else if ( stricmp( tokenType, "-MMX" ) == 0 )
		{
#ifndef COVERMOUNT
			war_SetRendMode(REND_MODE_RGB);
			pie_SetDirect3DDeviceName("RGB Emulation");
			pie_SetVideoBuffer(640, 480);
#else
			war_SetRendMode(REND_MODE_SOFTWARE);
			war_SetFog(FALSE);
#endif
		}
		else if ( stricmp( tokenType, "-RGB" ) == 0 )
		{
#ifndef COVERMOUNT
			war_SetRendMode(REND_MODE_RGB);
			pie_SetDirect3DDeviceName("RGB Emulation");
			pie_SetVideoBuffer(640, 480);
#else
			war_SetRendMode(REND_MODE_SOFTWARE);
			war_SetFog(FALSE);
#endif
		}
		else if ( stricmp( tokenType, "-REF" ) == 0 )
		{
#ifndef COVERMOUNT
			war_SetRendMode(REND_MODE_REF);
			pie_SetDirect3DDeviceName("Reference Rasterizer");
			pie_SetVideoBuffer(640, 480);
#else
			war_SetRendMode(REND_MODE_SOFTWARE);
			war_SetFog(FALSE);
#endif
		}
		else if ( stricmp( tokenType, "-title" ) == 0 )
		{
			SetGameMode(GS_TITLE_SCREEN);
		}
#ifndef NON_INTERACT
		else if ( stricmp( tokenType, "-game" ) == 0 )
		{
			// find the game name
			token = argv[++i];
			if (token == NULL)
			{
				DBERROR( ("Unrecognised -game name\n") );
				return FALSE;
			}
			strncpy(pLevelName, token, 254);
			SetGameMode(GS_NORMAL);
		}
		else if ( stricmp( tokenType, "-savegame" ) == 0 )
		{
			// find the game name
			token = argv[++i];
			if (token == NULL)
			{
				DBERROR( ("Unrecognised -savegame name\n") );
				return FALSE;
			}
			strcpy(saveGameName, SaveGamePath);
			strncat(saveGameName, token, 240);
			SetGameMode(GS_SAVEGAMELOAD);
		}
#endif
		else if ( stricmp( tokenType, "-datapath" ) == 0 )
		{
			// find the quoted path name
			token = argv[++i];
			if (token == NULL)
			{
				DBERROR( ("Unrecognised datapath\n") );
				return FALSE;
			}
			resSetBaseDir(token);

			if (_chdir(token) != 0)
			{
				DBERROR(("Path not found: %s\n", token));
			}
		}

//#ifndef COVERMOUNT
		else if( stricmp( tokenType,cl2) == 0)
		{
			bAllowDebugMode =TRUE;
		}
//#endif
		else if( sscanf(tokenType,"-%ix%i", &w, &h) == 2)
		{
			pie_SetVideoBuffer(w, h);
		}
		else if( stricmp( tokenType,"-noTranslucent") == 0)
		{
			war_SetTranslucent(FALSE);
		}
		else if( stricmp( tokenType,"-noAdditive") == 0)
		{
			war_SetAdditive(FALSE);
		}
		else if( stricmp( tokenType,"-noFog") == 0)
		{
			pie_SetFogCap(FOG_CAP_NO);
		}
		else if( stricmp( tokenType,"-greyFog") == 0)
		{
			pie_SetFogCap(FOG_CAP_GREY);
		}
		else if (stricmp(tokenType, "-CDA") == 0) {
			playAudioCDs = TRUE;
		}
		else if (stricmp(tokenType, "-noCDA") == 0) {
			playAudioCDs = FALSE;
		}
		else if( stricmp( tokenType,"-2meg") == 0)
		{
			pie_SetTexCap(TEX_CAP_2M);
		}
		else if( stricmp( tokenType,"-seqSmall") == 0)
		{
			war_SetSeqMode(SEQ_SMALL);
		}
		else if( stricmp( tokenType,"-seqSkip") == 0)
		{
			war_SetSeqMode(SEQ_SKIP);
		}
		else if( stricmp( tokenType,"-disableLobby") == 0)
		{
			bDisableLobby = TRUE;
		}
/*		else if ( stricmp( tokenType,"-routeLimit") == 0)
		{
			// find the actual maximum routing limit
			token = strtok( NULL, seps );
			if (token == NULL)
			{
				DBERROR( ("Unrecognised -routeLimit value\n") );
				return FALSE;
			}

			if (!openWarzoneKey())
			{
				DBERROR(("Couldn't open registry for -routeLimit"));
				return FALSE;
			}
			fpathSetMaxRoute(atoi(token));
			setWarzoneKeyNumeric("maxRoute",(DWORD)(fpathGetMaxRoute()));
			closeWarzoneKey();
		}*/

	// gamespy flags
		else if (   stricmp(tokenType, "+host") == 0		// host a multiplayer.
			 || stricmp(tokenType, "+connect") == 0
			 || stricmp(tokenType, "+name") == 0
			 || stricmp(tokenType, "+ip") == 0
			 || stricmp(tokenType, "+maxplayers") == 0
			 || stricmp(tokenType, "+hostname") == 0)
		{
			token = argv[++i];
			scanGameSpyFlags(tokenType, token);
		}	
	// end of gamespy

		else
		{
			// ignore (gamespy requirement)
		//	DBERROR( ("Unrecognised command-line token %s\n", tokenType) );
		//	return FALSE;
		}
	}
	
	/* Hack to disable higher resolution requests in d3d for the demo */
	if(bCrippleD3D)
	{
		pie_SetVideoBuffer(640, 480);
	}

	// look for any gamespy flags in the command line.
#ifdef NON_INTERACT
	SetGameMode(GS_NORMAL);
#endif

	return TRUE;
}





// ////////////////////////////////////////////////////////
// gamespy flags
BOOL scanGameSpyFlags(LPSTR gflag,LPSTR value)
{
	static UBYTE count = 0;
//	UDWORD val;
	LPVOID finalconnection;	 

//#ifdef COVERMOUNT
//	return TRUE;
//#endif


#if 0
	// check for gamespy flag...

	// if game spy not enabled....
	if(!openWarzoneKey())
		return FALSE;
	if(!getWarzoneKeyNumeric("allowGameSpy",&val))
	{
		return TRUE;
	}
	closeWarzoneKey();
#endif

	count++;
	if(count == 1)
	{
		lobbyInitialise();
		bDisableLobby	 = TRUE;				// dont reset everything on boot!
		gameSpy.bGameSpy = TRUE;
	}

	if(	 stricmp( gflag,"+host") == 0)			// host a multiplayer.
	{
		NetPlay.bHost = 1;
		game.bytesPerSec			= INETBYTESPERSEC;	
		game.packetsPerSec			= INETPACKETS;
		NETsetupTCPIP(&finalconnection,"");
		NETselectProtocol(finalconnection);
	}
	else if( stricmp( gflag,"+connect") == 0)	// join a multiplayer.
	{
		NetPlay.bHost = 0;
		game.bytesPerSec			= INETBYTESPERSEC;	
		game.packetsPerSec			= INETPACKETS;
		NETsetupTCPIP(&finalconnection,value);
		NETselectProtocol(finalconnection);
		// gflag is add to con to.
	}
	else if( stricmp( gflag,"+name") == 0)		// player name.
	{
		strcpy((char *)sPlayer,value);		
	}
	else if( stricmp( gflag,"+hostname") == 0)	// game name.
	{	
		strcpy(game.name,value);
	}

/*not used from here on..
+ip
+maxplayers
+game
+team
+skin
+playerid
+password
tokenType = strtok( NULL, seps );
*/
	return TRUE;
}


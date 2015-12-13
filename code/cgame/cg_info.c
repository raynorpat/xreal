/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2006 Robert Beckebans <trebor_7@users.sourceforge.net>

This file is part of XreaL source code.

XreaL source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

XreaL source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with XreaL source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

// cg_info.c -- display information while data is being loading
#include "cg_local.h"

/*
======================
CG_LoadingString
======================
*/
void CG_LoadingString(const char *s)
{
	Q_strncpyz(cg.infoScreenText, s, sizeof(cg.infoScreenText));

	trap_UpdateScreen();
}

/*
======================
CG_DrawProgressBar
======================
*/
static void CG_DrawProgressBar(void)
{
	int             x, y, w, status = 0;
	float           rectColor[4];

	w = 400 - SMALLCHAR_WIDTH;
	x = (SCREEN_WIDTH - w) / 2;
	y = 462;
	rectColor[0] = 0.0f;
	rectColor[1] = 0.0f;
	rectColor[2] = 0.0f;
	rectColor[3] = 0.4f;

	CG_FillRect(0, y, SCREEN_WIDTH, 18, rectColor);	// semi black progress bar

	rectColor[0] = 0.7f;
	rectColor[1] = 0.0f;
	rectColor[2] = 0.0f;
	rectColor[3] = 0.4f;

	if(cg.infoScreenText[0])
	{
		if(Q_strncmp(cg.infoScreenText, "sounds", sizeof(cg.infoScreenText)) == 0)
			status = 0;
		else if(Q_strncmp(cg.infoScreenText, "collision map", sizeof(cg.infoScreenText)) == 0)
			status = 1;
		else if(Q_strncmp(cg.infoScreenText, "graphics", sizeof(cg.infoScreenText)) == 0)
			status = 2;
		else if(Q_strncmp(cg.infoScreenText, "game media", sizeof(cg.infoScreenText)) == 0)
			status = 3;
		else if(Q_strncmp(cg.infoScreenText, "effects", sizeof(cg.infoScreenText)) == 0)
			status = 4;
		else if(Q_strncmp(cg.infoScreenText, "clients", sizeof(cg.infoScreenText)) == 0)
			status = 5;
	}
	else
	{
		status = 6;				// we are awaiting the snapshot
	}

	// draw the red progress bar
	switch (status)
	{
		default:
		case 0:
			CG_FillRect(0, y, SCREEN_WIDTH - 500, 18, rectColor);
			break;

		case 1:
			CG_FillRect(0, y, SCREEN_WIDTH - 425, 18, rectColor);
			break;

		case 2:
			CG_FillRect(0, y, SCREEN_WIDTH - 325, 18, rectColor);
			break;

		case 3:
			CG_FillRect(0, y, SCREEN_WIDTH - 235, 18, rectColor);
			break;

		case 4:
			CG_FillRect(0, y, SCREEN_WIDTH - 150, 18, rectColor);
			break;

		case 5:
			CG_FillRect(0, y, SCREEN_WIDTH - 75, 18, rectColor);
			break;

		case 6:
			CG_FillRect(0, y, SCREEN_WIDTH, 18, rectColor);
			break;
	}
}

/*
====================
CG_DrawInformation

Draw all the status / pacifier stuff during level loading
====================
*/
void CG_DrawInformation(void)
{
	const char     *s = NULL;
	const char     *info;
	const char     *sysInfo;
	int             x, y, w;
	int             value;
	qhandle_t       levelshot;
	qhandle_t       detail;
	char            buf[1024];
	char            st[1024];

	info = CG_ConfigString(CS_SERVERINFO);
	sysInfo = CG_ConfigString(CS_SYSTEMINFO);

	s = Info_ValueForKey(info, "mapname");
	levelshot = trap_R_RegisterShaderNoMip(va("levelshots/%s.tga", s));
	if(!levelshot)
		levelshot = trap_R_RegisterShaderNoMip("menu/art/unknownmap");

	trap_R_SetColor(NULL);
	CG_DrawPic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, levelshot);

	// blend a detail texture over it
	detail = trap_R_RegisterShader("levelShotDetail");
	trap_R_DrawStretchPic(0, 0, cgs.glconfig.vidWidth, cgs.glconfig.vidHeight, 0, 0, 2.5, 2, detail);

	// draw the progress bar
	CG_DrawProgressBar();

	// the first 150 rows are reserved for the client connection
	// screen to write into
	if(cg.infoScreenText[0])
	{
		Com_sprintf(st, sizeof(st), "Loading... %s", cg.infoScreenText);
		w = CG_DrawStrlen(st) * SMALLCHAR_WIDTH;
		x = (SCREEN_WIDTH - w) / 2;
		y = 463;

		CG_DrawStringExt(x, y, st, colorWhite, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 200);
	}
	else
	{
		Com_sprintf(st, sizeof(st), "Awaiting snapshot...");
		w = CG_DrawStrlen(st) * SMALLCHAR_WIDTH;
		x = (SCREEN_WIDTH - w) / 2;
		y = 463;

		CG_DrawStringExt(x, y, st, colorWhite, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 200);
	}

	// draw info string information
	y = 180 - 32;

	// don't print server lines if playing a local game
	trap_Cvar_VariableStringBuffer("sv_running", buf, sizeof(buf));
	if(!atoi(buf))
	{
		// server hostname
		Q_strncpyz(buf, Info_ValueForKey(info, "sv_hostname"), 1024);
		Q_CleanStr(buf);
		UI_DrawProportionalString(320, y, buf, UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorWhite);
		y += PROP_HEIGHT;

		// pure server
		s = Info_ValueForKey(sysInfo, "sv_pure");
		if(s[0] == '1')
		{
			UI_DrawProportionalString(320, y, "Pure Server", UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorWhite);
			y += PROP_HEIGHT;
		}

		// server-specific message of the day
		s = CG_ConfigString(CS_MOTD);
		if(s[0])
		{
			UI_DrawProportionalString(320, y, s, UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorWhite);
			y += PROP_HEIGHT;
		}

		// some extra space after hostname and motd
		y += 10;
	}

	// map-specific message (long map name)
	s = CG_ConfigString(CS_MESSAGE);
	if(s[0])
	{
		UI_DrawProportionalString(320, y, s, UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorWhite);
		y += PROP_HEIGHT;
	}

	// cheats warning
	s = Info_ValueForKey(sysInfo, "sv_cheats");
	if(s[0] == '1')
	{
		UI_DrawProportionalString(320, y, "CHEATS ARE ENABLED", UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorRed);
		y += PROP_HEIGHT;
	}

	// game type
	switch (cgs.gametype)
	{
		case GT_FFA:
			s = "Free For All";
			break;
		case GT_SINGLE_PLAYER:
			s = "Single Player";
			break;
		case GT_TOURNAMENT:
			s = "Tournament";
			break;
		case GT_TEAM:
			s = "Team Deathmatch";
			break;
		case GT_CTF:
			s = "Capture The Flag";
			break;
#ifdef MISSIONPACK
		case GT_1FCTF:
			s = "One Flag CTF";
			break;
		case GT_OBELISK:
			s = "Overload";
			break;
		case GT_HARVESTER:
			s = "Harvester";
			break;
#endif
		default:
			s = "Unknown Gametype";
			break;
	}
	UI_DrawProportionalString(320, y, s, UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorWhite);
	y += PROP_HEIGHT / PROP_SMALL_SIZE_SCALE;

	value = atoi(Info_ValueForKey(info, "timelimit"));
	if(value)
	{
		UI_DrawProportionalString(320, y, va("timelimit %i", value), UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorWhite);
		y += PROP_HEIGHT;
	}

	if(cgs.gametype < GT_CTF)
	{
		value = atoi(Info_ValueForKey(info, "fraglimit"));
		if(value)
		{
			UI_DrawProportionalString(320, y, va("fraglimit %i", value), UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorWhite);
			y += PROP_HEIGHT;
		}
	}
	else if(cgs.gametype >= GT_CTF)
	{
		value = atoi(Info_ValueForKey(info, "capturelimit"));
		if(value)
		{
			UI_DrawProportionalString(320, y, va("capturelimit %i", value), UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, colorWhite);
			y += PROP_HEIGHT;
		}
	}
}

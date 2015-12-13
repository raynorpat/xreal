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

// cg_scoreboard -- draw the scoreboard on top of the game screen
#include "cg_local.h"

#define	SCOREBOARD_X		(0)

#define SB_HEADER			86
#define SB_TOP				(SB_HEADER+32)

// Where the status bar starts, so we don't overwrite it
#define SB_STATUSBAR		420

#define SB_NORMAL_HEIGHT	40
#define SB_INTER_HEIGHT		16	// interleaved height

#define SB_MAXCLIENTS_NORMAL  ((SB_STATUSBAR - SB_TOP) / SB_NORMAL_HEIGHT)
#define SB_MAXCLIENTS_INTER   ((SB_STATUSBAR - SB_TOP) / SB_INTER_HEIGHT - 1)

#define SB_HEAD_X			(SCOREBOARD_X+112)

#define SB_SCORELINE_X		112

#define SB_RATING_WIDTH	    (6 * BIGCHAR_WIDTH)	// width 6
#define SB_SCORE_X			(SB_SCORELINE_X + BIGCHAR_WIDTH)	// width 6
#define SB_RATING_X			(SB_SCORELINE_X + 6 * BIGCHAR_WIDTH)	// width 6
#define SB_PING_X			(SB_SCORELINE_X + 12 * BIGCHAR_WIDTH + 8)	// width 5
#define SB_TIME_X			(SB_SCORELINE_X + 17 * BIGCHAR_WIDTH + 8)	// width 5
#define SB_NAME_X			(SB_SCORELINE_X + 22 * BIGCHAR_WIDTH)	// width 15

char           *monthStr2[12] = {
	"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

// The new and improved score board

// In cases where the number of clients is high, the score board heads are interleaved
// here's the layout

//  112    144   240   320   400   <-- pixel position
// head  score  ping  time  name

/*
=================
CG_DrawClientScore
=================
*/
static void CG_DrawClientScore(int y, score_t * score, float *color, float fade, qboolean largeFormat)
{
	char            string[1024];
	vec3_t          headAngles;
	clientInfo_t   *ci;
	int             headx;

	if(score->client < 0 || score->client >= cgs.maxclients)
	{
		Com_Printf("Bad score->client: %i\n", score->client);
		return;
	}

	ci = &cgs.clientinfo[score->client];

	// highlight your position
	if(score->client == cg.snap->ps.clientNum)
	{
		float           hcolor[4];

		hcolor[0] = 0.7f;
		hcolor[1] = 0.7f;
		hcolor[2] = 0.7f;
		hcolor[3] = fade * 0.7f;
		CG_FillRect(SB_SCORELINE_X + 40, y, SB_SCORELINE_X * 4, BIGCHAR_HEIGHT + 5, hcolor);
	}

	// draw the players head
	headx = SB_HEAD_X + (SB_RATING_WIDTH / 2) - 5;

	VectorClear(headAngles);
	headAngles[YAW] = 90 * (cg.time / 1000.0) + 30;

	if(largeFormat)
		CG_DrawHead(headx, y, 24, 24, score->client, headAngles);
	else
		CG_DrawHead(headx, y, 16, 16, score->client, headAngles);

#ifdef MISSIONPACK
	// draw the team task
	if(ci->teamTask != TEAMTASK_NONE)
	{
		if(ci->teamTask == TEAMTASK_OFFENSE)
		{
			CG_DrawPic(headx + 48, y, 16, 16, cgs.media.assaultShader);
		}
		else if(ci->teamTask == TEAMTASK_DEFENSE)
		{
			CG_DrawPic(headx + 48, y, 16, 16, cgs.media.defendShader);
		}
	}
#endif

	// draw the score line
	if(score->ping == -1)
	{
		Com_sprintf(string, sizeof(string), " connecting    %s", ci->name);
	}
	else if(ci->team == TEAM_SPECTATOR)
	{
		Com_sprintf(string, sizeof(string), " SPECT %3i %4i %s", score->ping, score->time, ci->name);
	}
	else if(cg.snap->ps.stats[STAT_CLIENTS_READY] & (1 << score->client))
	{
		Com_sprintf(string, sizeof(string), " READY %3i %4i %s", score->ping, score->time, ci->name);
	}
	else
	{
		Com_sprintf(string, sizeof(string), "%5i %4i %4i %s", score->score, score->ping, score->time, ci->name);
	}

	CG_DrawBigString(SB_SCORELINE_X + (SB_RATING_WIDTH / 2), y, string, fade);
}

/*
=================
CG_TeamScoreboard
=================
*/
static int CG_TeamScoreboard(int y, team_t team, float fade, int maxClients, int lineHeight)
{
	int             i;
	score_t        *score;
	float           color[4];
	int             count;
	clientInfo_t   *ci;

	color[0] = color[1] = color[2] = 1.0;
	color[3] = fade;

	count = 0;
	for(i = 0; i < cg.numScores && count < maxClients; i++)
	{
		score = &cg.scores[i];
		ci = &cgs.clientinfo[score->client];

		if(team != ci->team)
			continue;

		CG_DrawClientScore(y + lineHeight * count, score, color, fade, lineHeight == SB_NORMAL_HEIGHT);

		count++;
	}

	return count;
}

/*
================
CG_CenterGiantLine
================
*/
static void CG_CenterGiantLine(float y, const char *string)
{
	float           x;
	vec4_t          color;

	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

	x = 0.5 * (640 - GIANT_WIDTH * CG_DrawStrlen(string));

	CG_DrawStringExt(x, y, string, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0);
}

// ======================================================================================

/*
=================
CG_DrawScoreboard

Draw the normal in-game scoreboard
=================
*/
qboolean CG_DrawOldScoreboard(void)
{
	int             x, y, w, n1, n2;
	float           fade;
	float          *fadeColor;
	const char     *s;
	int             maxClients;
	int             lineHeight;
	int             topBorderSize, bottomBorderSize;
	qtime_t         tm;
	char            st[1024];

	if(cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION)
	{
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	if(cg.warmup && !cg.showScores)
		return qfalse;

	if(cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD || cg.predictedPlayerState.pm_type == PM_INTERMISSION)
	{
		fade = 1.0;
		fadeColor = colorWhite;
	}
	else
	{
		fadeColor = CG_FadeColor(cg.scoreFadeTime, FADE_TIME);

		if(!fadeColor)
		{
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			return qfalse;
		}
		fade = *fadeColor;
	}

	// fragged by ... line
	if(cg.killerName[0] && cg.predictedPlayerState.pm_type != PM_INTERMISSION && cg.predictedPlayerState.pm_type == PM_DEAD)
	{
		s = va("Fragged by %s", cg.killerName);
		w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;
		x = (SCREEN_WIDTH - w) / 2;
		y = 40 + 70;
		CG_DrawStringExt(x, y, s, colorWhite, qfalse, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 900);
	}
	else
	{
		s = va("%s place with %i", CG_PlaceString(cg.snap->ps.persistant[PERS_RANK] + 1), cg.snap->ps.persistant[PERS_SCORE]);
		w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;
		x = (SCREEN_WIDTH - w) / 2;
		y = 40 + 70;
		CG_DrawStringExt(x, y, s, colorWhite, qfalse, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 900);
	}

	// scoreboard
	y = SB_HEADER + 70;

	CG_DrawPic(SB_SCORE_X + (SB_RATING_WIDTH / 2), y, 64, 32, cgs.media.scoreboardScore);
	CG_DrawPic(SB_PING_X - (SB_RATING_WIDTH / 2), y, 64, 32, cgs.media.scoreboardPing);
	CG_DrawPic(SB_TIME_X - (SB_RATING_WIDTH / 2), y, 64, 32, cgs.media.scoreboardTime);
	CG_DrawPic(SB_NAME_X - (SB_RATING_WIDTH / 2), y, 64, 32, cgs.media.scoreboardName);

	y = SB_TOP + 70;

	// If there are more than SB_MAXCLIENTS_NORMAL, use the interleaved scores
	if(cg.numScores > SB_MAXCLIENTS_NORMAL)
	{
		maxClients = SB_MAXCLIENTS_INTER;
		lineHeight = SB_INTER_HEIGHT;
		topBorderSize = 8;
		bottomBorderSize = 16;
	}
	else
	{
		maxClients = SB_MAXCLIENTS_NORMAL;
		lineHeight = SB_NORMAL_HEIGHT;
		topBorderSize = 16;
		bottomBorderSize = 16;
	}

	if(cgs.gametype >= GT_TEAM)
	{
		// teamplay scoreboard
		y += lineHeight / 2;

		if(cg.teamScores[0] >= cg.teamScores[1])
		{
			n1 = CG_TeamScoreboard(y, TEAM_RED, fade, maxClients, lineHeight);
			CG_DrawTeamBackground(0, y - topBorderSize, 640, n1 * lineHeight + bottomBorderSize, 0.33f, TEAM_RED);
			y += (n1 * lineHeight) + BIGCHAR_HEIGHT;
			maxClients -= n1;

			n2 = CG_TeamScoreboard(y, TEAM_BLUE, fade, maxClients, lineHeight);
			CG_DrawTeamBackground(0, y - topBorderSize, 640, n2 * lineHeight + bottomBorderSize, 0.33f, TEAM_BLUE);
			y += (n2 * lineHeight) + BIGCHAR_HEIGHT;
			maxClients -= n2;
		}
		else
		{
			n1 = CG_TeamScoreboard(y, TEAM_BLUE, fade, maxClients, lineHeight);
			CG_DrawTeamBackground(0, y - topBorderSize, 640, n1 * lineHeight + bottomBorderSize, 0.33f, TEAM_BLUE);
			y += (n1 * lineHeight) + BIGCHAR_HEIGHT;
			maxClients -= n1;

			n2 = CG_TeamScoreboard(y, TEAM_RED, fade, maxClients, lineHeight);
			CG_DrawTeamBackground(0, y - topBorderSize, 640, n2 * lineHeight + bottomBorderSize, 0.33f, TEAM_RED);
			y += (n2 * lineHeight) + BIGCHAR_HEIGHT;
			maxClients -= n2;
		}

		n1 = CG_TeamScoreboard(y, TEAM_SPECTATOR, fade, maxClients, lineHeight);
		y += (n1 * lineHeight) + BIGCHAR_HEIGHT;
	}
	else
	{
		// free for all scoreboard
		n1 = CG_TeamScoreboard(y, TEAM_FREE, fade, maxClients, lineHeight);
		y += (n1 * lineHeight) + BIGCHAR_HEIGHT;

		n2 = CG_TeamScoreboard(y, TEAM_SPECTATOR, fade, maxClients - n1, lineHeight);
		y += (n2 * lineHeight) + BIGCHAR_HEIGHT;
	}

	// load any models that have been deferred
	if(++cg.deferredPlayerLoading > 10)
	{
		CG_LoadDeferredPlayers();
	}

	// put the current date and time at the bottom along with version info
	trap_RealTime(&tm);
	Com_sprintf(st, sizeof(st), "%2i:%s%i:%s%i (%i %s %i) " S_COLOR_YELLOW "XreaL v" PRODUCT_VERSION " " S_COLOR_BLUE
				" http://xreal.sourceforge.net", (1 + (tm.tm_hour + 11) % 12), // 12 hour format
				(tm.tm_min > 9 ? "" : "0"),	// minute padding
				tm.tm_min, (tm.tm_sec > 9 ? "" : "0"), // second padding
				tm.tm_sec, tm.tm_mday, monthStr2[tm.tm_mon], 1900 + tm.tm_year);

	w = CG_Text_Width(st, 0.3f, 0, &cgs.media.freeSansBoldFont);
	x = 320 - w / 2;
	y = 475;
	CG_Text_Paint(x, y, 0.3f, colorWhite, st, 0, 0, UI_DROPSHADOW, &cgs.media.freeSansBoldFont);

	return qtrue;
}

/*
=================
CG_DrawTourneyScoreboard

Draw the oversize scoreboard for tournements
=================
*/
void CG_DrawOldTourneyScoreboard(void)
{
	const char     *s;
	vec4_t          color;
	int             min, tens, ones;
	clientInfo_t   *ci;
	int             y;
	int             i;

	// request more scores regularly
	if(cg.scoresRequestTime + 2000 < cg.time)
	{
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand("score");
	}

	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

	// draw the dialog background
	color[0] = color[1] = color[2] = 0;
	color[3] = 1;
	CG_FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color);

	// print the mesage of the day
	s = CG_ConfigString(CS_MOTD);
	if(!s[0])
		s = "Scoreboard";

	// print optional title
	CG_CenterGiantLine(8, s);

	// print server time
	ones = cg.time / 1000;
	min = ones / 60;
	ones %= 60;
	tens = ones / 10;
	ones %= 10;
	s = va("%i:%i%i", min, tens, ones);

	CG_CenterGiantLine(64, s);

	// print the two scores
	y = 160;
	if(cgs.gametype >= GT_TEAM)
	{
		// teamplay scoreboard
		CG_DrawStringExt(8, y, "Red Team", color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0);

		s = va("%i", cg.teamScores[0]);
		CG_DrawStringExt(632 - GIANT_WIDTH * strlen(s), y, s, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0);

		y += 64;

		CG_DrawStringExt(8, y, "Blue Team", color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0);

		s = va("%i", cg.teamScores[1]);
		CG_DrawStringExt(632 - GIANT_WIDTH * strlen(s), y, s, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0);
	}
	else
	{
		// free for all scoreboard
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			ci = &cgs.clientinfo[i];
			if(!ci->infoValid)
				continue;

			if(ci->team != TEAM_FREE)
				continue;

			CG_DrawStringExt(8, y, ci->name, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0);

			s = va("%i", ci->score);
			CG_DrawStringExt(632 - GIANT_WIDTH * strlen(s), y, s, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0);

			y += 64;
		}
	}
}

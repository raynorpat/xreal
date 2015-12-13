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
//
// cg_xppm.c -- handle md5 model stuff ( instead of cg_players.c )
#include "cg_local.h"

#ifdef XPPM


/*
======================
CG_ParseCharacterFile

Read a configuration file containing body.md5mesh custom
models/players/visor/character.cfg, etc
======================
*/

static qboolean CG_ParseCharacterFile(const char *filename, clientInfo_t * ci)
{
	char           *text_p, *prev;
	int             len;
	int             i;
	char           *token;
	float           fps;
	int             skip;
	char            text[20000];
	fileHandle_t    f;

	// load the file
	len = trap_FS_FOpenFile(filename, &f, FS_READ);
	if(len <= 0)
	{
		return qfalse;
	}
	if(len >= sizeof(text) - 1)
	{
		CG_Printf("File %s too long\n", filename);
		trap_FS_FCloseFile(f);
		return qfalse;
	}
	trap_FS_Read(text, len, f);
	text[len] = 0;
	trap_FS_FCloseFile(f);

	// parse the text
	text_p = text;
	skip = 0;					// quite the compiler warning

	ci->footsteps = FOOTSTEP_STONE;
	VectorClear(ci->headOffset);
	ci->gender = GENDER_MALE;
	ci->fixedlegs = qfalse;
	ci->fixedtorso = qfalse;
	ci->firstTorsoBoneName[0] = '\0';
	ci->lastTorsoBoneName[0] = '\0';
	ci->torsoControlBoneName[0] = '\0';
	ci->neckControlBoneName[0] = '\0';
	ci->modelScale[0] = 1;
	ci->modelScale[1] = 1;
	ci->modelScale[2] = 1;

	// read optional parameters
	while(1)
	{
		prev = text_p;			// so we can unget
		token = Com_Parse(&text_p);
		if(!token[0])
		{
			break;
		}
		
		if(!Q_stricmp(token, "footsteps"))
		{
			token = Com_Parse(&text_p);
			if(!token)
			{
				break;
			}
			if(!Q_stricmp(token, "default") || !Q_stricmp(token, "normal") || !Q_stricmp(token, "stone"))
			{
				ci->footsteps = FOOTSTEP_STONE;
			}
			else if(!Q_stricmp(token, "boot"))
			{
				ci->footsteps = FOOTSTEP_BOOT;
			}
			else if(!Q_stricmp(token, "flesh"))
			{
				ci->footsteps = FOOTSTEP_FLESH;
			}
			else if(!Q_stricmp(token, "mech"))
			{
				ci->footsteps = FOOTSTEP_MECH;
			}
			else if(!Q_stricmp(token, "energy"))
			{
				ci->footsteps = FOOTSTEP_ENERGY;
			}
			else
			{
				CG_Printf("Bad footsteps parm in %s: %s\n", filename, token);
			}
			continue;
		}
		else if(!Q_stricmp(token, "headoffset"))
		{
			for(i = 0; i < 3; i++)
			{
				token = Com_Parse(&text_p);
				if(!token)
				{
					break;
				}
				ci->headOffset[i] = atof(token);
			}
			continue;
		}
		else if(!Q_stricmp(token, "sex"))
		{
			token = Com_Parse(&text_p);
			if(!token)
			{
				break;
			}
			if(token[0] == 'f' || token[0] == 'F')
			{
				ci->gender = GENDER_FEMALE;
			}
			else if(token[0] == 'n' || token[0] == 'N')
			{
				ci->gender = GENDER_NEUTER;
			}
			else
			{
				ci->gender = GENDER_MALE;
			}
			continue;
		}
		else if(!Q_stricmp(token, "fixedlegs"))
		{
			ci->fixedlegs = qtrue;
			continue;
		}
		else if(!Q_stricmp(token, "fixedtorso"))
		{
			ci->fixedtorso = qtrue;
			continue;
		}
		else if(!Q_stricmp(token, "firstTorsoBoneName"))
		{
			token = Com_Parse(&text_p);
			Q_strncpyz(ci->firstTorsoBoneName, token, sizeof(ci->firstTorsoBoneName));
			continue;
		}
		else if(!Q_stricmp(token, "lastTorsoBoneName"))
		{
			token = Com_Parse(&text_p);
			Q_strncpyz(ci->lastTorsoBoneName, token, sizeof(ci->lastTorsoBoneName));
			continue;
		}
		else if(!Q_stricmp(token, "torsoControlBoneName"))
		{
			token = Com_Parse(&text_p);
			Q_strncpyz(ci->torsoControlBoneName, token, sizeof(ci->torsoControlBoneName));
			continue;
		}
		else if(!Q_stricmp(token, "neckControlBoneName"))
		{
			token = Com_Parse(&text_p);
			Q_strncpyz(ci->neckControlBoneName, token, sizeof(ci->neckControlBoneName));
			continue;
		}
		else if(!Q_stricmp(token, "modelScale"))
		{
			for(i = 0; i < 3; i++)
			{
				token = Com_ParseExt(&text_p, qfalse);
				if(!token)
				{
					break;
				}
				ci->modelScale[i] = atof(token);
			}
			continue;
		}

		Com_Printf("unknown token '%s' is %s\n", token, filename);
	}
	
	return qtrue;
}

static qboolean CG_RegisterPlayerAnimation(clientInfo_t * ci, const char *modelName, int anim, const char *animName,
										   qboolean loop, qboolean reversed, qboolean clearOrigin)
{
	char            filename[MAX_QPATH];
	int             frameRate;

	Com_sprintf(filename, sizeof(filename), "models/players/%s/%s.md5anim", modelName, animName);
	ci->animations[anim].handle = trap_R_RegisterAnimation(filename);
	if(!ci->animations[anim].handle)
	{
		Com_Printf("Failed to load animation file %s\n", filename);
		return qfalse;
	}

	ci->animations[anim].firstFrame = 0;
	ci->animations[anim].numFrames = trap_R_AnimNumFrames(ci->animations[anim].handle);
	frameRate = trap_R_AnimFrameRate(ci->animations[anim].handle);

	if(frameRate == 0)
	{
		frameRate = 1;
	}
	ci->animations[anim].frameTime = 1000 / frameRate;
	ci->animations[anim].initialLerp = 1000 / frameRate;

	if(loop)
	{
		ci->animations[anim].loopFrames = ci->animations[anim].numFrames;
	}
	else
	{
		ci->animations[anim].loopFrames = 0;
	}

	ci->animations[anim].reversed = reversed;
	ci->animations[anim].clearOrigin = clearOrigin;

	return qtrue;
}


/*
==========================
CG_XPPM_RegisterClientModel
==========================
*/
qboolean CG_XPPM_RegisterClientModel(clientInfo_t * ci, const char *modelName, const char *skinName,
										   const char *headModelName, const char *headSkinName, const char *teamName)
{
	int             i;
	char            filename[MAX_QPATH * 2];
	const char     *headName;
	char            newTeamName[MAX_QPATH * 2];

	if(headModelName[0] == '\0'){
		headName = modelName;
	}else{
		headName = headModelName;
	}

	Com_sprintf(filename, sizeof(filename), "models/players/%s/body.md5mesh", modelName);
	ci->bodyModel = trap_R_RegisterModel(filename, qfalse);

	if(!ci->bodyModel){
		Com_Printf("Failed to load body mesh file  %s\n", filename);
		return qfalse;
	}


	if(ci->bodyModel){
		// load the animations
		Com_sprintf(filename, sizeof(filename), "models/players/%s/character.cfg", modelName);
		if(!CG_ParseCharacterFile(filename, ci)){
			Com_Printf("Failed to load character file %s\n", filename);
			return qfalse;
		}


		if(!CG_RegisterPlayerAnimation(ci, modelName, LEGS_IDLE, "idle", qtrue, qfalse, qfalse)){
			Com_Printf("Failed to load idle animation file %s\n", filename);
			return qfalse;
		}

		// make LEGS_IDLE the default animation
		for(i = 0; i < MAX_ANIMATIONS; i++){
			if(i == LEGS_IDLE)
				continue;

			ci->animations[i] = ci->animations[LEGS_IDLE];
		}

		// FIXME we don't have death animations with player models created for Quake 4
		CG_RegisterPlayerAnimation(ci, modelName, BOTH_DEATH1, "death1", qfalse, qfalse, qfalse);
		CG_RegisterPlayerAnimation(ci, modelName, BOTH_DEATH2, "death2", qfalse, qfalse, qfalse);
		CG_RegisterPlayerAnimation(ci, modelName, BOTH_DEATH3, "death3", qfalse, qfalse, qfalse);

		CG_RegisterPlayerAnimation(ci, modelName, TORSO_GESTURE, "taunt_1", qfalse, qfalse, qfalse);

		CG_RegisterPlayerAnimation(ci, modelName, TORSO_ATTACK, "machinegun_fire", qfalse, qfalse, qfalse);
		CG_RegisterPlayerAnimation(ci, modelName, TORSO_ATTACK2, "gauntlet_fire", qfalse, qfalse, qfalse);

		CG_RegisterPlayerAnimation(ci, modelName, TORSO_STAND2, "gauntlet_aim", qfalse, qfalse, qfalse);

		CG_RegisterPlayerAnimation(ci, modelName, LEGS_WALKCR, "crouch_walk_forward", qtrue, qfalse, qtrue);
		CG_RegisterPlayerAnimation(ci, modelName, LEGS_WALK, "walk", qtrue, qfalse, qtrue);
		CG_RegisterPlayerAnimation(ci, modelName, LEGS_RUN, "run", qtrue, qfalse, qtrue);
		CG_RegisterPlayerAnimation(ci, modelName, LEGS_BACK, "run", qtrue, qtrue, qtrue);

		// FIXME CG_RegisterPlayerAnimation(ci, modelName, LEGS_SWIM, "swim", qtrue);

		CG_RegisterPlayerAnimation(ci, modelName, LEGS_JUMP, "jump", qfalse, qfalse, qfalse);
		CG_RegisterPlayerAnimation(ci, modelName, LEGS_LAND, "soft_land", qfalse, qfalse, qfalse);

		CG_RegisterPlayerAnimation(ci, modelName, LEGS_JUMPB, "jump", qfalse, qfalse, qfalse);	// FIXME ?
		CG_RegisterPlayerAnimation(ci, modelName, LEGS_LANDB, "fall", qfalse, qfalse, qfalse);

		CG_RegisterPlayerAnimation(ci, modelName, LEGS_IDLECR, "crouch", qtrue, qfalse, qfalse);

		// FIXME CG_RegisterPlayerAnimation(ci, modelName, LEGS_TURN, "jump");

		CG_RegisterPlayerAnimation(ci, modelName, LEGS_BACKCR, "crouch_walk_forward", qtrue, qtrue, qtrue);
		CG_RegisterPlayerAnimation(ci, modelName, LEGS_BACKWALK, "walk_backwards", qtrue, qfalse, qtrue);


		if(CG_FindClientModelFile(filename, sizeof(filename), ci, teamName, modelName, skinName, "body", "skin")){
			ci->bodySkin = trap_R_RegisterSkin(filename);
		}
		if(!ci->bodySkin){
			Com_Printf("Body skin load failure: %s\n", filename);
			return qfalse;
		}
	}

	if(CG_FindClientHeadFile(filename, sizeof(filename), ci, teamName, headName, headSkinName, "icon", "skin")){
		ci->modelIcon = trap_R_RegisterShaderNoMip(filename);
	}
	else if(CG_FindClientHeadFile(filename, sizeof(filename), ci, teamName, headName, headSkinName, "icon", "tga")){
		ci->modelIcon = trap_R_RegisterShaderNoMip(filename);
	}

/* do we need this ?

	if(!ci->modelIcon)
	{
		Com_Printf("Failed to load icon file %s\n", filename);
		return qfalse;
	}
*/
	return qtrue;
}



/*
======================
CG_XPPM_CopyClientInfoModel

copy of CG_CopyClientInfoModel
======================
*/
void CG_XPPM_CopyClientInfoModel(clientInfo_t * from, clientInfo_t * to)
{
	VectorCopy(from->headOffset, to->headOffset);
	to->footsteps = from->footsteps;
	to->gender = from->gender;

	Q_strncpyz(to->firstTorsoBoneName, from->firstTorsoBoneName, sizeof(to->firstTorsoBoneName));
	Q_strncpyz(to->lastTorsoBoneName, from->lastTorsoBoneName, sizeof(to->lastTorsoBoneName));

	Q_strncpyz(to->torsoControlBoneName, from->torsoControlBoneName, sizeof(to->torsoControlBoneName));
	Q_strncpyz(to->neckControlBoneName, from->neckControlBoneName, sizeof(to->neckControlBoneName));
	
	VectorCopy(from->modelScale, to->modelScale);

	to->bodyModel = from->bodyModel;
	to->bodySkin = from->bodySkin;

	to->legsModel = from->legsModel;
	to->legsAnimation = from->legsAnimation;
	to->legsSkin = from->legsSkin;

	to->torsoModel = from->torsoModel;
	to->torsoAnimation = from->torsoAnimation;
	to->torsoSkin = from->torsoSkin;

	to->headModel = from->headModel;
	to->headSkin = from->headSkin;

	to->modelIcon = from->modelIcon;

	to->newAnims = from->newAnims;

	memcpy(to->animations, from->animations, sizeof(to->animations));
	memcpy(to->sounds, from->sounds, sizeof(to->sounds));
}




/*
===============
CG_XPPM_PlayerAngles

copy of CG_PlayerAngles
===============
*/
static void CG_XPPM_PlayerAngles(centity_t * cent, vec3_t legsAngles, vec3_t torsoAngles, vec3_t headAngles)
{
	float           dest;
	static int      movementOffsets[8] = { 0, 22, 45, -22, 0, 22, -45, -22 };
	vec3_t          velocity;
	float           speed;
	int             dir, clientNum;
	clientInfo_t   *ci;

	VectorCopy(cent->lerpAngles, headAngles);
	headAngles[YAW] = AngleMod(headAngles[YAW]);
	VectorClear(legsAngles);
	VectorClear(torsoAngles);

	// --------- yaw -------------

	// allow yaw to drift a bit
	if((cent->currentState.legsAnim & ~ANIM_TOGGLEBIT) != LEGS_IDLE
	   || (cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT) != TORSO_STAND)
	{
		// if not standing still, always point all in the same direction
		cent->pe.torso.yawing = qtrue;	// always center
		cent->pe.torso.pitching = qtrue;	// always center
		cent->pe.legs.yawing = qtrue;	// always center
	}

	// adjust legs for movement dir
	if(cent->currentState.eFlags & EF_DEAD)
	{
		// don't let dead bodies twitch
		dir = 0;
	}
	else
	{
		dir = cent->currentState.angles2[YAW];
		if(dir < 0 || dir > 7)
		{
			CG_Error("Bad player movement angle");
		}
	}
	legsAngles[YAW] = headAngles[YAW] + movementOffsets[dir];
	torsoAngles[YAW] = headAngles[YAW] + 0.25 * movementOffsets[dir];

	// torso
	CG_SwingAngles(torsoAngles[YAW], 25, 90, cg_swingSpeed.value, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing);
	CG_SwingAngles(legsAngles[YAW], 40, 90, cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing);

	torsoAngles[YAW] = cent->pe.torso.yawAngle;
	legsAngles[YAW] = cent->pe.legs.yawAngle;


	// --------- pitch -------------

	// only show a fraction of the pitch angle in the torso
	if(headAngles[PITCH] > 180)
	{
		dest = (-360 + headAngles[PITCH]) * 0.75f;
	}
	else
	{
		dest = headAngles[PITCH] * 0.75f;
	}
	CG_SwingAngles(dest, 15, 30, 0.1f, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching);
	torsoAngles[PITCH] = cent->pe.torso.pitchAngle;

	//
	clientNum = cent->currentState.clientNum;
	if(clientNum >= 0 && clientNum < MAX_CLIENTS)
	{
		ci = &cgs.clientinfo[clientNum];
		if(ci->fixedtorso)
		{
			torsoAngles[PITCH] = 0.0f;
		}
	}

	// --------- roll -------------


	// lean towards the direction of travel
	VectorCopy(cent->currentState.pos.trDelta, velocity);
	speed = VectorNormalize(velocity);
	if(speed)
	{
		vec3_t          axis[3];
		float           side;

		speed *= 0.02f;

		AnglesToAxis(legsAngles, axis);
		side = speed * DotProduct(velocity, axis[1]);
		legsAngles[ROLL] -= side;

		side = speed * DotProduct(velocity, axis[0]);
		legsAngles[PITCH] += side;
	}

	//
	clientNum = cent->currentState.clientNum;
	if(clientNum >= 0 && clientNum < MAX_CLIENTS)
	{
		ci = &cgs.clientinfo[clientNum];
		if(ci->fixedlegs)
		{
			legsAngles[YAW] = torsoAngles[YAW];
			legsAngles[PITCH] = 0.0f;
			legsAngles[ROLL] = 0.0f;
		}
	}

	// pain twitch
	CG_AddPainTwitch(cent, torsoAngles);

	// pull the angles back out of the hierarchial chain
	AnglesSubtract(headAngles, torsoAngles, headAngles);
	AnglesSubtract(torsoAngles, legsAngles, torsoAngles);
}


/*
===============
CG_XPPM_SetLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
*/
static void CG_XPPM_SetLerpFrameAnimation(clientInfo_t * ci, lerpFrame_t * lf, int newAnimation)
{
	animation_t    *anim;

	//save old animation
	
	lf->old_animationNumber = lf->animationNumber;
	lf->old_animation = lf->animation;

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if(newAnimation < 0 || newAnimation >= MAX_TOTALANIMATIONS)
	{
		CG_Error("bad player animation number: %i", newAnimation);
	}

	anim = &ci->animations[newAnimation];

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;

	if(cg_debugPlayerAnim.integer)
	{
		CG_Printf("player anim: %i\n", newAnimation);
	}

	
	debug_anim_current = lf->animationNumber;
	debug_anim_old = lf->old_animationNumber;

	if(lf->old_animationNumber <= 0){ // skip initial / invalid blending
		lf->blendlerp = 0.0f;
		return;
	}

	//TODO: blend through two blendings!
	lf->blendlerp = 1.0f;

	memcpy( &lf->oldSkeleton, &lf->skeleton, sizeof(refSkeleton_t));
	
	//Com_Printf("new: %i old %i\n", newAnimation,lf->old_animationNumber);
		
	if(!trap_R_BuildSkeleton(&lf->oldSkeleton,lf->old_animation->handle, lf->oldFrame, lf->frame, 1.0f, lf->old_animation->clearOrigin)){
		CG_Printf("Can't build blending skeleton\n");
		return;
	}


}

//TODO: choose propper values and use blending speed from character.cfg
// blending is slow for testing issues

static void CG_XPPM_BlendLerpFrame(lerpFrame_t * lf){

	if(cg_animBlend.value <= 0.0f){
		lf->blendlerp = 0.0f;
		return;
	}

	if(( lf->blendlerp > 0.0f ) && (cg.time > lf->blendtime)){

#if 0 
		//linear blending
			lf->blendlerp -=0.025f;
#else
		//exp blending
		lf->blendlerp -=lf->blendlerp / cg_animBlend.value;
#endif
		if(lf->blendlerp <= 0.0f)
			lf->blendlerp = 0.0f;
		if(lf->blendlerp >= 1.0f)
			lf->blendlerp = 1.0f;

		lf->blendtime = cg.time + 10;

		debug_anim_blend = lf->blendlerp;
	}

}



/*
===============
CG_XPPM_RunLerpFrame

copy of CG_RunLerpFrame
===============
*/
static void CG_XPPM_RunLerpFrame(clientInfo_t * ci, lerpFrame_t * lf, int newAnimation, float speedScale)
{
	int             f, numFrames;
	animation_t    *anim;
	qboolean        animChanged;

	// debugging tool to get no animations
	if(cg_animSpeed.integer == 0)
	{
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}

	// see if the animation sequence is switching
	if(newAnimation != lf->animationNumber || !lf->animation)
	{
		CG_XPPM_SetLerpFrameAnimation(ci, lf, newAnimation);

		if(!lf->animation)
		{
			memcpy(&lf->oldSkeleton, &lf->skeleton, sizeof(refSkeleton_t));
		}

		animChanged = qtrue;

	}
	else
	{
		animChanged = qfalse;
	}


	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if(cg.time >= lf->frameTime || animChanged)
	{

		if(animChanged)
		{
			lf->oldFrame = 0;
			lf->oldFrameTime = cg.time;
		}
		else

		{
			lf->oldFrame = lf->frame;
			lf->oldFrameTime = lf->frameTime;
		}

		// get the next frame based on the animation
		anim = lf->animation;
		if(!anim->frameTime)
		{
			return;				// shouldn't happen
		}

		if(cg.time < lf->animationTime)
		{
			lf->frameTime = lf->animationTime;	// initial lerp
		}
		else
		{
			lf->frameTime = lf->oldFrameTime + anim->frameTime;
		}
		f = (lf->frameTime - lf->animationTime) / anim->frameTime;
		f *= speedScale;		// adjust for haste, etc

		numFrames = anim->numFrames;

		if(anim->flipflop)
		{
			numFrames *= 2;
		}

		if(f >= numFrames)
		{
			f -= numFrames;

			if(anim->loopFrames)
			{
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			}
			else
			{
				f = numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}

		if(anim->reversed)
		{
			lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
		}
		else if(anim->flipflop && f >= anim->numFrames)
		{
			lf->frame = anim->firstFrame + anim->numFrames - 1 - (f % anim->numFrames);
		}
		else
		{
			lf->frame = anim->firstFrame + f;
		}

		if(cg.time > lf->frameTime)
		{
			lf->frameTime = cg.time;
			if(cg_debugPlayerAnim.integer)
			{
				CG_Printf("clamp player lf->frameTime\n");
			}
		}
	}

	if(lf->frameTime > cg.time + 200)
	{
		lf->frameTime = cg.time;
	}

	if(lf->oldFrameTime > cg.time)
	{
		lf->oldFrameTime = cg.time;
	}

	// calculate current lerp value
	if(lf->frameTime == lf->oldFrameTime)
	{
		lf->backlerp = 0;
	}
	else
	{
		lf->backlerp = 1.0 - (float)(cg.time - lf->oldFrameTime) / (lf->frameTime - lf->oldFrameTime);
	}
	
	//blend old and current animation
	CG_XPPM_BlendLerpFrame(lf);

	if(!trap_R_BuildSkeleton(&lf->skeleton, lf->animation->handle, lf->oldFrame, lf->frame, 1.0 - lf->backlerp, lf->animation->clearOrigin))
	{
		CG_Printf("Can't build lf->skeleton\n");
	}

	// lerp between old and new animation if possible
	if(lf->blendlerp > 0.0f){
		if(!trap_R_BlendSkeleton(&lf->skeleton, &lf->oldSkeleton, lf->blendlerp))
		{
			CG_Printf("Can't blend\n");
			return;
		}
	}

}

/*
===============
CG_XPPM_PlayerAnimation
===============
*/

static void CG_XPPM_PlayerAnimation(centity_t * cent)
{
	clientInfo_t   *ci;
	int             clientNum;
	float           speedScale;

	clientNum = cent->currentState.clientNum;

	if(cg_noPlayerAnims.integer)
	{
		return;
	}

	if(cent->currentState.powerups & (1 << PW_HASTE))
	{
		speedScale = 1.5;
	}
	else
	{
		speedScale = 1;
	}

	ci = &cgs.clientinfo[clientNum];

	// do the shuffle turn frames locally
	if(cent->pe.legs.yawing && (cent->currentState.legsAnim & ~ANIM_TOGGLEBIT) == LEGS_IDLE)
	{
		CG_XPPM_RunLerpFrame(ci, &cent->pe.legs, LEGS_TURN, speedScale);
	}
	else
	{
		CG_XPPM_RunLerpFrame(ci, &cent->pe.legs, cent->currentState.legsAnim, speedScale);
	}

	CG_XPPM_RunLerpFrame(ci, &cent->pe.torso, cent->currentState.torsoAnim, speedScale);
}





/*
===============
CG_XPPM_Player
===============
*/

void CG_XPPM_Player(centity_t * cent){
	clientInfo_t   *ci;
	refEntity_t     body;
	int             clientNum;
	int             renderfx;
	qboolean        shadow;
	float           shadowPlane;
	int             noShadowID;

	vec3_t          legsAngles;
	vec3_t          torsoAngles;
	vec3_t          headAngles;

	quat_t          legsQuat;
	quat_t          torsoQuat;
	quat_t          headQuat;

	int             i;
	int             boneIndex;
	int             firstTorsoBone;
	int             lastTorsoBone;

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if(clientNum < 0 || clientNum >= MAX_CLIENTS)
	{
		CG_Error("Bad clientNum on player entity");
	}
	ci = &cgs.clientinfo[clientNum];

	// it is possible to see corpses from disconnected players that may
	// not have valid clientinfo
	if(!ci->infoValid)
	{
		CG_Printf("Bad clientInfo for player %i\n", clientNum);
		return;
	}

	// get the player model information
	renderfx = 0;
	if(cent->currentState.number == cg.snap->ps.clientNum)
	{
		if(!cg.renderingThirdPerson)
		{
			renderfx = RF_THIRD_PERSON;	// only draw in mirrors
		}
		else
		{
			if(cg_cameraMode.integer)
			{
				return;
			}
		}
	}

	memset(&body, 0, sizeof(body));

	// generate a new unique noShadowID to avoid that the lights of the quad damage
	// will cause bad player shadows
	noShadowID = CG_UniqueNoShadowID();
	body.noShadowID = noShadowID;

	AxisClear(body.axis);

	// get the rotation information
	CG_XPPM_PlayerAngles(cent, legsAngles, torsoAngles, headAngles);
	AnglesToAxis(legsAngles, body.axis);

	// get the animation state (after rotation, to allow feet shuffle)
	CG_XPPM_PlayerAnimation(cent);

	// add the talk baloon or disconnect icon
	CG_PlayerSprites(cent);

	// add the shadow
	shadow = CG_PlayerShadow(cent, &shadowPlane, noShadowID);

	// add a water splash if partially in and out of water
	CG_PlayerSplash(cent);

	if(cg_shadows.integer == 2 && shadow)
	{
		renderfx |= RF_SHADOW_PLANE;
	}
	renderfx |= RF_LIGHTING_ORIGIN;	// use the same origin for all
#ifdef MISSIONPACK
	if(cgs.gametype == GT_HARVESTER)
	{
		CG_PlayerTokens(cent, renderfx);
	}
#endif

	// add the body
	body.hModel = ci->bodyModel;
	//body.customSkin = ci->bodySkin;

	if(!body.hModel)
	{
		CG_Printf("No body model for player %i\n", clientNum);
		return;
	}

	VectorCopy(cent->lerpOrigin, body.origin);
	body.origin[2] += MINS_Z;

	VectorCopy(body.origin, body.lightingOrigin);
	body.shadowPlane = shadowPlane;
	body.renderfx = renderfx;
	VectorCopy(body.origin, body.oldorigin);	// don't positionally lerp at all

	// copy legs skeleton to have a base
	memcpy(&body.skeleton, &cent->pe.legs.skeleton, sizeof(refSkeleton_t));

	if(cent->pe.legs.skeleton.numBones != cent->pe.torso.skeleton.numBones)
	{
		CG_Error("cent->pe.legs.skeleton.numBones != cent->pe.torso.skeleton.numBones");
		return;
	}

	// combine legs and torso skeletons
#if 1
	firstTorsoBone = trap_R_BoneIndex(body.hModel, ci->firstTorsoBoneName);

	if(firstTorsoBone >= 0 && firstTorsoBone < cent->pe.torso.skeleton.numBones)
	{
		lastTorsoBone = trap_R_BoneIndex(body.hModel, ci->lastTorsoBoneName);

		if(lastTorsoBone >= 0 && lastTorsoBone < cent->pe.torso.skeleton.numBones)
		{
			// copy torso bones
			for(i = firstTorsoBone; i < lastTorsoBone; i++)
			{
				memcpy(&body.skeleton.bones[i], &cent->pe.torso.skeleton.bones[i], sizeof(refBone_t));
			}
		}

		body.skeleton.type = SK_RELATIVE;

		// update AABB
		for(i = 0; i < 3; i++)
		{
			body.skeleton.bounds[0][i] =
				cent->pe.torso.skeleton.bounds[0][i] <
				cent->pe.legs.skeleton.bounds[0][i] ? cent->pe.torso.skeleton.bounds[0][i] : cent->pe.legs.skeleton.bounds[0][i];
			body.skeleton.bounds[1][i] =
				cent->pe.torso.skeleton.bounds[1][i] >
				cent->pe.legs.skeleton.bounds[1][i] ? cent->pe.torso.skeleton.bounds[1][i] : cent->pe.legs.skeleton.bounds[1][i];
		}
	}
	else
	{
		// bad no hips found
		body.skeleton.type = SK_INVALID;
	}
#endif

	// rotate legs
#if 0
	boneIndex = trap_R_BoneIndex(body.hModel, "origin");

	if(boneIndex >= 0 && boneIndex < cent->pe.legs.skeleton.numBones)
	{
		// HACK: convert angles to bone system
		QuatFromAngles(legsQuat, legsAngles[YAW], -legsAngles[ROLL], legsAngles[PITCH]);
		QuatMultiply0(body.skeleton.bones[boneIndex].rotation, legsQuat);
	}
#endif


	// rotate torso
#if 1
	boneIndex = trap_R_BoneIndex(body.hModel, ci->torsoControlBoneName);

	if(boneIndex >= 0 && boneIndex < cent->pe.legs.skeleton.numBones)
	{
		// HACK: convert angles to bone system
		QuatFromAngles(torsoQuat, torsoAngles[ROLL], -torsoAngles[PITCH], torsoAngles[YAW]);
		QuatMultiply0(body.skeleton.bones[boneIndex].rotation, torsoQuat);
	}
#endif

	// rotate head
#if 1
	boneIndex = trap_R_BoneIndex(body.hModel, ci->neckControlBoneName);

	if(boneIndex >= 0 && boneIndex < cent->pe.legs.skeleton.numBones)
	{
		// HACK: convert angles to bone system
		QuatFromAngles(headQuat, headAngles[ROLL], headAngles[PITCH], headAngles[YAW]);
		QuatMultiply0(body.skeleton.bones[boneIndex].rotation, headQuat);
	}
#endif

	// transform relative bones to absolute ones required for vertex skinning and tag attachments
	CG_TransformSkeleton(&body.skeleton, ci->modelScale);

	// add body to renderer
	CG_AddRefEntityWithPowerups(&body, &cent->currentState, ci->team);

	// TODO add TA kamikaze model and other stuff

	// add the gun / barrel / flash
	CG_AddPlayerWeapon(&body, NULL, cent, ci->team);

	// add powerups floating behind the player
	CG_PlayerPowerups(cent, &body, noShadowID);

//unlagged - client options
	// add the bounding box (if cg_drawBBox is 1)
	CG_AddBoundingBox(cent);
//unlagged - client options
}


#endif
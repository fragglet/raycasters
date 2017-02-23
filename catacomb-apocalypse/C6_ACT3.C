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

// C3_PLAY.C

#include "DEF.H"
#pragma hdrstop

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

boolean ShootPlayer (objtype *ob, short obclass, short speed, statetype *state);
void T_ShootPlayer(objtype *ob);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/





/*
=============================================================================

							DEMON

=============================================================================
*/

void T_TrollDemon (objtype *ob);

statetype s_demonpause = {DEMON1PIC,40,NULL,&s_demon2};

statetype s_demon1 = {DEMON1PIC,20,&T_TrollDemon,&s_demon2};
statetype s_demon2 = {DEMON2PIC,20,&T_TrollDemon,&s_demon3};
statetype s_demon3 = {DEMON3PIC,20,&T_TrollDemon,&s_demon4};
statetype s_demon4 = {DEMON4PIC,20,&T_TrollDemon,&s_demon1};

statetype s_demonattack1 = {DEMONATTACK1PIC,20,NULL,&s_demonattack2};
statetype s_demonattack2 = {DEMONATTACK2PIC,20,NULL,&s_demonattack3};
statetype s_demonattack3 = {DEMONATTACK3PIC,30,&T_DoDamage,&s_demonpause};

statetype s_demonouch = {DEMONOUCHPIC,15,&T_TrollDemon,&s_demon1};

statetype s_demondie1 = {DEMONDIE1PIC,40,NULL,&s_demondie2};
statetype s_demondie2 = {DEMONDIE2PIC,30,&LargeSound,&s_demondie3};
statetype s_demondie3 = {DEMONDIE3PIC,0,NULL,&s_demondie3};



/*
===============
=
= SpawnDemon
=
===============
*/

void SpawnDemon (int tilex, int tiley)
{
	SpawnNewObj(tilex,tiley,&s_demon1,PIXRADIUS*35);
	new->obclass = demonobj;
	new->speed = 2048;
	new->flags |= of_shootable;
	new->hitpoints = EasyHitPoints(30);
}


/*
=============================================================================

										TROLL

=============================================================================
*/

statetype s_trollpause = {TROLL1PIC, 30, &T_DoDamage, &s_troll2};

statetype s_troll1 = {TROLL1PIC, 13, &T_TrollDemon, &s_troll2};
statetype s_troll2 = {TROLL2PIC, 13, &T_TrollDemon, &s_troll3};
statetype s_troll3 = {TROLL3PIC, 13, &T_TrollDemon, &s_troll4};
statetype s_troll4 = {TROLL4PIC, 13, &T_TrollDemon, &s_troll1};

statetype s_trollattack1 = {TROLLATTACK1PIC, 15, NULL, &s_trollattack2};
statetype s_trollattack2 = {TROLLATTACK2PIC, 20, NULL, &s_trollpause};

statetype s_trollouch = {TROLLOUCHPIC, 14, &T_TrollDemon, &s_troll1};

statetype s_trolldie1 = {TROLLDIE1PIC, 18, NULL, &s_trolldie2};
statetype s_trolldie2 = {TROLLDIE2PIC, 15, &LargeSound, &s_trolldie3};
statetype s_trolldie3 = {TROLLDIE3PIC, 0, NULL, &s_trolldie3};


/*
===============
=
= SpawnTroll
=
===============
*/

void SpawnTroll (int tilex, int tiley)
{
	SpawnNewObj(tilex,tiley,&s_troll1,35*PIXRADIUS);
	new->speed = 2500;
	new->obclass = trollobj;
	new->flags |= of_shootable;
	new->hitpoints = EasyHitPoints(15);
}


/*
=============================================================================

										CYBORG DEMON

=============================================================================
*/

void T_Demon (objtype *ob);

statetype s_cyborg_demon1 = {CYBORG1PIC, 20, T_TrollDemon, &s_cyborg_demon2};
statetype s_cyborg_demon2 = {CYBORG2PIC, 20, T_TrollDemon, &s_cyborg_demon3};
statetype s_cyborg_demon3 = {CYBORG3PIC, 20, T_TrollDemon, &s_cyborg_demon4};
statetype s_cyborg_demon4 = {CYBORG4PIC, 20, T_TrollDemon, &s_cyborg_demon1};

statetype s_cyborg_demonattack1 = {CYBORGATTACK1PIC, 20, NULL, &s_cyborg_demonattack2};
statetype s_cyborg_demonattack2 = {CYBORGATTACK2PIC, 20, NULL, &s_cyborg_demonattack3};
statetype s_cyborg_demonattack3 = {CYBORGATTACK3PIC, 30, T_DoDamage, &s_cyborg_demon2};

statetype s_cyborg_demonouch = {CYBORGOUCHPIC, 30, NULL, &s_cyborg_demon1};

statetype s_cyborg_demondie1 = {CYBORGOUCHPIC, 40, NULL, &s_cyborg_demondie2};
statetype s_cyborg_demondie2 = {CYBORGDIE1PIC, 30, &LargeSound, &s_cyborg_demondie3};
statetype s_cyborg_demondie3 = {CYBORGDIE2PIC, 20, NULL, &s_cyborg_demondie3};

/*
===============
=
= SpawnCyborgDemon
=
===============
*/

void SpawnCyborgDemon (int tilex, int tiley)
{
	SpawnNewObj(tilex, tiley, &s_cyborg_demon1, PIXRADIUS*35);
	new->obclass	= cyborgdemonobj;
	new->speed = 2048;
	new->flags |= of_shootable;
	new->hitpoints = EasyHitPoints(30);
}


/*
===============
=
= T_TrollDemon
=
===============
*/

void T_TrollDemon (objtype *ob)
{
	if (Chase (ob,true) || (random(1000)<RANDOM_ATTACK))
	{
		if (ob->obclass == cyborgdemonobj)
			ob->state = &s_cyborg_demonattack1;
		else
			if (ob->obclass == trollobj)
				ob->state = &s_trollattack1;
			else
				ob->state = &s_demonattack1;
		ob->ticcount = ob->state->tictime;
	}
}





/*
=============================================================================

										INVISIBLE DUDE!

=============================================================================
*/

void T_InvisibleDude (objtype *ob);

statetype s_invis_fizz1 = {INVIS_FIZZ1PIC, 8, &T_InvisibleDude, &s_invis_fizz2};
statetype s_invis_fizz2 = {INVIS_FIZZ2PIC, 8, &T_InvisibleDude, &s_invis_fizz3};
statetype s_invis_fizz3 = {INVIS_FIZZ3PIC, 8, &T_InvisibleDude, &s_invis_walk};

statetype s_invis_walk = {0, 25, &T_InvisibleDude, &s_invis_walk};
statetype s_invis_attack = {0, -1, &T_DoDamage, &s_invis_pause};
statetype s_invis_pause = {0, 40, NULL, &s_invis_walk};

statetype s_invis_flash1 = {INVIS_FIZZ1PIC, 8, &T_InvisibleDude, &s_invis_walk};
statetype s_invis_flash2 = {INVIS_FIZZ2PIC, 8, &T_InvisibleDude, &s_invis_walk};
statetype s_invis_flash3 = {INVIS_FIZZ3PIC, 8, &T_InvisibleDude, &s_invis_walk};

statetype s_invis_death1 = {INVIS_DEATH1PIC, 40, NULL, &s_invis_death2};
statetype s_invis_death2 = {INVIS_DEATH2PIC, 30, &LargeSound, &s_invis_death3};
statetype s_invis_death3 = {INVIS_DEATH3PIC, 20, NULL, &s_invis_death3};

/*
===============
=
= SpawnInvisDude
=
===============
*/
void SpawnInvisDude(int tilex, int tiley)
{
	SpawnNewObj(tilex, tiley, &s_invis_walk, PIXRADIUS*20);
	new->obclass	= invisdudeobj;
	new->speed		= 2048;
	new->flags		|= of_shootable;
	new->hitpoints	= EasyHitPoints(20);
	new->temp1		= 0;		// for random flashing of pictures
}


/*
===============
=
= T_InvisibleDude
=
===============
*/
void T_InvisibleDude (objtype *ob)
{
	if (!random(100))
	{
		switch (ob->temp1++)
		{
			case 0:
				ob->state = &s_invis_flash1;
			break;

			case 1:
				ob->state = &s_invis_flash2;
			break;

			case 2:
				ob->state = &s_invis_flash3;
				ob->temp1 = 0;
			break;
		}
		ob->ticcount = ob->state->tictime;
	}


	if (Chase (ob,true))
	{
		ob->state = &s_invis_attack;
		ob->ticcount = ob->state->tictime;
	}

}



/*
=============================================================================

											BOUNCE

temp2 = set when hit player, reset when hit wall

=============================================================================
*/

#define SPDBOUNCE	4096
#define DMGBOUNCE	10

void T_Bounce (objtype *ob);
void T_Bounce_Death (objtype *ob);

statetype s_bounce1 = {PSHOT1PIC, 8, &T_Bounce, &s_bounce2};
statetype s_bounce2 = {PSHOT2PIC, 8, &T_Bounce, &s_bounce1};

/*
===============
=
= SpawnBounce
=
===============
*/

void SpawnBounce (int tilex, int tiley, boolean towest)
{
	SpawnNewObj(tilex, tiley, &s_bounce1, 24*PIXRADIUS);
	new->obclass = bounceobj;
	new->hitpoints = EasyHitPoints(10);
	new->flags |= of_shootable;
	if (towest)
		new->dir = west;
	else
		new->dir = north;
}


/*
===============
=
= T_Bounce
=
===============
*/

void T_Bounce (objtype *ob)
{
	long move;
	long deltax,deltay,size;

	move = SPDBOUNCE*tics;
	size = (long)ob->size + player->size + move;

	while (move)
	{
		deltax = ob->x - player->x;
		deltay = ob->y - player->y;

		if (deltax <= size && deltax >= -size
		&& deltay <= size && deltay >= -size && !ob->temp2)
		{
			ob->temp2 = 1;
			TakeDamage (DMGBOUNCE);
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}
		actorat[ob->tilex][ob->tiley] = 0;	// pick up marker from goal

		ob->x = ((long)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((long)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		move -= ob->distance;

		//
		// bounce if hit wall
		//
		switch (ob->dir)
		{
		case north:
			if (tilemap[ob->tilex][--ob->tiley])
			{
				ob->dir = south;
				ob->tiley+=2;
				ob->temp2 = 0;
			}
			break;
		case east:
			if (tilemap[++ob->tilex][ob->tiley])
			{
				ob->dir = west;
				ob->tilex-=2;
				ob->temp2 = 0;
			}
			break;
		case south:
			if (tilemap[ob->tilex][++ob->tiley])
			{
				ob->dir = north;
				ob->tiley-=2;
				ob->temp2 = 0;
			}
			break;
		case west:
			if (tilemap[--ob->tilex][ob->tiley])
			{
				ob->dir = east;
				ob->tilex+=2;
				ob->temp2 = 0;
			}
			break;
		}

		ob->distance = TILEGLOBAL;

		actorat[ob->tilex][ob->tiley] = ob;	// set down a new goal marker
	}
	CalcBounds (ob);
}


/*
=============================================================================

							GRELMINAR

=============================================================================
*/


void T_Grelminar (objtype *ob);
void T_GrelminarShoot (objtype *ob);
void T_Grelm_DropKey(objtype *ob);

statetype s_grelpause = {GREL1PIC,50,NULL,&s_grel2};

statetype s_grel1 = {GREL1PIC,20,T_Grelminar,&s_grel2};
statetype s_grel2 = {GREL2PIC,20,T_Grelminar,&s_grel1};

statetype s_grelattack3 = {GRELATTACKPIC,30,NULL,&s_grelpause};

statetype s_grelouch = {GRELHITPIC,6,NULL,&s_grel1};

statetype s_greldie1 = {GRELDIE1PIC,22,NULL,&s_greldie2};
statetype s_greldie2 = {GRELDIE2PIC,22,NULL,&s_greldie3};
statetype s_greldie3 = {GRELDIE3PIC,22,NULL,&s_greldie4};
statetype s_greldie4 = {GRELDIE4PIC,22,NULL,&s_greldie5};
statetype s_greldie5 = {GRELDIE5PIC,22,NULL,&s_greldie5a};
statetype s_greldie5a = {GRELDIE5PIC,-1,T_Grelm_DropKey,&s_greldie6};
statetype s_greldie6 = {GRELDIE6PIC,0,NULL,&s_greldie6};

statetype s_gshot1 = {SKULL_SHOTPIC,8,T_ShootPlayer,&s_gshot1};

/*
===============
=
= SpawnGrelminar
=
===============
*/

void SpawnGrelminar (int tilex, int tiley)
{
	unsigned Grel_Hard;
	unsigned DropKey;

	SpawnNewObj(tilex,tiley,&s_grel1,PIXRADIUS*25);
	new->obclass = grelmobj;
	new->speed = 2048;
	new->flags |= of_shootable;

	//
	// if Grelminar is to drop a key the info-plane byte to the right
	//		should have a 1 in the highbyte, else he will not drop the key.
	//
	DropKey = *(mapsegs[2]+farmapylookup[tiley]+tilex+1);
	if (DropKey)
		new->temp1 = DropKey>>8;
	else
		new->temp1 = 0;

	//
	// The info-plane byte below Grelminar will determine how powerful
	//		Grelminar is.  If nothing is there, he is the most powerful.
	//			-- affected are the hit points and the shot damage.
	//	The hit points are controlled here, the shot damage is controlled
	// 	within the spawning of the shot.  See ShootPlayer for more info.
	//
	Grel_Hard = *(mapsegs[2]+farmapylookup[tiley+1]+tilex);
	if (Grel_Hard)
	{
		new->temp2 = Grel_Hard>>8;
		new->hitpoints = EasyHitPoints((new->temp2 * 10));
	}
	else
	{
		new->hitpoints = EasyHitPoints(100);
		new->temp2 = 10;
	}
}


/*
===============
=
= T_Grelminar
=
===============
*/

void T_Grelminar (objtype *ob)
{
	Chase (ob,false);

	if (!random(10))
		if (ShootPlayer(ob,gshotobj,ob->temp2,&s_gshot1))
		{
			ob->state = &s_grelattack3;
			ob->ticcount = ob->state->tictime;
		}
	if (CheckHandAttack(ob))
		TakeDamage (ob->temp2*3);

}


//=================================
//
// T_Grelm_DropKey
//
//=================================
void T_Grelm_DropKey(objtype *ob)
{
	if (!(ob->temp1))
	{
		ob->state = NULL;
		return;
	}

	SpawnBonus(ob->tilex,ob->tiley,B_RKEY);
	SD_PlaySound(GRELM_DEADSND);
	ob->temp1 = false;
}



//--------------------------------------------------------------------------
// ShootPlayer()
//--------------------------------------------------------------------------
boolean ShootPlayer(objtype *ob, short obclass, short speed, statetype *state)
{
	int angle = AngleNearPlayer(ob);

	if (angle == -1)
		return(false);

	DSpawnNewObjFrac (ob->x,ob->y,state,PIXRADIUS*14);
	new->obclass = obclass;
	new->active = always;
	new->angle = angle;

	//
	//	If the shot is Grelminar's, then determine the power of the shot.
	//	The shot speed is hard-wired as 10000.  But the shot power is
	//		determined by speed.  Speed now contains "Grelminar's level of
	//		hardness" and this is multiplied by 3 to get the shot power.
	//
	if (obclass == gshotobj)
	{
		new->speed = 10000;
		new->temp1 = speed*3;
	}
	else
		new->speed = speed;


	return(true);
}

//--------------------------------------------------------------------------
// T_ShootPlayer()
//--------------------------------------------------------------------------
void T_ShootPlayer(objtype *ob)
{
	objtype *check;
	long xmove,ymove,speed;

	speed = ob->speed*tics;

	xmove = FixedByFrac(speed,costable[ob->angle]);
	ymove = -FixedByFrac(speed,sintable[ob->angle]);

	if (ShotClipMove(ob,xmove,ymove))
	{
		ob->state = &s_pshot_exp1;
		ob->ticcount = ob->state->tictime;
		return;
	}

	ob->tilex = ob->x >> TILESHIFT;
	ob->tiley = ob->y >> TILESHIFT;


// check for collision with wall
//
	if (tilemap[ob->tilex][ob->tiley])
	{
//		SD_PlaySound (SHOOTWALLSND);
		ob->state = &s_pshot_exp1;
		ob->ticcount = s_pshot_exp1.tictime;
		return;
	}



// check for collision with player
//
	if ( ob->xl <= player->xh
	&& ob->xh >= player->xl
	&& ob->yl <= player->yh
	&& ob->yh >= player->yl)
	{
		switch (ob->obclass)
		{
			case wshotobj:						// Wizard's shot
				TakeDamage (7);
			break;

			case hshotobj:						// Egyptian Head's shot
				TakeDamage (5);
			break;

			case bshotobj:						// Blob's shot
				TakeDamage (5);
			break;

			case rshotobj:						// Ray's shot
				TakeDamage (5);
			break;

			case rbshotobj:					// RamBone's shot
				TakeDamage(7);
			break;

			case fmshotobj:					// Future Mage's shot
				TakeDamage(7);
			break;

			case rtshotobj:					// RoboTank's shot
				TakeDamage(15);
			break;

			case syshotobj:					// Stompy's shot
				TakeDamage(7);
			break;

			case bgshotobj:					// Bug's shot
				TakeDamage(7);
			break;

			case eshotobj:						// Eye's shot
				TakeDamage(5);
			break;

			case gshotobj:
				TakeDamage (ob->temp1);		// the damage of Grelminar's shot -
			break;								//   see Grelminar's spawning

		}
		ob->state = NULL;
		return;
	}

// check for collision with other solid and realsolid objects.
//  Great terminology!! -- solid objects really aren't solid
//                      -- realsolid objects ARE solid
//	if ((actorat[ob->tilex][ob->tiley]) && (actorat[ob->tilex][ob->tiley]->obclass != ob->obclass))
	if (((actorat[ob->tilex][ob->tiley]->obclass == realsolidobj) ||
		 (actorat[ob->tilex][ob->tiley]->obclass == solidobj)) &&
		 (actorat[ob->tilex][ob->tiley]->flags & of_shootable))
	{
			ob->state = &s_pshot_exp1;
			ob->ticcount = s_pshot_exp1.tictime;
			return;
	}


// check for collision with player
//
	for (check = player->next; check; check=check->next)
		if ((ob->flags & of_shootable)
		&& ob->xl <= check->xh
		&& ob->xh >= check->xl
		&& ob->yl <= check->yh
		&& ob->yh >= check->yl)
		{
			switch (ob->obclass)
			{
// APOCALYPSE
				case wshotobj:						// Wizard's shot
					ShootActor (check, 3);
				break;

				case hshotobj:						// Egyptian Head's shot
					ShootActor (check, 5);
				break;

				case bshotobj:						// Blob's shot
					ShootActor (check, 2);
				break;

				case rshotobj:						// Ray's shot
					ShootActor (check, 5);
				break;

				case rbshotobj:					// RamBone's shot
					ShootActor (check, 5);
				break;

				case fmshotobj:					// Future Mage's shot
					ShootActor (check, 5);
				break;

				case rtshotobj:					// RoboTank's shot
					ShootActor (check, 15);
				break;

				case syshotobj:					// Stompy's shot
					ShootActor (check, 5);
				break;

				case bgshotobj:					// Bug's shot
					ShootActor (check, 3);
				break;

				case eshotobj:						// Eye's shot
					ShootActor (check, 2);
				break;

				case gshotobj:
					ShootActor (check,25);		//NOLAN--check on me!!!!!!!
				break;

				case pshotobj:
					ShootActor (check,25);
				break;

			}
			ob->state = &s_pshot_exp1;
			ob->ticcount = s_pshot_exp1.tictime;
			return;
		}
}

//-------------------------------------------------------------------------
// AngleNearPlayer()
//-------------------------------------------------------------------------
int AngleNearPlayer(objtype *ob)
{
	int angle=-1;
	int xdiff = ob->tilex-player->tilex;
	int ydiff = ob->tiley-player->tiley;

	if (ob->tiley == player->tiley)
	{
		if (ob->tilex < player->tilex)
			angle = 0;
		else
			angle = 180;
	}
	else
	if (ob->tilex == player->tilex)
	{
		if (ob->tiley < player->tiley)
			angle = 270;
		else
			angle = 90;
	}
	else
	if (xdiff == ydiff)
		if (ob->tilex < player->tilex)
		{
			if (ob->tiley < player->tiley)
				angle = 315;
			else
				angle = 45;
		}
		else
		{
			if (ob->tiley < player->tiley)
				angle = 225;
			else
				angle = 135;
		}

	return(angle);
}



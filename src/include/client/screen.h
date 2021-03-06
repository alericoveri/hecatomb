/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * =======================================================================
 *
 * Header for the 2D client stuff and the .cinf file format
 *
 * =======================================================================
 */

 #ifndef CL_SCREEN_H
 #define CL_SCREEN_H

 #include "prereqs.h"

 /**
  *
  */
 void  SCR_Init ( void );

 /**
  * This is called every frame, and can also be called
  * explicitly to flush text to the screen.
  */
 void  SCR_UpdateScreen ( void );


 /**
  * Called for important messages that should stay
  * in the center of the screen for a few moments
  */
 void  SCR_CenterPrint ( char *str );

 /**
  *
  */
 void  SCR_BeginLoadingPlaque ( void );

 /**
  *
  */
 void  SCR_EndLoadingPlaque ( void );

 /**
  *
  */
 void  SCR_DebugGraph ( float value, q_int32_t color );

 /**
  * Allows rendering code to cache all needed sbar graphics
  */
 void  SCR_TouchPics ( void );

 /**
  * Scroll it up or down
  */
 void  SCR_RunConsole ( void );

 extern  float   scr_con_current;
 extern  float   scr_conlines; /* lines of console to display */

 extern  q_int32_t     sb_lines;

 extern  cvar_t    *scr_viewsize;
 extern  cvar_t    *crosshair;

 extern  vrect_t   scr_vrect; /* position of render window */

 extern  char    crosshair_pic[MAX_QPATH];
 extern  q_int32_t     crosshair_width, crosshair_height;

 /**
  *
  */
 void SCR_AddDirtyPoint ( q_int32_t x, q_int32_t y );

 /**
  *
  */
 void SCR_DirtyScreen ( void );

 /**
  *
  */
 void SCR_PlayCinematic ( char *name );

 /**
  *
  */
 qboolean SCR_DrawCinematic ( void );

 /**
  *
  */
 void SCR_RunCinematic ( void );

 /**
  *
  */
 void SCR_StopCinematic ( void );

 /**
  *
  */
 void SCR_FinishCinematic ( void );

 #endif /* CL_SCREEN_H */

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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * This file implements the 2D stuff. For example the HUD and the
 * networkgraph.
 *
 * =======================================================================
 */

 #include "prereqs.h"
 #include "system.h"
 #include "client.h"

 float scr_con_current; /* aproaches scr_conlines at scr_conspeed */
 float scr_conlines; /* 0.0 to 1.0 lines of console to display */

 qboolean scr_initialized; /* ready to draw */

 q_int32_t scr_draw_loading;

 vrect_t scr_vrect; /* position of render window on screen */

 cvar_t *scr_viewsize;
 cvar_t *scr_conspeed;
 cvar_t *scr_centertime;
 cvar_t *scr_showturtle;
 cvar_t *scr_showpause;

 cvar_t *scr_netgraph;
 cvar_t *scr_timegraph;
 cvar_t *scr_debuggraph;
 cvar_t *scr_graphheight;
 cvar_t *scr_graphscale;
 cvar_t *scr_graphshift;
 cvar_t *scr_drawall;

 typedef struct {
   q_int32_t x1, y1, x2, y2;
 } dirty_t;

 dirty_t scr_dirty, scr_old_dirty[2];

 char crosshair_pic[MAX_QPATH];
 q_int32_t crosshair_width, crosshair_height;

 extern cvar_t *cl_drawfps;
 extern cvar_t *crosshair_scale;

 void SCR_TimeRefresh_f ( void );
 void SCR_Loading_f ( void );

 /*
  * A new packet was just parsed
  */
 void
 CL_AddNetgraph ( void )
 {
   q_int32_t i;
   q_int32_t in;
   q_int32_t ping;

   /* if using the debuggraph for something
      else, don't add the net lines */
   if ( scr_debuggraph->value || scr_timegraph->value ) {
     return;
   }

   for ( i = 0; i < cls.netchan.dropped; i++ ) {
     SCR_DebugGraph ( 30, 0x40 );
   }

   for ( i = 0; i < cl.surpressCount; i++ ) {
     SCR_DebugGraph ( 30, 0xdf );
   }

   /* see what the latency was on this packet */
   in = cls.netchan.incoming_acknowledged & ( CMD_BACKUP - 1 );
   ping = cls.realtime - cl.cmd_time[in];
   ping /= 30;

   if ( ping > 30 ) {
     ping = 30;
   }

   SCR_DebugGraph ( ( float ) ping, 0xd0 );
 }

 typedef struct {
   float value;
   q_int32_t color;
 } graphsamp_t;

 static q_int32_t current;
 static graphsamp_t values[2024];

 /* ========================================================================= */
 void
 SCR_DebugGraph ( float value, q_int32_t color )
 {
   values[current & 2023].value = value;
   values[current & 2023].color = color;
   current++;
 }

 /* ========================================================================= */
 void
 SCR_DrawDebugGraph ( void )
 {
   q_int32_t a, x, y, w, i, h;
   float v;
   q_int32_t color;
   /* draw the graph */
   w = scr_vrect.width;
   x = scr_vrect.x;
   y = scr_vrect.y + scr_vrect.height;
   Draw_Fill ( x, y - scr_graphheight->value,
               w, scr_graphheight->value, 8 );

   for ( a = 0; a < w; a++ ) {
     i = ( current - 1 - a + 1024 ) & 1023;
     v = values[i].value;
     color = values[i].color;
     v = v * scr_graphscale->value + scr_graphshift->value;

     if ( v < 0 ) {
       v += scr_graphheight->value *
            ( 1 + ( q_int32_t ) ( -v / scr_graphheight->value ) );
     }

     h = ( q_int32_t ) v % ( q_int32_t ) scr_graphheight->value;
     Draw_Fill ( x + w - 1 - a, y - h, 1, h, color );
   }
 }

 char scr_centerstring[1024];
 float scr_centertime_start; /* for slow victory printing */
 float scr_centertime_off;
 q_int32_t scr_center_lines;
 q_int32_t scr_erase_center;

 /* ========================================================================= */
 void
 SCR_CenterPrint ( char *str )
 {
   char *s;
   char line[64];
   q_int32_t i, j, l;
   strncpy ( scr_centerstring, str, sizeof ( scr_centerstring ) - 1 );
   scr_centertime_off = scr_centertime->value;
   scr_centertime_start = cl.time;
   /* count the number of lines for centering */
   scr_center_lines = 1;
   s = str;

   while ( *s ) {
     if ( *s == '\n' ) {
       scr_center_lines++;
     }

     s++;
   }

   /* echo it to the console */
   Com_Printf ( "\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n\n" );
   s = str;

   do {
     /* scan the width of the line */
     for ( l = 0; l < 40; l++ ) {
       if ( ( s[l] == '\n' ) || !s[l] ) {
         break;
       }
     }

     for ( i = 0; i < ( 40 - l ) / 2; i++ ) {
       line[i] = ' ';
     }

     for ( j = 0; j < l; j++ ) {
       line[i++] = s[j];
     }

     line[i] = '\n';
     line[i + 1] = 0;
     Com_Printf ( "%s", line );

     while ( *s && *s != '\n' ) {
       s++;
     }

     if ( !*s ) {
       break;
     }

     s++; /* skip the \n */
   } while ( 1 );

   Com_Printf ( "\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n\n" );
   Con_ClearNotify();
 }

 /* ========================================================================= */
 void
 SCR_DrawCenterString ( void )
 {
   char *start;
   q_int32_t l;
   q_int32_t j;
   q_int32_t x, y;
   q_int32_t remaining;
   /* the finale prints the characters one at a time */
   remaining = 9999;
   scr_erase_center = 0;
   start = scr_centerstring;

   if ( scr_center_lines <= 4 ) {
     y = viddef.height * 0.35;
   } else {
     y = 48;
   }

   do {
     /* scan the width of the line */
     for ( l = 0; l < 40; l++ ) {
       if ( ( start[l] == '\n' ) || !start[l] ) {
         break;
       }
     }

     x = ( viddef.width - l * 8 ) / 2;
     SCR_AddDirtyPoint ( x, y );

     for ( j = 0; j < l; j++, x += 8 ) {
       Draw_Char ( x, y, start[j] );

       if ( !remaining-- ) {
         return;
       }
     }

     SCR_AddDirtyPoint ( x, y + 8 );
     y += 8;

     while ( *start && *start != '\n' ) {
       start++;
     }

     if ( !*start ) {
       break;
     }

     start++; /* skip the \n */
   } while ( 1 );
 }

 /* ========================================================================= */
 void
 SCR_CheckDrawCenterString ( void )
 {
   scr_centertime_off -= cls.frametime;

   if ( scr_centertime_off <= 0 ) {
     return;
   }

   SCR_DrawCenterString();
 }

 /*
  * Sets scr_vrect, the coordinates of the rendered window
  */
 static void
 SCR_CalcVrect ( void )
 {
   q_int32_t size;

   /* bound viewsize */
   if ( scr_viewsize->value < 40 ) {
     Cvar_Set ( "viewsize", "40" );
   }

   if ( scr_viewsize->value > 100 ) {
     Cvar_Set ( "viewsize", "100" );
   }

   size = scr_viewsize->value;
   scr_vrect.width = viddef.width * size / 100;
   scr_vrect.width &= ~7;
   scr_vrect.height = viddef.height * size / 100;
   scr_vrect.height &= ~1;
   scr_vrect.x = ( viddef.width - scr_vrect.width ) / 2;
   scr_vrect.y = ( viddef.height - scr_vrect.height ) / 2;
 }

 /*
  * Keybinding command
  */
 void
 SCR_SizeUp_f ( void )
 {
   Cvar_SetValue ( "viewsize", ( float ) scr_viewsize->value + 10 );
 }

 /*
  *Keybinding command
  */
 void
 SCR_SizeDown_f ( void )
 {
   Cvar_SetValue ( "viewsize", ( float ) scr_viewsize->value - 10 );
 }

 /*
  * Set a specific sky and rotation speed
  */
 void
 SCR_Sky_f ( void )
 {
   float rotate;
   vec3_t axis;

   if ( Cmd_Argc() < 2 ) {
     Com_Printf ( "Usage: sky <basename> <rotate> <axis x y z>\n" );
     return;
   }

   if ( Cmd_Argc() > 2 ) {
     rotate = ( float ) strtod ( Cmd_Argv ( 2 ), ( char ** ) NULL );
   } else {
     rotate = 0;
   }

   if ( Cmd_Argc() == 6 ) {
     axis[0] = ( float ) strtod ( Cmd_Argv ( 3 ), ( char ** ) NULL );
     axis[1] = ( float ) strtod ( Cmd_Argv ( 4 ), ( char ** ) NULL );
     axis[2] = ( float ) strtod ( Cmd_Argv ( 5 ), ( char ** ) NULL );
   } else {
     axis[0] = 0;
     axis[1] = 0;
     axis[2] = 1;
   }

   R_SetSky ( Cmd_Argv ( 1 ), rotate, axis );
 }

 /* ========================================================================= */
 void
 SCR_Init ( void )
 {
   scr_viewsize = Cvar_Get ( "viewsize", "100", CVAR_ARCHIVE );
   scr_conspeed = Cvar_Get ( "scr_conspeed", "3", 0 );
   scr_showturtle = Cvar_Get ( "scr_showturtle", "0", 0 );
   scr_showpause = Cvar_Get ( "scr_showpause", "1", 0 );
   scr_centertime = Cvar_Get ( "scr_centertime", "2.5", 0 );
   scr_netgraph = Cvar_Get ( "netgraph", "0", 0 );
   scr_timegraph = Cvar_Get ( "timegraph", "0", 0 );
   scr_debuggraph = Cvar_Get ( "debuggraph", "0", 0 );
   scr_graphheight = Cvar_Get ( "graphheight", "32", 0 );
   scr_graphscale = Cvar_Get ( "graphscale", "1", 0 );
   scr_graphshift = Cvar_Get ( "graphshift", "0", 0 );
   scr_drawall = Cvar_Get ( "scr_drawall", "0", 0 );
   /* register our commands */
   Cmd_AddCommand ( "timerefresh", SCR_TimeRefresh_f );
   Cmd_AddCommand ( "loading", SCR_Loading_f );
   Cmd_AddCommand ( "sizeup", SCR_SizeUp_f );
   Cmd_AddCommand ( "sizedown", SCR_SizeDown_f );
   Cmd_AddCommand ( "sky", SCR_Sky_f );
   scr_initialized = true;
 }

 /* ========================================================================= */
 void
 SCR_DrawNet ( void )
 {
   if ( cls.netchan.outgoing_sequence - cls.netchan.incoming_acknowledged < CMD_BACKUP - 1 ) {
     return;
   }

   Draw_Pic ( scr_vrect.x + 64, scr_vrect.y, "net" );
 }

 /* ========================================================================= */
 void
 SCR_DrawPause ( void )
 {
   q_int32_t w, h;

   if ( !scr_showpause->value ) { /* turn off for screenshots */
     return;
   }

   if ( !cl_paused->value ) {
     return;
   }

   Draw_GetPicSize ( &w, &h, "pause" );
   Draw_Pic ( ( viddef.width - w ) / 2, viddef.height / 2 + 8, "pause" );
 }

 /* ========================================================================= */
 void
 SCR_DrawLoading ( void )
 {
   q_int32_t w, h;

   if ( !scr_draw_loading ) {
     return;
   }

   scr_draw_loading = false;
   Draw_GetPicSize ( &w, &h, "loading" );
   Draw_Pic ( ( viddef.width - w ) / 2, ( viddef.height - h ) / 2, "loading" );
 }

 /* ========================================================================= */
 void
 SCR_RunConsole ( void )
 {
   /* decide on the height of the console */
   if ( cls.key_dest == key_console ) {
     scr_conlines = 0.5; /* half screen */
   } else {
     scr_conlines = 0; /* none visible */
   }

   if ( scr_conlines < scr_con_current ) {
     scr_con_current -= scr_conspeed->value * cls.frametime;

     if ( scr_conlines > scr_con_current ) {
       scr_con_current = scr_conlines;
     }
   } else if ( scr_conlines > scr_con_current ) {
     scr_con_current += scr_conspeed->value * cls.frametime;

     if ( scr_conlines < scr_con_current ) {
       scr_con_current = scr_conlines;
     }
   }
 }

 /* ========================================================================= */
 void
 SCR_DrawConsole ( void )
 {
   Con_CheckResize();

   if ( ( cls.state == ca_disconnected ) || ( cls.state == ca_connecting ) ) {
     /* forced full screen console */
     Con_DrawConsole ( 1.0 );
     return;
   }

   if ( ( cls.state != ca_active ) || !cl.refresh_prepped ) {
     /* connected, but can't render */
     Con_DrawConsole ( 0.5 );
     Draw_Fill ( 0, viddef.height / 2, viddef.width, viddef.height / 2, 0 );
     return;
   }

   if ( scr_con_current ) {
     Con_DrawConsole ( scr_con_current );
   } else {
     if ( ( cls.key_dest == key_game ) || ( cls.key_dest == key_message ) ) {
       Con_DrawNotify(); /* only draw notify in game */
     }
   }
 }

 /* ========================================================================= */
 void
 SCR_BeginLoadingPlaque ( void )
 {
   S_StopAllSounds();
   cl.sound_prepped = false; /* don't play ambients */

 #ifdef HT_WITH_OGG
   OGG_Stop();
 #endif

   if ( cls.disable_screen ) {
     return;
   }

   if ( developer->value ) {
     return;
   }

   if ( cls.state == ca_disconnected ) {
     /* if at console, don't bring up the plaque */
     return;
   }

   if ( cls.key_dest == key_console ) {
     return;
   }

   if ( cl.cinematictime > 0 ) {
     scr_draw_loading = 2; /* clear to balack first */
   } else {
     scr_draw_loading = 1;
   }

   SCR_UpdateScreen();
   SCR_StopCinematic();
   cls.disable_screen = Sys_Milliseconds();
   cls.disable_servercount = cl.servercount;
 }

 /* ========================================================================= */
 void
 SCR_EndLoadingPlaque ( void )
 {
   cls.disable_screen = 0;
   Con_ClearNotify();
 }

 /* ========================================================================= */
 void
 SCR_Loading_f ( void )
 {
   SCR_BeginLoadingPlaque();
 }

 /* ========================================================================= */
 q_int32_t
 entitycmpfnc ( const entity_t *a, const entity_t *b )
 {
   /* all other models are sorted by model then skin */
   if ( a->model == b->model ) {
     return ( INT ) a->skin - ( INT ) b->skin;
   } else {
     return ( INT ) a->model - ( INT ) b->model;
   }
 }

 /* ========================================================================= */
 void
 SCR_TimeRefresh_f ( void )
 {
   q_int32_t i;
   q_int32_t start, stop;
   float time;

   if ( cls.state != ca_active ) {
     return;
   }

   start = Sys_Milliseconds();

   if ( Cmd_Argc() == 2 ) {
     /* run without page flipping */
     q_int32_t j;

     for ( j = 0; j < 1000; j++ ) {
       R_BeginFrame ( 0 );

       for ( i = 0; i < 128; i++ ) {
         cl.refdef.viewangles[1] = i / 128.0f * 360.0f;
         R_RenderFrame ( &cl.refdef );
       }

       GLimp_EndFrame();
     }
   } else {
     for ( i = 0; i < 128; i++ ) {
       cl.refdef.viewangles[1] = i / 128.0f * 360.0f;
       R_BeginFrame ( 0 );
       R_RenderFrame ( &cl.refdef );
       GLimp_EndFrame();
     }
   }

   stop = Sys_Milliseconds();
   time = ( stop - start ) / 1000.0f;
   Com_Printf ( "%f seconds (%f fps)\n", time, 128 / time );
 }

 /* ========================================================================= */
 void
 SCR_AddDirtyPoint ( q_int32_t x, q_int32_t y )
 {
   if ( x < scr_dirty.x1 ) {
     scr_dirty.x1 = x;
   }

   if ( x > scr_dirty.x2 ) {
     scr_dirty.x2 = x;
   }

   if ( y < scr_dirty.y1 ) {
     scr_dirty.y1 = y;
   }

   if ( y > scr_dirty.y2 ) {
     scr_dirty.y2 = y;
   }
 }

 /* ========================================================================= */
 void
 SCR_DirtyScreen ( void )
 {
   SCR_AddDirtyPoint ( 0, 0 );
   SCR_AddDirtyPoint ( viddef.width - 1, viddef.height - 1 );
 }

 /*
  * Clear any parts of the tiled background that were drawn on last frame
  */
 void
 SCR_TileClear ( void )
 {
   q_int32_t i;
   q_int32_t top, bottom, left, right;
   dirty_t clear;

   if ( scr_con_current == 1.0 ) {
     return; /* full screen console */
   }

   if ( scr_viewsize->value == 100 ) {
     return; /* full screen rendering */
   }

   if ( cl.cinematictime > 0 ) {
     return; /* full screen cinematic */
   }

   /* erase rect will be the union of the past three
      frames so tripple buffering works properly */
   clear = scr_dirty;

   for ( i = 0; i < 2; i++ ) {
     if ( scr_old_dirty[i].x1 < clear.x1 ) {
       clear.x1 = scr_old_dirty[i].x1;
     }

     if ( scr_old_dirty[i].x2 > clear.x2 ) {
       clear.x2 = scr_old_dirty[i].x2;
     }

     if ( scr_old_dirty[i].y1 < clear.y1 ) {
       clear.y1 = scr_old_dirty[i].y1;
     }

     if ( scr_old_dirty[i].y2 > clear.y2 ) {
       clear.y2 = scr_old_dirty[i].y2;
     }
   }

   scr_old_dirty[1] = scr_old_dirty[0];
   scr_old_dirty[0] = scr_dirty;
   scr_dirty.x1 = 9999;
   scr_dirty.x2 = -9999;
   scr_dirty.y1 = 9999;
   scr_dirty.y2 = -9999;
   /* don't bother with anything convered by the console */
   top = ( q_int32_t ) ( scr_con_current * viddef.height );

   if ( top >= clear.y1 ) {
     clear.y1 = top;
   }

   if ( clear.y2 <= clear.y1 ) {
     return; /* nothing disturbed */
   }

   top = scr_vrect.y;
   bottom = top + scr_vrect.height - 1;
   left = scr_vrect.x;
   right = left + scr_vrect.width - 1;

   if ( clear.y1 < top ) {
     /* clear above view screen */
     i = clear.y2 < top - 1 ? clear.y2 : top - 1;
     Draw_TileClear ( clear.x1, clear.y1,
                      clear.x2 - clear.x1 + 1, i - clear.y1 + 1, "backtile" );
     clear.y1 = top;
   }

   if ( clear.y2 > bottom ) {
     /* clear below view screen */
     i = clear.y1 > bottom + 1 ? clear.y1 : bottom + 1;
     Draw_TileClear ( clear.x1, i,
                      clear.x2 - clear.x1 + 1, clear.y2 - i + 1, "backtile" );
     clear.y2 = bottom;
   }

   if ( clear.x1 < left ) {
     /* clear left of view screen */
     i = clear.x2 < left - 1 ? clear.x2 : left - 1;
     Draw_TileClear ( clear.x1, clear.y1,
                      i - clear.x1 + 1, clear.y2 - clear.y1 + 1, "backtile" );
     clear.x1 = left;
   }

   if ( clear.x2 > right ) {
     /* clear left of view screen */
     i = clear.x1 > right + 1 ? clear.x1 : right + 1;
     Draw_TileClear ( i, clear.y1,
                      clear.x2 - i + 1, clear.y2 - clear.y1 + 1, "backtile" );
     clear.x2 = right;
   }
 }

 #define STAT_MINUS 10
 char *sb_nums[2][11] = {
   {
     "num_0", "num_1", "num_2", "num_3", "num_4", "num_5",
     "num_6", "num_7", "num_8", "num_9", "num_minus"
   },
   {
     "anum_0", "anum_1", "anum_2", "anum_3", "anum_4", "anum_5",
     "anum_6", "anum_7", "anum_8", "anum_9", "anum_minus"
   }
 };

 #define ICON_WIDTH 24
 #define ICON_HEIGHT 24
 #define CHAR_WIDTH 16
 #define ICON_SPACE 8

 /*
  * Allow embedded \n in the string
  */
 void
 SizeHUDString ( char *string, q_int32_t *w, q_int32_t *h )
 {
   q_int32_t lines, width, current;
   lines = 1;
   width = 0;
   current = 0;

   while ( *string ) {
     if ( *string == '\n' ) {
       lines++;
       current = 0;
     } else {
       current++;

       if ( current > width ) {
         width = current;
       }
     }

     string++;
   }

   *w = width * 8;
   *h = lines * 8;
 }

 /* ========================================================================= */
 void
 DrawHUDString ( char *string, q_int32_t x, q_int32_t y, q_int32_t centerwidth, q_int32_t xor )
 {
   q_int32_t margin;
   char line[1024];
   q_int32_t width;
   q_int32_t i;
   margin = x;

   while ( *string ) {
     /* scan out one line of text from the string */
     width = 0;

     while ( *string && *string != '\n' ) {
       line[width++] = *string++;
     }

     line[width] = 0;

     if ( centerwidth ) {
       x = margin + ( centerwidth - width * 8 ) / 2;
     } else {
       x = margin;
     }

     for ( i = 0; i < width; i++ ) {
       Draw_Char ( x, y, line[i] ^ xor );
       x += 8;
     }

     if ( *string ) {
       string++; /* skip the \n */
       y += 8;
     }
   }
 }

 /* ========================================================================= */
 void
 SCR_DrawField ( q_int32_t x, q_int32_t y, q_int32_t color, q_int32_t width, q_int32_t value )
 {
   char num[16], *ptr;
   q_int32_t l;
   q_int32_t frame;

   if ( width < 1 ) {
     return;
   }

   /* draw number string */
   if ( width > 5 ) {
     width = 5;
   }

   SCR_AddDirtyPoint ( x, y );
   SCR_AddDirtyPoint ( x + width * CHAR_WIDTH + 2, y + 23 );
   Com_sprintf ( num, sizeof ( num ), "%i", value );
   l = ( q_int32_t ) strlen ( num );

   if ( l > width ) {
     l = width;
   }

   x += 2 + CHAR_WIDTH * ( width - l );
   ptr = num;

   while ( *ptr && l ) {
     if ( *ptr == '-' ) {
       frame = STAT_MINUS;
     } else {
       frame = *ptr - '0';
     }

     Draw_Pic ( x, y, sb_nums[color][frame] );
     x += CHAR_WIDTH;
     ptr++;
     l--;
   }
 }

 /* ========================================================================= */
 void
 SCR_TouchPics ( void )
 {
   q_int32_t i, j;

   for ( i = 0; i < 2; i++ ) {
     for ( j = 0; j < 11; j++ ) {
       Draw_FindPic ( sb_nums[i][j] );
     }
   }

   if ( crosshair->value ) {
     if ( ( crosshair->value > 3 ) || ( crosshair->value < 0 ) ) {
       crosshair->value = 3;
     }

     Com_sprintf ( crosshair_pic, sizeof ( crosshair_pic ), "ch%i",
                   ( q_int32_t ) ( crosshair->value ) );
     Draw_GetPicSize ( &crosshair_width, &crosshair_height, crosshair_pic );

     if ( !crosshair_width ) {
       crosshair_pic[0] = 0;
     }
   }
 }

 /* ========================================================================= */
 void
 SCR_ExecuteLayoutString ( char *s )
 {
   q_int32_t x, y;
   q_int32_t value;
   char *token;
   q_int32_t width;
   q_int32_t index;
   clientinfo_t *ci;

   if ( ( cls.state != ca_active ) || !cl.refresh_prepped ) {
     return;
   }

   if ( !s[0] ) {
     return;
   }

   x = 0;
   y = 0;

   while ( s ) {
     token = COM_Parse ( &s );

     if ( !strcmp ( token, "xl" ) ) {
       token = COM_Parse ( &s );
       x = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       continue;
     }

     if ( !strcmp ( token, "xr" ) ) {
       token = COM_Parse ( &s );
       x = viddef.width + ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       continue;
     }

     if ( !strcmp ( token, "xv" ) ) {
       token = COM_Parse ( &s );
       x = viddef.width / 2 - 160 + ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       continue;
     }

     if ( !strcmp ( token, "yt" ) ) {
       token = COM_Parse ( &s );
       y = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       continue;
     }

     if ( !strcmp ( token, "yb" ) ) {
       token = COM_Parse ( &s );
       y = viddef.height + ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       continue;
     }

     if ( !strcmp ( token, "yv" ) ) {
       token = COM_Parse ( &s );
       y = viddef.height / 2 - 120 + ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       continue;
     }

     if ( !strcmp ( token, "pic" ) ) {
       /* draw a pic from a stat number */
       token = COM_Parse ( &s );
       index = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );

       if ( ( index < 0 ) || ( index >= sizeof ( cl.frame.playerstate.stats ) ) ) {
         Com_Error ( ERR_DROP, "bad stats index %d (0x%x)", index, index );
       }

       value = cl.frame.playerstate.stats[index];

       if ( value >= MAX_IMAGES ) {
         Com_Error ( ERR_DROP, "Pic >= MAX_IMAGES" );
       }

       if ( cl.configstrings[CS_IMAGES + value] ) {
         SCR_AddDirtyPoint ( x, y );
         SCR_AddDirtyPoint ( x + 23, y + 23 );
         Draw_Pic ( x, y, cl.configstrings[CS_IMAGES + value] );
       }

       continue;
     }

     if ( !strcmp ( token, "client" ) ) {
       /* draw a deathmatch client block */
       q_int32_t score, ping, time;
       token = COM_Parse ( &s );
       x = viddef.width / 2 - 160 + ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       token = COM_Parse ( &s );
       y = viddef.height / 2 - 120 + ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       SCR_AddDirtyPoint ( x, y );
       SCR_AddDirtyPoint ( x + 159, y + 31 );
       token = COM_Parse ( &s );
       value = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );

       if ( ( value >= MAX_CLIENTS ) || ( value < 0 ) ) {
         Com_Error ( ERR_DROP, "client >= MAX_CLIENTS" );
       }

       ci = &cl.clientinfo[value];
       token = COM_Parse ( &s );
       score = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       token = COM_Parse ( &s );
       ping = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       token = COM_Parse ( &s );
       time = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       DrawAltString ( x + 32, y, ci->name );
       DrawString ( x + 32, y + 8, "Score: " );
       DrawAltString ( x + 32 + 7 * 8, y + 8, va ( "%i", score ) );
       DrawString ( x + 32, y + 16, va ( "Ping:  %i", ping ) );
       DrawString ( x + 32, y + 24, va ( "Time:  %i", time ) );

       if ( !ci->icon ) {
         ci = &cl.baseclientinfo;
       }

       Draw_Pic ( x, y, ci->iconname );
       continue;
     }

     if ( !strcmp ( token, "ctf" ) ) {
       /* draw a ctf client block */
       q_int32_t score, ping;
       char block[80];
       token = COM_Parse ( &s );
       x = viddef.width / 2 - 160 + ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       token = COM_Parse ( &s );
       y = viddef.height / 2 - 120 + ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       SCR_AddDirtyPoint ( x, y );
       SCR_AddDirtyPoint ( x + 159, y + 31 );
       token = COM_Parse ( &s );
       value = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );

       if ( ( value >= MAX_CLIENTS ) || ( value < 0 ) ) {
         Com_Error ( ERR_DROP, "client >= MAX_CLIENTS" );
       }

       ci = &cl.clientinfo[value];
       token = COM_Parse ( &s );
       score = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       token = COM_Parse ( &s );
       ping = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );

       if ( ping > 999 ) {
         ping = 999;
       }

       sprintf ( block, "%3d %3d %-12.12s", score, ping, ci->name );

       if ( value == cl.playernum ) {
         DrawAltString ( x, y, block );
       } else {
         DrawString ( x, y, block );
       }

       continue;
     }

     if ( !strcmp ( token, "picn" ) ) {
       /* draw a pic from a name */
       token = COM_Parse ( &s );
       SCR_AddDirtyPoint ( x, y );
       SCR_AddDirtyPoint ( x + 23, y + 23 );
       Draw_Pic ( x, y, ( char * ) token );
       continue;
     }

     if ( !strcmp ( token, "num" ) ) {
       /* draw a number */
       token = COM_Parse ( &s );
       width = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );
       token = COM_Parse ( &s );
       value = cl.frame.playerstate.stats[ ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 )];
       SCR_DrawField ( x, y, 0, width, value );
       continue;
     }

     if ( !strcmp ( token, "hnum" ) ) {
       /* health number */
       q_int32_t color;
       width = 3;
       value = cl.frame.playerstate.stats[STAT_HEALTH];

       if ( value > 25 ) {
         color = 0;  /* green */
       } else if ( value > 0 ) {
         color = ( cl.frame.serverframe >> 2 ) & 1; /* flash */
       } else {
         color = 1;
       }

       if ( cl.frame.playerstate.stats[STAT_FLASHES] & 1 ) {
         Draw_Pic ( x, y, "field_3" );
       }

       SCR_DrawField ( x, y, color, width, value );
       continue;
     }

     if ( !strcmp ( token, "anum" ) ) {
       /* ammo number */
       q_int32_t color;
       width = 3;
       value = cl.frame.playerstate.stats[STAT_AMMO];

       if ( value > 5 ) {
         color = 0; /* green */
       } else if ( value >= 0 ) {
         color = ( cl.frame.serverframe >> 2 ) & 1; /* flash */
       } else {
         continue; /* negative number = don't show */
       }

       if ( cl.frame.playerstate.stats[STAT_FLASHES] & 4 ) {
         Draw_Pic ( x, y, "field_3" );
       }

       SCR_DrawField ( x, y, color, width, value );
       continue;
     }

     if ( !strcmp ( token, "rnum" ) ) {
       /* armor number */
       q_int32_t color;
       width = 3;
       value = cl.frame.playerstate.stats[STAT_ARMOR];

       if ( value < 1 ) {
         continue;
       }

       color = 0; /* green */

       if ( cl.frame.playerstate.stats[STAT_FLASHES] & 2 ) {
         Draw_Pic ( x, y, "field_3" );
       }

       SCR_DrawField ( x, y, color, width, value );
       continue;
     }

     if ( !strcmp ( token, "stat_string" ) ) {
       token = COM_Parse ( &s );
       index = ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 );

       if ( ( index < 0 ) || ( index >= MAX_CONFIGSTRINGS ) ) {
         Com_Error ( ERR_DROP, "Bad stat_string index" );
       }

       index = cl.frame.playerstate.stats[index];

       if ( ( index < 0 ) || ( index >= MAX_CONFIGSTRINGS ) ) {
         Com_Error ( ERR_DROP, "Bad stat_string index" );
       }

       DrawString ( x, y, cl.configstrings[index] );
       continue;
     }

     if ( !strcmp ( token, "cstring" ) ) {
       token = COM_Parse ( &s );
       DrawHUDString ( token, x, y, 320, 0 );
       continue;
     }

     if ( !strcmp ( token, "string" ) ) {
       token = COM_Parse ( &s );
       DrawString ( x, y, token );
       continue;
     }

     if ( !strcmp ( token, "cstring2" ) ) {
       token = COM_Parse ( &s );
       DrawHUDString ( token, x, y, 320, 0x80 );
       continue;
     }

     if ( !strcmp ( token, "string2" ) ) {
       token = COM_Parse ( &s );
       DrawAltString ( x, y, token );
       continue;
     }

     if ( !strcmp ( token, "if" ) ) {
       /* draw a number */
       token = COM_Parse ( &s );
       value = cl.frame.playerstate.stats[ ( q_int32_t ) strtol ( token, ( char ** ) NULL, 10 )];

       if ( !value ) {
         /* skip to endif */
         while ( s && strcmp ( token, "endif" ) ) {
           token = COM_Parse ( &s );
         }
       }

       continue;
     }
   }
 }

 /*
  * The status bar is a small layout program that
  * is based on the stats array
  */
 void
 SCR_DrawStats ( void )
 {
   SCR_ExecuteLayoutString ( cl.configstrings[CS_STATUSBAR] );
 }

 #define STAT_LAYOUTS 13

 /* ========================================================================= */
 void
 SCR_DrawLayout ( void )
 {
   if ( !cl.frame.playerstate.stats[STAT_LAYOUTS] ) {
     return;
   }

   SCR_ExecuteLayoutString ( cl.layout );
 }

 /* ========================================================================= */
 void
 SCR_UpdateScreen ( void )
 {
   q_int32_t numframes;
   q_int32_t i;
   float separation[2] = {0, 0};

   /* if the screen is disabled (loading plaque is
      up, or vid mode changing) do nothing at all */
   if ( cls.disable_screen ) {
     if ( Sys_Milliseconds() - cls.disable_screen > 120000 ) {
       cls.disable_screen = 0;
       Com_Printf ( "Loading plaque timed out.\n" );
     }

     return;
   }

   if ( !scr_initialized || !con.initialized ) {
     return; /* not initialized yet */
   }

   separation[0] = 0;
   separation[1] = 0;
   numframes = 1;

   for ( i = 0; i < numframes; i++ ) {
     R_BeginFrame ( separation[i] );

     if ( scr_draw_loading == 2 ) {
       /* loading plaque over black screen */
       q_int32_t w, h;
       R_SetPalette ( NULL );
       scr_draw_loading = false;
       Draw_GetPicSize ( &w, &h, "loading" );
       Draw_Pic ( ( viddef.width - w ) / 2, ( viddef.height - h ) / 2, "loading" );
     }
     /* if a cinematic is supposed to be running,
        handle menus and console specially */
     else if ( cl.cinematictime > 0 ) {
       if ( cls.key_dest == key_menu ) {
         if ( cl.cinematicpalette_active ) {
           R_SetPalette ( NULL );
           cl.cinematicpalette_active = false;
         }

         M_Draw();
       } else if ( cls.key_dest == key_console ) {
         if ( cl.cinematicpalette_active ) {
           R_SetPalette ( NULL );
           cl.cinematicpalette_active = false;
         }

         SCR_DrawConsole();
       } else {
         SCR_DrawCinematic();
       }
     } else {
       /* make sure the game palette is active */
       if ( cl.cinematicpalette_active ) {
         R_SetPalette ( NULL );
         cl.cinematicpalette_active = false;
       }

       /* do 3D refresh drawing, and then update the screen */
       SCR_CalcVrect();
       /* clear any dirty part of the background */
       SCR_TileClear();
       V_RenderView ( separation[i] );
       SCR_DrawStats();

       if ( cl.frame.playerstate.stats[STAT_LAYOUTS] & 1 ) {
         SCR_DrawLayout();
       }

       if ( cl.frame.playerstate.stats[STAT_LAYOUTS] & 2 ) {
         CL_DrawInventory();
       }

       SCR_DrawNet();
       SCR_CheckDrawCenterString();

       if ( cl_drawfps->value ) {
         char s[8];
         sprintf ( s, "%3.0ffps", 1 / cls.frametime );
         DrawString ( viddef.width - 64, 0, s );
       }

       if ( scr_timegraph->value ) {
         SCR_DebugGraph ( cls.frametime * 300, 0 );
       }

       if ( scr_debuggraph->value || scr_timegraph->value ||
            scr_netgraph->value ) {
         SCR_DrawDebugGraph();
       }

       SCR_DrawPause();
       SCR_DrawConsole();
       M_Draw();
       SCR_DrawLoading();
     }
   }

   GLimp_EndFrame();
 }

 /* ========================================================================= */
 void
 SCR_DrawCrosshair ( void )
 {
   if ( !crosshair->value ) {
     return;
   }

   if ( crosshair->modified ) {
     crosshair->modified = false;
     SCR_TouchPics();
   }

   if ( crosshair_scale->modified ) {
     crosshair_scale->modified = false;

     if ( crosshair_scale->value > 5 ) {
       Cvar_SetValue ( "crosshair_scale", 5 );
     } else if ( crosshair_scale->value < 0.25 ) {
       Cvar_SetValue ( "crosshair_scale", 0.25 );
     }
   }

   if ( !crosshair_pic[0] ) {
     return;
   }

   Draw_Pic ( scr_vrect.x + ( ( scr_vrect.width - crosshair_width ) >> 1 ),
              scr_vrect.y + ( ( scr_vrect.height - crosshair_height ) >> 1 ),
              crosshair_pic );
 }

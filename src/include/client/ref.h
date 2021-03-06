/*
 * Copyright (C) 2013 Alejandro Ricoveri
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
 * ABI between client and refresher
 *
 * =======================================================================
 */

 #ifndef CL_REF_H
 #define CL_REF_H

 #include "prereqs.h"
 #include "client/vid.h"

 #define MAX_DLIGHTS   32
 #define MAX_ENTITIES  128
 #define MAX_PARTICLES 4096
 #define MAX_LIGHTSTYLES 256

 #define POWERSUIT_SCALE   4.0F

 #define SHELL_RED_COLOR   0xF2
 #define SHELL_GREEN_COLOR 0xD0
 #define SHELL_BLUE_COLOR  0xF3

 #define SHELL_RG_COLOR    0xDC
 #define SHELL_RB_COLOR    0x68
 #define SHELL_BG_COLOR    0x78

 #define SHELL_DOUBLE_COLOR    0xDF
 #define SHELL_HALF_DAM_COLOR  0x90
 #define SHELL_CYAN_COLOR    0x72

 #define SHELL_WHITE_COLOR 0xD7

 #define ENTITY_FLAGS  68
 #define API_VERSION   3

/**
  *
  */
 typedef struct entity_s {
   struct model_s    *model; /* opaque type outside refresh */
   float       angles[3];

   /* most recent data */
   float       origin[3]; /* also used as RF_BEAM's "from" */
   q_int32_t         frame; /* also used as RF_BEAM's diameter */

   /* previous data for lerping */
   float       oldorigin[3]; /* also used as RF_BEAM's "to" */
   q_int32_t         oldframe;

   /* misc */
   float backlerp; /* 0.0 = current, 1.0 = old */
   q_int32_t   skinnum; /* also used as RF_BEAM's palette index */

   q_int32_t   lightstyle; /* for flashing entities */
   float alpha; /* ignore if RF_TRANSLUCENT isn't set */

   struct image_s  *skin; /* NULL for inline skin */
   q_int32_t   flags;
 } entity_t;

/**
  *
  */
 typedef struct {
   vec3_t  origin;
   vec3_t  color;
   float intensity;
 } dlight_t;

/**
  *
  */
 typedef struct {
   vec3_t  origin;
   q_int32_t   color;
   float alpha;
 } particle_t;

/**
  *
  */
 typedef struct {
   float   rgb[3]; /* 0.0 - 2.0 */
   float   white; /* highest of rgb */
 } lightstyle_t;

/**
  *
  */
 typedef struct {
   q_int32_t     x, y, width, height; /* in virtual screen coordinates */
   float   fov_x, fov_y;
   float   vieworg[3];
   float   viewangles[3];
   float   blend[4]; /* rgba 0-1 full screen blend */
   float   time; /* time is uesed to auto animate */
   q_int32_t     rdflags; /* RDF_UNDERWATER, etc */

   byte    *areabits; /* if not NULL, only areas with set bits will be drawn */

   lightstyle_t  *lightstyles; /* [MAX_LIGHTSTYLES] */

   q_int32_t     num_entities;
   entity_t  *entities;

   q_int32_t     num_dlights;
   dlight_t  *dlights;

   q_int32_t     num_particles;
   particle_t  *particles;
 } refdef_t;

 /*
  * Refresh public API
  */

 /**
  * Specifies the model that will be used as the world
  */
 void R_BeginRegistration ( char *map );

 /**
  *
  */
 void R_Clear ( void );

 /**
  *
  */
 struct model_s *R_RegisterModel ( char *name );

 /**
  *
  */
 struct image_s *R_RegisterSkin ( char *name );

 /**
  *
  */
 void R_SetSky ( char *name, float rotate, vec3_t axis );

 /**
  *
  */
 void R_EndRegistration ( void );

 /**
  *
  */
 struct image_s *Draw_FindPic ( char *name );

 /**
  *
  */
 void R_RenderFrame ( refdef_t *fd );

 /**
  *
  */
 void Draw_GetPicSize ( q_int32_t *w, q_int32_t *h, char *name );

 /**
  *
  */
 void Draw_Pic ( q_int32_t x, q_int32_t y, char *name );

 /**
  *
  */
 void Draw_StretchPic ( q_int32_t x, q_int32_t y, q_int32_t w, q_int32_t h, char *name );

 /**
  * Draws one 8*8 graphics character with 0 being transparent.
  * It can be clipped to the top of the screen to allow the console to be
  * smoothly scrolled off.
  */
 void Draw_Char ( q_int32_t x, q_int32_t y, q_int32_t c );

 /**
  * This repeats a 64*64 tile graphic to fill
  * the screen around a sized down
  * refresh window.
  */
 void Draw_TileClear ( q_int32_t x, q_int32_t y, q_int32_t w, q_int32_t h, char *name );

 /**
  * Fills a box of pixels with a single color
  */
 void Draw_Fill ( q_int32_t x, q_int32_t y, q_int32_t w, q_int32_t h, q_int32_t c );

 /**
  *
  */
 void Draw_FadeScreen ( void );

 /**
  *
  */
 void Draw_StretchRaw ( q_int32_t x, q_int32_t y, q_int32_t w, q_int32_t h, q_int32_t cols, q_int32_t rows, byte *data );

 /**
  * Initiate the refresher
  */
 q_int32_t R_Init ( void *hinstance, void *hWnd );

 /**
  * Shutdown the refresher
  */
 void R_Shutdown ( void );

 /**
  *
  */
 void R_SetPalette ( const q_uint8_t *palette );

 /**
  *
  */
 void R_BeginFrame ( float camera_separation );

 /**
  * Swaps the buffers to show the new frame
  */
 void GLimp_EndFrame ( void );

 /**
  * Sets the hardware gamma
  */
 void GLimp_UpdateGamma ( void );

 #endif /* CL_REF_H */

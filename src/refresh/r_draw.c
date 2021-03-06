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
 * Drawing of all images that are not textures
 *
 * =======================================================================
 */

 #include "prereqs.h"
 #include "memory.h"
 #include "refresh/local.h"

 image_t *draw_chars;

 extern qboolean scrap_dirty;
 void Scrap_Upload ( void );

 extern q_uint32_t r_rawpalette[256];

 /* ========================================================================= */
 void
 Draw_InitLocal ( void )
 {
   /* load console characters (don't bilerp characters) */
   draw_chars = R_FindImage ( "pics/conchars.pcx", it_pic );
   R_Bind ( draw_chars->texnum );
   qglTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   qglTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
 }

 /* ========================================================================= */
 void
 Draw_Char ( q_int32_t x, q_int32_t y, q_int32_t num )
 {
   q_int32_t row, col;
   float frow, fcol, size;
   num &= 255;

   if ( ( num & 127 ) == 32 ) {
     return; /* space */
   }

   if ( y <= -8 ) {
     return; /* totally off screen */
   }

   row = num >> 4;
   col = num & 15;
   frow = row * 0.0625;
   fcol = col * 0.0625;
   size = 0.0625;
   R_Bind ( draw_chars->texnum );
   qglBegin ( GL_QUADS );
   qglTexCoord2f ( fcol, frow );
   qglVertex2f ( x, y );
   qglTexCoord2f ( fcol + size, frow );
   qglVertex2f ( x + 8, y );
   qglTexCoord2f ( fcol + size, frow + size );
   qglVertex2f ( x + 8, y + 8 );
   qglTexCoord2f ( fcol, frow + size );
   qglVertex2f ( x, y + 8 );
   qglEnd();
 }

 /* ========================================================================= */
 image_t *
 Draw_FindPic ( char *name )
 {
   image_t *gl;
   char fullname[MAX_QPATH];

   if ( ( name[0] != '/' ) && ( name[0] != '\\' ) ) {
     Com_sprintf ( fullname, sizeof ( fullname ), "pics/%s.pcx", name );
     gl = R_FindImage ( fullname, it_pic );
   } else {
     gl = R_FindImage ( name + 1, it_pic );
   }

   return gl;
 }

 /* ========================================================================= */
 void
 Draw_GetPicSize ( q_int32_t *w, q_int32_t *h, char *pic )
 {
   image_t *gl;
   gl = Draw_FindPic ( pic );

   if ( !gl ) {
     *w = *h = -1;
     return;
   }

   *w = gl->width;
   *h = gl->height;
 }

 /* ========================================================================= */
 void
 Draw_StretchPic ( q_int32_t x, q_int32_t y, q_int32_t w, q_int32_t h, char *pic )
 {
   image_t *gl;
   gl = Draw_FindPic ( pic );

   if ( !gl ) {
     VID_Printf ( PRINT_ALL, "Can't find pic: %s\n", pic );
     return;
   }

   if ( scrap_dirty ) {
     Scrap_Upload();
   }

   R_Bind ( gl->texnum );
   qglBegin ( GL_QUADS );
   qglTexCoord2f ( gl->sl, gl->tl );
   qglVertex2f ( x, y );
   qglTexCoord2f ( gl->sh, gl->tl );
   qglVertex2f ( x + w, y );
   qglTexCoord2f ( gl->sh, gl->th );
   qglVertex2f ( x + w, y + h );
   qglTexCoord2f ( gl->sl, gl->th );
   qglVertex2f ( x, y + h );
   qglEnd();
 }

 /* ========================================================================= */
 void
 Draw_Pic ( q_int32_t x, q_int32_t y, char *pic )
 {
   image_t *gl;
   gl = Draw_FindPic ( pic );

   if ( !gl ) {
     VID_Printf ( PRINT_ALL, "Can't find pic: %s\n", pic );
     return;
   }

   if ( scrap_dirty ) {
     Scrap_Upload();
   }

   R_Bind ( gl->texnum );
   qglBegin ( GL_QUADS );
   qglTexCoord2f ( gl->sl, gl->tl );
   qglVertex2f ( x, y );
   qglTexCoord2f ( gl->sh, gl->tl );
   qglVertex2f ( x + gl->width, y );
   qglTexCoord2f ( gl->sh, gl->th );
   qglVertex2f ( x + gl->width, y + gl->height );
   qglTexCoord2f ( gl->sl, gl->th );
   qglVertex2f ( x, y + gl->height );
   qglEnd();
 }

 /* ========================================================================= */
 void
 Draw_TileClear ( q_int32_t x, q_int32_t y, q_int32_t w, q_int32_t h, char *pic )
 {
   image_t *image;
   image = Draw_FindPic ( pic );

   if ( !image ) {
     VID_Printf ( PRINT_ALL, "Can't find pic: %s\n", pic );
     return;
   }

   R_Bind ( image->texnum );
   qglBegin ( GL_QUADS );
   qglTexCoord2f ( x / 64.0, y / 64.0 );
   qglVertex2f ( x, y );
   qglTexCoord2f ( ( x + w ) / 64.0, y / 64.0 );
   qglVertex2f ( x + w, y );
   qglTexCoord2f ( ( x + w ) / 64.0, ( y + h ) / 64.0 );
   qglVertex2f ( x + w, y + h );
   qglTexCoord2f ( x / 64.0, ( y + h ) / 64.0 );
   qglVertex2f ( x, y + h );
   qglEnd();
 }

 /* ========================================================================= */
 void
 Draw_Fill ( q_int32_t x, q_int32_t y, q_int32_t w, q_int32_t h, q_int32_t c )
 {
   union {
     q_uint32_t c;
     byte v[4];
   } color;

   if ( ( q_uint32_t ) c > 255 ) {
     VID_Error ( ERR_FATAL, "Draw_Fill: bad color" );
   }

   qglDisable ( GL_TEXTURE_2D );
   color.c = d_8to24table[c];
   qglColor3f ( color.v[0] / 255.0, color.v[1] / 255.0,
                color.v[2] / 255.0 );
   qglBegin ( GL_QUADS );
   qglVertex2f ( x, y );
   qglVertex2f ( x + w, y );
   qglVertex2f ( x + w, y + h );
   qglVertex2f ( x, y + h );
   qglEnd();
   qglColor3f ( 1, 1, 1 );
   qglEnable ( GL_TEXTURE_2D );
 }

 /* ========================================================================= */
 void
 Draw_FadeScreen ( void )
 {
   qglEnable ( GL_BLEND );
   qglDisable ( GL_TEXTURE_2D );
   qglColor4f ( 0, 0, 0, 0.8 );
   qglBegin ( GL_QUADS );
   qglVertex2f ( 0, 0 );
   qglVertex2f ( vid.width, 0 );
   qglVertex2f ( vid.width, vid.height );
   qglVertex2f ( 0, vid.height );
   qglEnd();
   qglColor4f ( 1, 1, 1, 1 );
   qglEnable ( GL_TEXTURE_2D );
   qglDisable ( GL_BLEND );
 }

 /* ========================================================================= */
 void
 Draw_StretchRaw ( q_int32_t x, q_int32_t y, q_int32_t w, q_int32_t h, q_int32_t cols, q_int32_t rows, byte *data )
 {
   q_uint32_t image32[256 * 256];
   q_uint8_t image8[256 * 256];
   q_int32_t i, j, trows;
   byte *source;
   q_int32_t frac, fracstep;
   float hscale;
   q_int32_t row;
   float t;
   R_Bind ( 0 );

   if ( rows <= 256 ) {
     hscale = 1;
     trows = rows;
   } else {
     hscale = rows / 256.0;
     trows = 256;
   }

   t = rows * hscale / 256 - 1.0 / 512.0;

   if ( !qglColorTableEXT ) {
     q_uint32_t *dest;

     for ( i = 0; i < trows; i++ ) {
       row = ( q_int32_t ) ( i * hscale );

       if ( row > rows ) {
         break;
       }

       source = data + cols * row;
       dest = &image32[i * 256];
       fracstep = cols * 0x10000 / 256;
       frac = fracstep >> 1;

       for ( j = 0; j < 256; j++ ) {
         dest[j] = r_rawpalette[source[frac >> 16]];
         frac += fracstep;
       }
     }

     qglTexImage2D ( GL_TEXTURE_2D, 0, gl_tex_solid_format,
                     256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     image32 );
   } else {
     q_uint8_t *dest;

     for ( i = 0; i < trows; i++ ) {
       row = ( q_int32_t ) ( i * hscale );

       if ( row > rows ) {
         break;
       }

       source = data + cols * row;
       dest = &image8[i * 256];
       fracstep = cols * 0x10000 / 256;
       frac = fracstep >> 1;

       for ( j = 0; j < 256; j++ ) {
         dest[j] = source[frac >> 16];
         frac += fracstep;
       }
     }

     qglTexImage2D ( GL_TEXTURE_2D,
                     0,
                     GL_COLOR_INDEX8_EXT,
                     256, 256,
                     0,
                     GL_COLOR_INDEX,
                     GL_UNSIGNED_BYTE,
                     image8 );
   }

   qglTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   qglTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   qglBegin ( GL_QUADS );
   qglTexCoord2f ( 1.0 / 512.0, 1.0 / 512.0 );
   qglVertex2f ( x, y );
   qglTexCoord2f ( 511.0 / 512.0, 1.0 / 512.0 );
   qglVertex2f ( x + w, y );
   qglTexCoord2f ( 511.0 / 512.0, t );
   qglVertex2f ( x + w, y + h );
   qglTexCoord2f ( 1.0 / 512.0, t );
   qglVertex2f ( x, y + h );
   qglEnd();
 }

 /* ========================================================================= */
 q_int32_t
 Draw_GetPalette ( void )
 {
   q_int32_t i;
   q_int32_t r, g, b;
   q_uint32_t v;
   byte *pic, *pal;
   q_int32_t width, height;
   /* get the palette */
   LoadPCX ( "pics/colormap.pcx", &pic, &pal, &width, &height );

   if ( !pal ) {
     VID_Error ( ERR_FATAL, "Couldn't load pics/colormap.pcx" );
   }

   for ( i = 0; i < 256; i++ ) {
     r = pal[i * 3 + 0];
     g = pal[i * 3 + 1];
     b = pal[i * 3 + 2];
     v = ( 255 << 24 ) + ( r << 0 ) + ( g << 8 ) + ( b << 16 );
     d_8to24table[i] = LittleLong ( v );
   }

   d_8to24table[255] &= LittleLong ( 0xffffff ); /* 255 is transparent */
   Mem_free ( pic );
   Mem_free ( pal );
   return 0;
 }

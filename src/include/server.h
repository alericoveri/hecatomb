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
 * Main header file for the server
 *
 * =======================================================================
 */

 #ifndef SV_SERVER_H
 #define SV_SERVER_H

 #include "prereqs.h"
 #include "game.h"

 #define MAX_MASTERS 8
 #define LATENCY_COUNTS 16
 #define RATE_MESSAGES 10

 /* MAX_CHALLENGES is made large to prevent a denial
    of service attack that could cycle all of them
    out before legitimate users connected */
 #define MAX_CHALLENGES 1024

 #define SV_OUTPUTBUF_LENGTH (MAX_MSGLEN - 16)
 #define EDICT_NUM(n) ((edict_t *)((byte *)ge->edicts + ge->edict_size * (n)))
 #define NUM_FOR_EDICT(e) (((byte *)(e) - (byte *)ge->edicts) / ge->edict_size)

 /**
  *
  */
 typedef enum {
   ss_dead,            /* no map loaded */
   ss_loading,         /* spawning level edicts */
   ss_game,            /* actively running */
   ss_cinematic,
   ss_demo,
   ss_pic
 } server_state_t;

 /**
  *
  */
 typedef struct {
   server_state_t state;           /* precache commands are only valid during load */

   qboolean attractloop;           /* running cinematics and demos for the local system only */
   qboolean loadgame;              /* client begins should reuse existing entity */

   q_uint32_t time;                  /* always sv.framenum * 100 msec */
   q_int32_t framenum;

   char name[MAX_QPATH];           /* map name, or cinematic name */
   struct cmodel_s *models[MAX_MODELS];

   char configstrings[MAX_CONFIGSTRINGS][MAX_QPATH];
   entity_state_t baselines[MAX_EDICTS];

   /* the multicast buffer is used to send a message to a set of clients
      it is only used to marshall data until SV_Multicast is called */
   sizebuf_t multicast;
   byte multicast_buf[MAX_MSGLEN];

   /* demo server information */
   FILE *demofile;
   qboolean timedemo; /* don't time sync */
 } server_t;

/**
  *
  */
 typedef enum {
   cs_free,        /* can be reused for a new connection */
   cs_zombie,      /* client has been disconnected, but don't reuse
              connection for a couple seconds */
   cs_connected,   /* has been assigned to a client_t, but not in game yet */
   cs_spawned      /* client is fully in game */
 } client_state_t;

 /**
  *
  */
 typedef struct {
   q_int32_t areabytes;
   byte areabits[MAX_MAP_AREAS / 8];       /* portalarea visibility bits */
   player_state_t ps;
   q_int32_t num_entities;
   q_int32_t first_entity;                       /* into the circular sv_packet_entities[] */
   q_int32_t senttime;                           /* for ping calculations */
 } client_frame_t;

 /**
  *
  */
 typedef struct client_s {
   client_state_t state;

   char userinfo[MAX_INFO_STRING];     /* name, etc */

   q_int32_t lastframe;                      /* for delta compression */
   usercmd_t lastcmd;                  /* for filling in big drops */

   q_int32_t commandMsec;                    /* every seconds this is reset, if user */
   /* commands exhaust it, assume time cheating */

   q_int32_t frame_latency[LATENCY_COUNTS];
   q_int32_t ping;

   q_int32_t message_size[RATE_MESSAGES];    /* used to rate drop packets */
   q_int32_t rate;
   q_int32_t surpressCount;                  /* number of messages rate supressed */

   edict_t *edict;                     /* EDICT_NUM(clientnum+1) */
   char name[32];                      /* extracted from userinfo, high bits masked */
   q_int32_t messagelevel;                   /* for filtering printed messages */

   /* The datagram is written to by sound calls, prints,
      temp ents, etc. It can be harmlessly overflowed. */
   sizebuf_t datagram;
   byte datagram_buf[MAX_MSGLEN];

   client_frame_t frames[UPDATE_BACKUP];     /* updates can be delta'd from here */

   byte *download;                     /* file being downloaded */
   q_int32_t downloadsize;                   /* total bytes (can't use EOF because of paks) */
   q_int32_t downloadcount;                  /* bytes sent */

   q_int32_t lastmessage;                    /* sv.framenum when packet was last received */
   q_int32_t lastconnect;

   q_int32_t challenge;                      /* challenge of this user, randomly generated */

   netchan_t netchan;
 } client_t;

 /**
  *
  */
 typedef struct {
   netadr_t adr;
   q_int32_t challenge;
   q_int32_t time;
 } challenge_t;

 /**
  *
  */
 typedef struct {
   qboolean initialized;               /* sv_init has completed */
   q_int32_t realtime;                       /* always increasing, no clamping, etc */

   char mapcmd[MAX_TOKEN_CHARS];       /* ie: *intro.cin+base */

   q_int32_t spawncount;                     /* incremented each server start */
   /* used to check late spawns */

   client_t *clients;                  /* [maxclients->value]; */
   q_int32_t num_client_entities;            /* maxclients->value*UPDATE_BACKUP*MAX_PACKET_ENTITIES */
   q_int32_t next_client_entities;           /* next client_entity to use */
   entity_state_t *client_entities;    /* [num_client_entities] */

   q_int32_t last_heartbeat;

   challenge_t challenges[MAX_CHALLENGES];    /* to prevent invalid IPs from connecting */

   /* serverrecord values */
   FILE *demofile;
   sizebuf_t demo_multicast;
   byte demo_multicast_buf[MAX_MSGLEN];
 } server_static_t;

 extern netadr_t net_from;
 extern sizebuf_t net_message;

 extern netadr_t master_adr[MAX_MASTERS];    /* address of the master server */

 extern server_static_t svs;                 /* persistant server info */
 extern server_t sv;                         /* local server */

 extern cvar_t *sv_paused;
 extern cvar_t *maxclients;
 extern cvar_t *sv_noreload;                 /* don't reload level state when reentering */
 extern cvar_t *sv_airaccelerate;            /* don't reload level state when reentering */
 /* development tool */
 extern cvar_t *sv_enforcetime;

 extern client_t *sv_client;
 extern edict_t *sv_player;

 /**
  * Only called at quake2.exe startup, not for each game
  */
 void SV_Init ( void );

 /**
  * Called when each game quits,
  * before Sys_Quit or Sys_Error
  */
 void SV_Shutdown ( char *finalmsg, qboolean reconnect );

 /**
  *
  */
 void SV_Frame ( q_int32_t msec );

 /**
  * Used by SV_Shutdown to send a final message to all
  * connected clients before the server goes down. The
  * messages are sent immediately, not just stuck on the
  * outgoing message list, because the server is going
  * to totally exit after returning from this function.
  */
 void SV_FinalMessage ( char *message, qboolean reconnect );

 /**
  * Called when the player is totally leaving the server, either willingly
  * or unwillingly.  This is NOT called if the entire server is quiting
  * or crashing.
  */
 void SV_DropClient ( client_t *drop );

 /**
  *
  */
 q_int32_t SV_ModelIndex ( char *name );

 /**
  *
  */
 q_int32_t SV_SoundIndex ( char *name );

 /**
  *
  */
 q_int32_t SV_ImageIndex ( char *name );

 /**
  *
  */
 void SV_ExecuteUserCommand ( char *s );

 /**
  *
  */
 void SV_InitOperatorCommands ( void );

 /**
  * Pull specific info from a newly changed userinfo string
  * into a more C freindly form.
  */
 void SV_UserinfoChanged ( client_t *cl );

 /**
  * A brand new game has been started
  */
 void SV_InitGame ( void );

 /**
  *
  */
 void SV_Map ( qboolean attractloop, char *levelstring, qboolean loadgame );


 /**
  * Send a message to the master every few minutes to
  * let it know we are alive, and log information
  */
 void Master_Heartbeat ( void );

 /**
  * This has to be done before the world logic, because
  * player processing happens outside RunWorldFrame
  */
 void SV_PrepWorldFrame ( void );

 /**
  *
  */
 typedef enum {
    RD_NONE,
    RD_CLIENT,
    RD_PACKET
  } redirect_t;

 extern char sv_outputbuf[SV_OUTPUTBUF_LENGTH];

 /**
  *
  */
 void SV_FlushRedirect ( q_int32_t sv_redirected, char *outputbuf );

 /**
  *
  */
 void SV_DemoCompleted ( void );

 /**
  *
  */
 void SV_SendClientMessages ( void );

 /**
  * Sends the contents of sv.multicast to a subset of the clients,
  * then clears sv.multicast.
  *
  * MULTICAST_ALL  same as broadcast (origin can be NULL)
  * MULTICAST_PVS  send to clients potentially visible from org
  * MULTICAST_PHS  send to clients potentially hearable from org
  */
 void SV_Multicast ( vec3_t origin, multicast_t to );

 /**
  * Each entity can have eight independant sound sources, like voice,
  * weapon, feet, etc.
  *
  * If cahnnel & 8, the sound will be sent to everyone, not just
  * things in the PHS.
  *
  * Channel 0 is an auto-allocate channel, the others override anything
  * already running on that entity/channel pair.
  *
  * An attenuation of 0 will play full volume everywhere in the level.
  * Larger attenuations will drop off.  (max 4 attenuation)
  *
  * Timeofs can range from 0.0 to 0.1 to cause sounds to be started
  * later in the frame than they normally would.
  *
  * If origin is NULL, the origin is determined from the entity origin
  * or the midpoint of the entity box for bmodels.
  */
 void SV_StartSound ( vec3_t origin, edict_t *entity, q_int32_t channel,
                      q_int32_t soundindex, float volume, float attenuation,
                      float timeofs );


 /**
  * Sends text across to be displayed if the level passes
  */
 void SV_ClientPrintf ( client_t *cl, q_int32_t level, char *fmt, ... );

 /**
  * Sends text to all active clients
  */
 void SV_BroadcastPrintf ( q_int32_t level, char *fmt, ... );

 /**
  * Sends text to all active clients
  */
 void SV_BroadcastCommand ( char *fmt, ... );

 /**
  *
  */
 void SV_Nextserver ( void );

 /**
  * The current net_message is parsed for the given client
  */
 void SV_ExecuteClientMessage ( client_t *cl );

 /**
  *
  */
 void SV_ReadLevelFile ( void );

 /**
  *
  */
 void SV_Status_f ( void );

 /**
  *
  */
 void SV_WriteFrameToClient ( client_t *client, sizebuf_t *msg );

 /**
  * Save everything in the world out without deltas.
  * Used for recording footage for merged or assembled demos
  */
 void SV_RecordDemoMessage ( void );

 /**
  * Decides which entities are going to be visible to the client, and
  * copies off the playerstat and areabits.
  */
 void SV_BuildClientFrame ( client_t *client );

 //
 extern game_export_t *ge;

 /**
  * Init the game subsystem for a new map
  */
 void SV_InitGameProgs ( void );

 /**
  * Called when either the entire server is being killed, or
  * it is changing to a different game directory.
  */
 void SV_ShutdownGameProgs ( void );


 /* server side savegame stuff */
 /*----------------------------*/

 /**
  * Delete save/<XXX>/
  */
 void SV_WipeSavegame ( char *savename );

 /**
  *
  */
 void SV_CopySaveGame ( char *src, char *dst );

 /**
  *
  */
 void SV_WriteLevelFile ( void );

 /**
  *
  */
 void SV_WriteServerFile ( qboolean autosave );

 /**
  *
  */
 void SV_Loadgame_f ( void );

 /**
  *
  */
 void SV_Savegame_f ( void );

 /**
  * High level object sorting to reduce interaction tests
  */
 void SV_ClearWorld ( void );

 /**
  * Called after the world model has been loaded, before linking any entities
  */
 void SV_UnlinkEdict ( edict_t *ent );

 /**
  * Call before removing an entity, and before trying to move one,
  * so it doesn't clip against itself
  */
 void SV_LinkEdict ( edict_t *ent );

 /**
  * Needs to be called any time an entity changes origin, mins, maxs,
  * or solid. Automatically unlinks if needed. sets ent->v.absmin and
  * ent->v.absmax sets ent->leafnums[] for pvs determination even if
  * the entity is not solid
  */
 q_int32_t SV_AreaEdicts ( vec3_t mins, vec3_t maxs, edict_t **list,
                     q_int32_t maxcount, q_int32_t areatype );

 /**
  *
  */
 q_int32_t SV_PointContents ( vec3_t p );

 /**
  * Moves the given mins/maxs volume through the world from start to end.
  * Passedict and edicts owned by passedict are explicitly not checked.
  */
 trace_t SV_Trace ( vec3_t start, vec3_t mins, vec3_t maxs,
                    vec3_t end, edict_t *passedict, q_int32_t contentmask );

 #endif /* SV_SERVER_H */


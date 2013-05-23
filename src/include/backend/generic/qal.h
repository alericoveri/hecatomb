/*
 * Copyright (C) 2012 Yamagi Burmeister
 * Copyright (C) 2010 skuller.net
 * Copyright (C) 2005 Stuart Dalton (badcdev@gmail.com)
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
 * Header file to the low level "qal" API implementation. This source file
 * was almost entirely rewritten to include direct OpenAL function calls
 * instead of pointers, qal* functions are replaced by their analogous OAL
 * function. This code is based upon YQ2 and ioquake3 QAL implementation.
 *
 * =======================================================================
 */

 #ifndef QAL_H
 #define QAL_H

 #include "prereqs.h"

 /* OpenAL headers */
 #ifdef HT_OS_OSX
 # include <OpenAL/al.h>
 # include <OpenAL/alc.h>
 #else
 # define AL_ALEXT_PROTOTYPES
 # include <AL/al.h>
 # include <AL/alc.h>
 # include <AL/alext.h>
 # include <AL/efx.h>
 #endif

 #define qalEnable alEnable
 #define qalDisable alDisable
 #define qalIsEnabled alIsEnabled
 #define qalGetString alGetString
 #define qalGetBooleanv alGetBooleanv
 #define qalGetIntegerv alGetIntegerv
 #define qalGetFloatv alGetFloatv
 #define qalGetDoublev alGetDoublev
 #define qalGetBoolean alGetBoolean
 #define qalGetInteger alGetInteger
 #define qalGetFloat alGetFloat
 #define qalGetDouble alGetDouble
 #define qalGetError alGetError
 #define qalIsExtensionPresent alIsExtensionPresent
 #define qalGetProcAddress alGetProcAddress
 #define qalGetEnumValue alGetEnumValue
 #define qalListenerf alListenerf
 #define qalListener3f alListener3f
 #define qalListenerfv alListenerfv
 #define qalListeneri alListeneri
 #define qalListener3i alListener3i
 #define qalListeneriv alListeneriv
 #define qalGetListenerf alGetListenerf
 #define qalGetListener3f alGetListener3f
 #define qalGetListenerfv alGetListenerfv
 #define qalGetListeneri alGetListeneri
 #define qalGetListener3i alGetListener3i
 #define qalGetListeneriv alGetListeneriv
 #define qalGenSources alGenSources
 #define qalDeleteSources alDeleteSources
 #define qalIsSource alIsSource
 #define qalSourcef alSourcef
 #define qalSource3f alSource3f
 #define qalSourcefv alSourcefv
 #define qalSourcei alSourcei
 #define qalSource3i alSource3i
 #define qalSourceiv alSourceiv
 #define qalGetSourcef alGetSourcef
 #define qalGetSource3f alGetSource3f
 #define qalGetSourcefv alGetSourcefv
 #define qalGetSourcei alGetSourcei
 #define qalGetSource3i alGetSource3i
 #define qalGetSourceiv alGetSourceiv
 #define qalSourcePlayv alSourcePlayv
 #define qalSourceStopv alSourceStopv
 #define qalSourceRewindv alSourceRewindv
 #define qalSourcePausev alSourcePausev
 #define qalSourcePlay alSourcePlay
 #define qalSourceStop alSourceStop
 #define qalSourceRewind alSourceRewind
 #define qalSourcePause alSourcePause
 #define qalSourceQueueBuffers alSourceQueueBuffers
 #define qalSourceUnqueueBuffers alSourceUnqueueBuffers
 #define qalGenBuffers alGenBuffers
 #define qalDeleteBuffers alDeleteBuffers
 #define qalIsBuffer alIsBuffer
 #define qalBufferData alBufferData
 #define qalBufferf alBufferf
 #define qalBuffer3f alBuffer3f
 #define qalBufferfv alBufferfv
 #define qalBufferi alBufferi
 #define qalBuffer3i alBuffer3i
 #define qalBufferiv alBufferiv
 #define qalGetBufferf alGetBufferf
 #define qalGetBuffer3f alGetBuffer3f
 #define qalGetBufferfv alGetBufferfv
 #define qalGetBufferi alGetBufferi
 #define qalGetBuffer3i alGetBuffer3i
 #define qalGetBufferiv alGetBufferiv
 #define qalDopplerFactor alDopplerFactor
 #define qalDopplerVelocity alDopplerVelocity
 #define qalSpeedOfSound alSpeedOfSound
 #define qalDistanceModel alDistanceModel
 #define qalcCreateContext alcCreateContext
 #define qalcMakeContextCurrent alcMakeContextCurrent
 #define qalcProcessContext alcProcessContext
 #define qalcSuspendContext alcSuspendContext
 #define qalcDestroyContext alcDestroyContext
 #define qalcGetCurrentContext alcGetCurrentContext
 #define qalcGetContextsDevice alcGetContextsDevice
 #define qalcOpenDevice alcOpenDevice
 #define qalcCloseDevice alcCloseDevice
 #define qalcGetError alcGetError
 #define qalcIsExtensionPresent alcIsExtensionPresent
 #define qalcGetProcAddress alcGetProcAddress
 #define qalcGetEnumValue alcGetEnumValue
 #define qalcGetString alcGetString
 #define qalcGetIntegerv alcGetIntegerv
 #define qalcCaptureOpenDevice alcCaptureOpenDevice
 #define qalcCaptureCloseDevice alcCaptureCloseDevice
 #define qalcCaptureStart alcCaptureStart
 #define qalcCaptureStop alcCaptureStop
 #define qalcCaptureSamples alcCaptureSamples
 #define qalGenFilters alGenFilters
 #define qalFilteri alFilteri
 #define qalFilterf alFilterf
 #define qalDeleteFilters alDeleteFilters

 /**
  * Gives information over the OpenAL
  * implementation and it's state
  */
 void QAL_SoundInfo ( void );

 /**
  * Loads the OpenAL shared lib, creates
  * a context and device handle.
  */
 qboolean QAL_Init ( void );

 /**
  * Shuts OpenAL down, frees all context and
  * device handles and unloads the shared lib.
  */
 void QAL_Shutdown ( void );

 #endif /* QAL_H */

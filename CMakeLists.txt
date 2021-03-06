#
# Hecatomb
# A portable Quake II client
# CMake build automation scripts
# Author: Alejandro Ricoveri <alejandroricoveri@gmail.com>
#
# ==================================================
# !!! DO NOT ALTER ANYTHING BELOW THIS LINE !!!
# (unless you know what you're doing)
# ==================================================
#

# Minimum CMake version
cmake_minimum_required(VERSION 2.8)

# define a macro that helps defining an option
# ---------------------
macro(ht_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# set a default build type if none was provided
# this has to be done before the project() instruction!
# ---------------------
ht_set_option(CMAKE_BUILD_TYPE Debug STRING
    "Choose the type of build (Debug or Release)")


# set project name
# ---------------------
set (PROJ_NAME "hecatomb")
project (${PROJ_NAME})

# Helpful modules are here
# ---------------------
set (CMAKE_GOODIES_PATH ${CMAKE_SOURCE_DIR}/cmake)
set (CMAKE_MODULE_PATH "${CMAKE_GOODIES_PATH}/modules" ${CMAKE_MODULE_PATH})

# detect OS, compiler, architecture
# ---------------------
include (${CMAKE_GOODIES_PATH}/Config.cmake)

# Set debug/release definitions
# ---------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set (HT_BUILD_DEBUG 1)
  set (HT_CC_CFLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
else()
  set (HT_BUILD_RELEASE 1)
  add_definitions (-DNDEBUG)
  set (HT_CC_CFLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
endif()

# Essential info
# ---------------------
set (PROJ_DEVNAME "htq2")
set (PROJ_URL "http://alericoveri.github.io/hecatomb")
set (PROJ_GIT_URL "git://github.com/alericoveri/hecatomb.git")
set (PROJ_PRODUCT_NAME "Hecatomb Quake II")
set (PROJ_EMAIL "alejandroricoveri@gmail.com")
set (VERSION_MAJOR 0)
set (VERSION_MINOR 3)
set (VERSION_PATCH 0)
set (VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Default values
# ---------------------
set (DEF_MEM "stdc")
set (DEF_CLIENT FALSE)
set (DEF_SERVER FALSE)
set (DEF_GAME FALSE)
set (DEF_OGG FALSE)
set (DEF_OPENAL FALSE)
set (DEF_ZIP FALSE)
set (DEF_RETEXTURE FALSE)
if (WINDOWS OR OSX)
  set (DEF_CLIENT TRUE)
  set (DEF_GAME TRUE)
  set (DEF_OGG TRUE)
  set (DEF_ZIP TRUE)
  set (DEF_RETEXTURE TRUE)
endif()
if (WINDOWS)
  set (DEF_OPENAL TRUE)
endif()

#
# Architecture settings
# ---------------------
if (ARCH STREQUAL "x86")
    set (HT_ARCH_x86 1)
elseif (ARCH STREQUAL "x86_64")
    set (HT_ARCH_x86_64 1)
elseif (ARCH STREQUAL "armv7")
    set (HT_ARCH_arm 1)
    set (HT_ARCH_armv7 1)
elseif (ARCH STREQUAL "armv6")
    set (HT_ARCH_arm 1)
    set (HT_ARCH_armv6 1)
elseif (ARCH STREQUAL "armv5")
    set (HT_ARCH_arm 1)
    set (HT_ARCH_armv5 1)
elseif (ARCH STREQUAL "arm")
    set (HT_ARCH_arm 1)
elseif (ARCH STREQUAL "ia64")
    set (HT_ARCH_ia64 1)
elseif (ARCH STREQUAL "ppc64")
    set (HT_ARCH_ppc64 1)
elseif (ARCH STREQUAL "ppc")
    set (HT_ARCH_ppc 1)
endif()

# Check basic C headers
# ---------------------
include (CheckIncludeFiles)
check_include_files (assert.h HT_HAVE_ASSERT_H)
check_include_files (math.h HT_HAVE_MATH_H)
check_include_files (string.h HT_HAVE_STRING_H)
check_include_files (stdarg.h HT_HAVE_STDARG_H)
check_include_files (stdio.h HT_HAVE_STDIO_H)
check_include_files (stdlib.h HT_HAVE_STDLIB_H)
check_include_files (ctype.h HT_HAVE_CTYPE_H)
check_include_files (setjmp.h HT_HAVE_SETJMP_H)
check_include_files (signal.h HT_HAVE_SIGNAL_H)
check_include_files (time.h HT_HAVE_TIME_H)
check_include_files (locale.h HT_HAVE_LOCALE_H)
check_include_files (errno.h HT_HAVE_ERRNO_H)
check_include_files (stdint.h HT_HAVE_STDINT_H)
check_include_files (malloc.h HT_HAVE_MALLOC_H)

# Check functions
# ---------------------
include (CheckFunctionExists)
if (HT_MEM_STDC)
  check_function_exists (mallinfo HT_HAVE_MALLINFO)
else()
  set (HT_HAVE_MALLINFO 0)
endif()

# Check machine endianness (Byte order)
# ---------------------
include (TestBigEndian)
test_big_endian (BIG_ENDIAN)
if (BIG_ENDIAN)
  set (HT_ENDIAN_BIG 1)
else()
  set (HT_ENDIAN_LITTLE 1)
endif()

# let CMake know about our additional libraries paths (on Windows and OS X)
# Thanks to SFML guys for this snippet
# ---------------------
set (EXTLIBS_BASE "${PROJECT_SOURCE_DIR}/extlibs")
set (EXTLIBS_HEADERS "${EXTLIBS_BASE}/include")
set (EXTLIBS_LIBS "${EXTLIBS_BASE}/libs")
set (EXTLIBS_BIN "${EXTLIBS_BASE}/bin")
if (WINDOWS)
  set (CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${EXTLIBS_HEADERS}")
  set (SDL_INCLUDE_DIR ${EXTLIBS_HEADERS})
  set (SDL2_INCLUDE_DIR ${EXTLIBS_HEADERS})
  set (OPENAL_INCLUDE_DIR ${EXTLIBS_HEADERS})
  if (COMPILER_GCC)
    set (EXTLIBS_LIBS_MINGW "${EXTLIBS_LIBS}/mingw")
    if (HT_ARCH_x86)
      set (CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${EXTLIBS_LIBS_MINGW}/x86")
      set (CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${EXTLIBS_BIN}/x86")
    elseif(HT_ARCH_x86_64)
      set (CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${EXTLIBS_LIBS_MINGW}/x64")
      set (CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${EXTLIBS_BIN}/x64")
    endif()
  endif()
elseif (OSX)
  # The source tree comes with all dependencies that are
  # necessary ready to build all targets
  # default to searching for frameworks last
  # so we can give other libraries/frameworks the chance to get used
  set (CMAKE_FIND_FRAMEWORK LAST)
  # set up the default search directories for frameworks
  set (CMAKE_SYSTEM_FRAMEWORK_PATH "${EXTLIBS_LIBS}/osx" ${CMAKE_SYSTEM_FRAMEWORK_PATH})
endif()

# Set the memory allocator backend
# It switches between various implementations of malloc/realloc/calloc/free
# Use it at your own risk as it is highly experimental, current optional
# memory allocators are the following:
# stdc     - Use the Standard C Library implementation (this is the default)
# dlmalloc - http://g.oswego.edu/dl/html/malloc.html
# jemalloc - http://www.canonware.com/jemalloc
# ---------------------
ht_set_option (HT_WITH_MALLOC ${DEF_MEM} STRING "Memory allocator backend [EXPERIMENTAL]")
if (NOT WINDOWS)
  set_property (CACHE HT_WITH_MALLOC PROPERTY STRINGS "stdc;dlmalloc;jemalloc")
else ()
  set_property (CACHE HT_WITH_MALLOC PROPERTY STRINGS "stdc;dlmalloc")
endif()
mark_as_advanced (HT_WITH_MALLOC)

# Enable the 'client' target
# ---------------------
# This will be probably what you'll want to build the most
ht_set_option (HT_CLIENT ${DEF_CLIENT} BOOL "Build client executable")

# Enable the 'server' target
# ---------------------
# This will build the dedicated server
ht_set_option (HT_SERVER ${DEF_SERVER} BOOL "Build dedicated server executable")

# Enable the 'game' target
# ---------------------
# This will build the baseq2 DLL
ht_set_option (HT_GAME ${DEF_GAME} BOOL "Build baseq2 game DLL")

# Enables Generic Hunk Driver, a general purpose Hunk_* backend
# based solely on plain malloc/free. Use this at your own risk as
# it is an experimental feature.
# You're encouraged to use this allocator for having an easier way
# to porting htq2 to other platforms, otherwise you should remain
# using the platform-specific drivers, they're much faster and therefore
# reliable.
# ---------------------
ht_set_option(HT_HUNKDRIVER_GENERIC FALSE BOOL
  "Generic Hunk driver (based on malloc/free) [EXPERIMENTAL]")
mark_as_advanced (HT_HUNKDRIVER_GENERIC)

if (HT_CLIENT)

  # SDL 2.0
  # Enables the use of SDL2 instead of SDL1
  # ---------------------
  ht_set_option(HT_USE_SDL2 FALSE BOOL "Use libSDL-2.0 [EXPERIMENTAL]")

  # Enables OGG/Vorbis support. OGG/Vorbis files can be
  # used as a substitute of CD audio playback. Adds
  # dependencies to libogg, libvorbis and libvorbisfile.
  # ---------------------
  ht_set_option(HT_WITH_OGG ${DEF_OGG} BOOL "OGG/Vorbis support")

  # Enables high resolution retexturing support. Adds a dependency to libjpeg
  # ---------------------
  ht_set_option(HT_WITH_RETEXTURE ${DEF_RETEXTURE} BOOL "Retexturing support")

  # Enables the optional OpenAL sound system.
  # To use it your system needs libopenal.so.1
  # or openal32.dll (we recommend openal-soft)
  # installed
  # ---------------------
  ht_set_option(HT_WITH_OPENAL ${DEF_OPENAL} BOOL
    "Use the OpenAL sound system")

  # Set the gamma via X11 and not via SDL. This works
  # around problems in some SDL versions. Adds dependencies
  # to libX11 and libXxf86vm. Unsupported on
  # Windows and OS X.
  # ---------------------
  if (UNIX AND NOT OSX)
    ht_set_option(HT_WITH_X11GAMMA FALSE BOOL
      "Set the gamma via X11 and not via SDL")
  endif()
endif() # HT_CLIENT

if (HT_CLIENT OR HT_SERVER)

  # Enable systemwide installation of game assets
  # ---------------------
  ht_set_option(HT_WITH_SYSTEMWIDE TRUE BOOL
    "Enable systemwide installation of game assets")

  # Enables opening of ZIP files (also known as .pk3 packs).
    # Adds a dependency to zlib
  # ---------------------
  ht_set_option(HT_WITH_ZIP ${DEF_ZIP} BOOL
    "ZIP files support (for opening .pk3 packs)")

  # This will set the default system directory, a non-empty string
  # would actually be used. On Windows normals slashes (/)
  # instead of backslashed (\) must be used!
  # ---------------------
  if (WINDOWS)
    set (SYSTEMDIR_DEFAULT c:/quake2)
  else()
    set (SYSTEMDIR_DEFAULT /usr/share/games/quake2)
  endif()
  ht_set_option(HT_WITH_SYSTEMDIR "${SYSTEMDIR_DEFAULT}"
      PATH "Location of system-wide game assets installation")
endif()

# Compiler related definitions
# ---------------------
if (COMPILER_GCC)
    set (HT_CC_GCC 1)
elseif (COMPILER_CLANG)
    set (HT_CC_CLANG 1)
else()
  set (HT_CC_UNK 1)
endif()

#
# Let's see about that memory allocator
# ---------------------
if (NOT HT_WITH_MALLOC STREQUAL "stdc")
  if (HT_WITH_MALLOC STREQUAL "jemalloc")
    set (HT_MEM_JEMALLOC 1)
    find_package (Jemalloc REQUIRED)
  elseif (HT_WITH_MALLOC STREQUAL "dlmalloc")
    set (HT_MEM_DLMALLOC 1)
  else ()
    message (WARNING "Memory allocator under name \"${HT_WITH_MALLOC}\" not supported!. Falling back to \"${DEF_MEM}\" ...")
    set (HT_WITH_MALLOC ${DEF_MEM})
  endif()
endif()
# Fallback to stdc
if (HT_WITH_MALLOC STREQUAL ${DEF_MEM})
  set (HT_MEM_STDC 1)
endif()

if (UNIX)
  if (HT_CLIENT OR HT_SERVER)
    # Optional execinfo support for backtracing on crashes
    # ---------------------
    find_package (Libexecinfo)
    if(LIBEXECINFO_LIB_FOUND)
      set(HT_HAVE_EXECINFO 1)
    endif()

    # dlopen, dlsym, dlclose, etc ...
    # ---------------------
    find_package (LibDL REQUIRED)
  endif()
endif()

# Git related information
# ---------------------
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
if (NOT ${GIT_SHA1} STREQUAL "GITDIR-NOTFOUND")
  # This is not a fresh copy (from a git repo)
  set (HT_WITH_GIT 1)
endif()

if (HT_CLIENT)

  # X11 libraries
  # ---------------------
  if (HT_WITH_X11GAMMA)
    find_package (X11 REQUIRED)
  endif()

  # OpenGL
  # ---------------------
  find_package(OpenGL REQUIRED)

  # SDL
  # ---------------------
  if (HT_USE_SDL2)
    find_package (SDL2 REQUIRED)
  else()
    find_package (SDL REQUIRED)
  endif()

  # Check for a valid version of SDL before insertion
  # ---------------------
  if (SDL_VERSION_STRING)
    if (NOT SDL_VERSION_STRING MATCHES "1.2.*")
      set (SDL_VER_ERR 1)
    endif()
    if ( NOT SDL_VERSION_STRING MATCHES "2.*" )
      set (SDL2_VER_ERR 1)
    endif()
    if (SDL_VER_ERR AND SDL2_VER_ERR)
      message (FATAL_ERROR "Only SDL 1.2 or 2.0 are supported. Your SDL version is: ${SDL_VERSION_STRING}")
    endif()
  elseif(NOT WINDOWS)
    message (WARNING "SDL version couldn't be determined!. I'll use it anyway ...")
  endif()

  # OpenAL
  # ---------------------
  if (HT_WITH_OPENAL)
    find_package(OpenAL REQUIRED)
  endif()

  # OGG and Vorbis
  # ---------------------
  if (HT_WITH_OGG)
    find_package (VORBIS REQUIRED)
  endif()

  # libjpeg
  # ---------------------
  if (HT_WITH_RETEXTURE)
    if (OSX)
      set (JPEG_NAMES libjpeg)
    endif()
    find_package (JPEG REQUIRED)
  endif()

endif() # HT_CLIENT

if (HT_CLIENT OR HT_SERVER)

    # zlib
    # ---------------------
    if (HT_WITH_ZIP)
      find_package (ZLIB REQUIRED)
    endif()

endif()

# Source files
# ---------------------
add_subdirectory (src)

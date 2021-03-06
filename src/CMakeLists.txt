#
# Hecatomb
# A portable Quake II client
# http://github.com/alericoveri/hecatomb
#
# CMake build automation scripts
# Author: Alejandro Ricoveri <alejandroricoveri@gmail.com>
#
# Source files
# --------------------------------
#

#
# Define sources and executable
# ---------------------

set (CLIENT_EXEC "htq2")
set (SERVER_EXEC "htq2d")

#
# Definitions
# ---------------------
#

# Windows
if (WINDOWS)
  set (HT_OS_WINDOWS 1)
endif()

# UNIX
if (UNIX)

  set (HT_OS_UNIX 1)

  # BSD variants
  if (BSD)
    set (HT_OS_BSD 1)
    if (OPENBSD)
      set (HT_OS_OPENBSD 1)
    endif()
    if (FREEBSD)
      set (HT_OS_FREEBSD 1)
    endif()
  endif ()

  # Linux
  if (LINUX)
    set (HT_OS_LINUX 1)
  endif()

  # Apple OSX
  if (OSX)
    set (HT_OS_OSX 1)
  endif()

endif()

# An unknown operating system
if (UNK_OS)
  set (HT_OS_UNKNOWN 1)
endif()

# Source files root directory
# ---------------------
set (SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Include directories
# ---------------------
set (INCLUDE_DIRS ${SRC_DIR}/include)

# Standard libraries
# ---------------------
if (HT_CLIENT OR HT_SERVER)

  # Libraries
  set (LIBS m)

endif()

# Jemalloc as the memory allocator
if (HT_MEM_JEMALLOC)
  list (APPEND INCLUDE_DIRS ${JEMALLOC_INCLUDE_DIRS})
  list (APPEND LIBS ${JEMALLOC_LIBRARIES})
endif()

#
# Client-specific
# ---------------------
#
if (HT_CLIENT)

  # OpenGL
  list (APPEND INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
  set (CLIENT_LIBS ${OPENGL_LIBRARIES})

  # SDL
  if (HT_USE_SDL2)
    set (HT_WITH_SDL2 1)
    list (APPEND INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    list (APPEND CLIENT_LIBS ${SDL2_LIBRARY})
  else()
    list (APPEND INCLUDE_DIRS ${SDL_INCLUDE_DIR})
    list (APPEND CLIENT_LIBS ${SDL_LIBRARY})
  endif()

  # X11 hardware gamma
  if (HT_WITH_X11GAMMA)
    list (APPEND INCLUDE_DIRS ${X11_xf86vmode_INCLUDE_PATH})
    list (APPEND CLIENT_LIBS ${X11_Xxf86vm_LIB})
  endif()

  # Retexturing support
  if (HT_WITH_RETEXTURE)
    list (APPEND INCLUDE_DIRS ${JPEG_INCLUDE_DIR}) # libjpeg
    list (APPEND CLIENT_LIBS ${JPEG_LIBRARY}) # jpeg
  endif()

  # OpenAL
  if (HT_WITH_OPENAL)
    list (APPEND INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
    list (APPEND CLIENT_LIBS ${OPENAL_LIBRARY})
  endif()

  # OGG Vorbis
  if (HT_WITH_OGG)
    list (APPEND INCLUDE_DIRS ${OGG_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR})
    list (APPEND CLIENT_LIBS
      ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBIS_FILE_LIBRARY})
  endif()

  # Windows specific libraries
  if (WINDOWS)
    # WinMM
    list (APPEND CLIENT_LIBS -mwindows)
  endif()

endif() # HT_CLIENT

#
# These are common between client and server
# ---------------------
if (HT_CLIENT OR HT_SERVER)

  # Windows specific libraries
  if (WINDOWS)
    # Winsock
    list (APPEND LIBS ws2_32 winmm)
  endif()

  # .pk3 files support
  if (HT_WITH_ZIP)
    list (APPEND INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS})  # zlib
    list (APPEND LIBS ${ZLIB_LIBRARIES})
  endif()

  # Unix specific libraries
  if (UNIX)

    # dl
    list (APPEND INCLUDE_DIRS ${LIBDL_INCLUDE_DIR})
    list (APPEND LIBS ${LIBDL_LIBRARY}) # libdl

    # execinfo
    if (HT_HAVE_EXECINFO)
      list (APPEND INCLUDE_DIRS ${LIBEXECINFO_INCLUDE_DIR})
      list (APPEND LIBS ${LIBEXECINFO_LIBRARIES}) # libexecinfo
    endif()
  endif()

endif()

# Include path
# ---------------------
include_directories (${INCLUDE_DIRS})

# Add headers to the project
# ---------------------
add_subdirectory (include)

# Set output directory
# ---------------------
set (OUTPUT_DIR "${PROJECT_SOURCE_DIR}/bin/${ARCH}")

#
# Source code used by all targets
# ---------------------
add_subdirectory (common)

if (HT_CLIENT OR HT_SERVER)

  # Backends (platform specific)
  add_subdirectory (backends)

  # Server
  add_subdirectory (server)

endif()

if (HT_CLIENT)

  # Refresher
  add_subdirectory (refresh)

  if (WINDOWS)
    # On Windows it becomes necessary to define GLEW_STATIC
    set_source_files_properties (${GLEW_SRC} PROPERTIES COMPILE_DEFINITIONS GLEW_STATIC)
  endif()

  # Client
  add_subdirectory (client)

endif()

if (HT_GAME)

  # The game itself
  add_subdirectory (game)

endif()

if (HT_SERVER)

  # Server exec
  add_executable (server ${SERVER_EXEC_SRC})

endif()

# Configuration file
# ---------------------
configure_file (${SRC_DIR}/include/config.h.in ${SRC_DIR}/include/config.h)

if (HT_SERVER)

  # -DDEDICATED_ONLY for dedicated server
  # ---------------------
  set_target_properties(server PROPERTIES COMPILE_DEFINITIONS "DEDICATED_ONLY")
  set_target_properties(server PROPERTIES OUTPUT_NAME "${SERVER_EXEC}")
  target_link_libraries (server ${LIBS})

  # Set output directory
  # ---------------------
  set_target_properties (server
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

  # Install
  # ---------------------
  #
  if ( NOT WINDOWS )
    install (TARGETS server RUNTIME DESTINATION bin)
  endif()

endif()

if (HT_CLIENT)

  # Client exec
  # ---------------------
  add_executable (client ${CLIENT_EXEC_SRC} ${HEADERS})
  set_target_properties(client PROPERTIES OUTPUT_NAME "${CLIENT_EXEC}")
  target_link_libraries(client ${LIBS} ${CLIENT_LIBS})

  # Set output directory
  # ---------------------
  set_target_properties (client
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

  # Install
  # ---------------------
  #
  if ( NOT WINDOWS )
    install (TARGETS client RUNTIME DESTINATION bin)
  endif()

endif()

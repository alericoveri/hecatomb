#
# Hecatomb
# A portable Quake II client
# http://github.com/alericoveri/hecatomb
#
# CMake build automation scripts
# Author: Alejandro Ricoveri <alejandroricoveri@gmail.com>
#
# Game DLL
# --------------------------------
#

# Library name
set (GAME_DLL "game${ARCH}")

# Game sources
set (
  GAME_SRC

  g_ai.c
  g_chase.c
  g_cmds.c
  g_combat.c
  g_func.c
  g_items.c
  g_main.c
  g_misc.c
  g_monster.c
  g_phys.c
  g_spawn.c
  g_svcmds.c
  g_target.c
  g_trigger.c
  g_turret.c
  g_utils.c
  g_weapon.c
  monster/berserker/berserker.c
  monster/boss2/boss2.c
  monster/boss3/boss3.c
  monster/boss3/boss31.c
  monster/boss3/boss32.c
  monster/brain/brain.c
  monster/chick/chick.c
  monster/flipper/flipper.c
  monster/float/float.c
  monster/flyer/flyer.c
  monster/gladiator/gladiator.c
  monster/gunner/gunner.c
  monster/hover/hover.c
  monster/infantry/infantry.c
  monster/insane/insane.c
  monster/medic/medic.c
  monster/misc/move.c
  monster/mutant/mutant.c
  monster/parasite/parasite.c
  monster/soldier/soldier.c
  monster/supertank/supertank.c
  monster/tank/tank.c
  player/client.c
  player/hud.c
  player/trail.c
  player/view.c
  player/weapon.c
  savegame/savegame.c
)

# All sources needed by this target
set (GAME_DLL_SRC ${COMMON_SHARED_SRC} ${GAME_SRC})

# Game DLL target
add_library (game SHARED ${GAME_DLL_SRC})
target_link_libraries(game ${LIBS} ${GAME_LIBS})

# Remove "lib" prefix so it won't be like "libgame.so"
set_target_properties(game PROPERTIES PREFIX "")
set_target_properties(game PROPERTIES OUTPUT_NAME "${GAME_DLL}")

# Set output directory
if (WINDOWS)
  set_target_properties (game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/baseq2")
else()
  set_target_properties (game PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/baseq2")
endif()

# Export the dll name + suffix for use in the config
set (GAME_DLL "${GAME_DLL}${CMAKE_SHARED_LIBRARY_SUFFIX}" PARENT_SCOPE)

# Game DLL installation would be only relevant
# for system-wide installations on Unix systems
if (UNIX AND HT_WITH_SYSTEMWIDE)
  install (TARGETS game LIBRARY DESTINATION "${HT_WITH_SYSTEMDIR}/baseq2")
endif()

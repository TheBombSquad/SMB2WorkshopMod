cmake_minimum_required(VERSION 3.15)
project(SMB2WorkshopMod)

set(CMAKE_CXX_STANDARD 20)

# Specify 32-bit processor for better linting
# -Wno-write-strings because some GC SDK functions take non-const char *,
# and Ghidra can't represent const char * anyhow
set(CMAKE_C_FLAGS "-m32 -Wno-write-strings")
set(CMAKE_CXX_FLAGS "-m32 -Wno-write-strings")

include_directories(rel/include)

add_executable(SMB2WorkshopMod
        rel/assembly/main_loop_assembly.s
        rel/runtime/restfpr_x.s
        rel/runtime/restgpr_x.s
        rel/cxx.cpp
        rel/heap.cpp
        rel/main.cpp
        rel/pad.cpp
        rel/patch.cpp
        rel/rel.cpp
        rel/draw.cpp
        rel/relpatches.cpp
        rel/assembly/story_mode_music_fix.s
        rel/scratch.cpp
        rel/assembly.cpp
        rel/config.cpp
        rel/relutil.cpp rel/modlink.cpp
        rel/version.cpp)

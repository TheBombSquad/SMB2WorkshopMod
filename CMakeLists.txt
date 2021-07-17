cmake_minimum_required(VERSION 3.15)
project(ApeSphere)

set(CMAKE_CXX_STANDARD 20)

# Specify 32-bit processor for better linting
set(CMAKE_C_FLAGS -m32 -Wno-write-strings)
set(CMAKE_CXX_FLAGS -m32 -Wno-write-strings)

include_directories(rel/include)
include_directories(rel/include/gc)
include_directories(rel/include/mkb)

add_executable(ApeSphere
        rel/assembly/main_loop_assembly.s
        rel/runtime/restfpr_x.s
        rel/runtime/restgpr_x.s
        rel/cxx.cpp
        rel/heap.cpp
        rel/main.cpp
        rel/pad.cpp
        rel/patch.cpp
        rel/rel.cpp
        rel/tetris.cpp
        rel/memstore.cpp
        rel/savestate.cpp
        rel/draw.cpp
        rel/jump.cpp
        rel/menu_defn.cpp
        rel/scratch.cpp
        rel/assembly.cpp
        rel/timer.cpp
        rel/include/timer.h
        rel/include/iw.h
        rel/iw.cpp
        rel/include/menu_impl.h
        rel/menu_impl.cpp
        rel/relpatches.cpp
        rel/assembly/story_mode_music_fix.s
)

cmake_minimum_required(VERSION 3.17)
project(CrossCraft-Classic)

set(CMAKE_CXX_STANDARD 17)
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=leak")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=leak")


if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

add_subdirectory(ext/Stardust-Celeste Stardust)

add_executable(CrossCraft-Classic src/Main.cpp src/Gamestate.cpp src/Particles.cpp src/Player.cpp src/World.cpp src/ChunkStack.cpp src/ChunkMesh.cpp src/Clouds.hpp src/Clouds.cpp)
target_include_directories(CrossCraft-Classic PUBLIC ext/Stardust-Celeste/ext/glm/glm/ ext/FNL/Cpp)
target_link_libraries(CrossCraft-Classic SC-Entry Stardust-Celeste)

if(WIN32)
    target_link_libraries(CrossCraft-Classic ws2_32.lib)
endif()

if(PSP)
create_pbp_file(
    TARGET CrossCraft-Classic
    TITLE CrossCraft-Classic
    ICON_PATH ../src/assets/psp/ICON0.png
    BACKGROUND_PATH ../src/assets/psp/PIC1.png
    BUILD_PRX 1
)

endif()

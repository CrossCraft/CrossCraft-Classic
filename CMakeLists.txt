cmake_minimum_required(VERSION 3.17)
project(CrossCraft-Classic)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

add_subdirectory(ext/Stardust-Celeste Stardust)
add_subdirectory(ext/zlib ZLib)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)

add_executable(CrossCraft-Classic ${SRC_FILES} ${INC_FILES})
target_include_directories(CrossCraft-Classic PUBLIC ext/Stardust-Celeste/ext/glm/glm/ ext/FNL/Cpp ext/zlib/)
target_link_libraries(CrossCraft-Classic SC-Entry Stardust-Celeste zlib)

if(WIN32)
    target_link_libraries(CrossCraft-Classic ws2_32.lib)
endif()

# target_compile_options(CrossCraft-Classic PRIVATE -fsanitize=address)
# target_link_options(CrossCraft-Classic PRIVATE -fsanitize=address)
if(PSP)
    create_pbp_file(
        TARGET CrossCraft-Classic
        TITLE CrossCraft-Classic
        ICON_PATH ../src/assets/psp/ICON0.png
        BACKGROUND_PATH ../src/assets/psp/PIC1.png
        BUILD_PRX 1
    )
endif()

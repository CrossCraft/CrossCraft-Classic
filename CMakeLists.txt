cmake_minimum_required(VERSION 3.17)
project(CrossCraft-Classic)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
    add_compile_definitions(IOAPI_NO_64)
elseif(VITA)
    add_compile_definitions(BUILD_PLAT=3)
    add_compile_definitions(IOAPI_NO_64)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

add_subdirectory(ext/Stardust-Celeste Stardust)

if(NOT VITA)
    add_subdirectory(ext/zlib ZLib)
endif()

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)
set(MINIZIP_SRC ext/zlib/contrib/minizip/ioapi.c ext/zlib/contrib/minizip/mztools.c ext/zlib/contrib/minizip/unzip.c ext/zlib/contrib/minizip/zip.c)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(MINIZIP_SRC ext/zlib/contrib/minizip/iowin32.c ext/zlib/contrib/minizip/ioapi.c ext/zlib/contrib/minizip/mztools.c ext/zlib/contrib/minizip/unzip.c ext/zlib/contrib/minizip/zip.c)
endif()

add_executable(CrossCraft-Classic ${MINIZIP_SRC} ${SRC_FILES} ${INC_FILES})
target_include_directories(CrossCraft-Classic PUBLIC ext/Stardust-Celeste/ext/glm/glm/ ext/FNL/Cpp ext/zlib/)

if(NOT VITA)
    target_link_libraries(CrossCraft-Classic SC-Entry Stardust-Celeste zlib)
else()
    target_link_libraries(CrossCraft-Classic SC-Entry Stardust-Celeste zlibstatic)
endif()

if(WIN32)
    target_link_libraries(CrossCraft-Classic ws2_32.lib)
else()
    target_compile_options(CrossCraft-Classic PRIVATE -g)
endif()

if(PSP)
    target_compile_options(CrossCraft-Classic PRIVATE -g)
    create_pbp_file(
        TARGET CrossCraft-Classic
        TITLE CrossCraft-Classic
        ICON_PATH ../src/assets/psp/ICON0.png
        BACKGROUND_PATH ../src/assets/psp/PIC1.png
        BUILD_PRX 1
    )
endif()

if(VITA)
    SET(CMAKE_SYSTEM_NAME Generic)
    SET(CMAKE_SYSTEM_VERSION 1)
    SET(CMAKE_SYSTEM_PROCESSOR "arm7-a")
    SET(CMAKE_C_COMPILER arm-vita-eabi-gcc)
    SET(CMAKE_CXX_COMPILER arm-vita-eabi-g++)
    set(CMAKE_ASM_COMPILER arm-vita-eabi-gcc)
    set(CMAKE_STRIP arm-vita-eabi-strip)
    set(CMAKE_AR arm-vita-eabi-ar)
    set(CMAKE_LINKER arm-vita-eabi-ld)
    set(CMAKE_NM arm-vita-eabi-nm)
    set(CMAKE_OBJCOPY arm-vita-eabi-objcopy)
    set(CMAKE_OBJDUMP arm-vita-eabi-objdump)
    set(CMAKE_RANLIB arm-vita-eabi-ranlib)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -Wl,-q")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wl,-q")

    set(VITA_ELF_CREATE vita-elf-create)
    set(VITA_ELF_EXPORT vita-elf-export)
    set(VITA_LIBS_GEN vita-libs-gen)
    set(VITA_MAKE_FSELF vita-make-fself)
    set(VITA_MKSFOEX vita-mksfoex)
    set(VITA_PACK_VPK vita-pack-vpk)

    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

    set(VITA_APP_NAME "CrossCraft Classic")
    set(VITA_TITLEID "CCC000110")
    set(VITA_VERSION "01.10")

    target_include_directories(CrossCraft-Classic PUBLIC ext/Stardust-Celeste/include/)
    target_link_libraries(CrossCraft-Classic SceCtrl_stub vitaGL vitashark mathneon SceShaccCg_stub SceShaccCgExt SceAppMgr_stub SceGxm_stub SceCommonDialog_stub SceProcessmgr_stub SceDisplay_stub SceKernelDmacMgr_stub ScePower_stub taihen_stub)

    vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})

    vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE src/assets/vita/icon0.png sce_sys/icon0.png
        FILE src/assets/vita/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
        FILE src/assets/vita/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
        FILE src/assets/vita/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
        FILE texturepacks/default.zip texturepacks/default.zip
    )
endif()
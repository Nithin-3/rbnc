cmake_minimum_required(VERSION 3.20)
project(rbnc C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(CURL REQUIRED libcurl)

add_executable(game
    game.c world.c player.c camera.c render.c object.c args.c ws.c input.c hud.c
)
target_include_directories(game PRIVATE ${SDL3_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
target_link_libraries(game PRIVATE ${SDL3_LIBRARIES} ${CURL_LIBRARIES})
if(UNIX)
    target_link_libraries(game PRIVATE m)
endif()
target_compile_options(game PRIVATE ${SDL3_CFLAGS_OTHER})

find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    add_custom_target(format COMMAND ${CLANG_FORMAT} -i ${CMAKE_SOURCE_DIR}/*.c ${CMAKE_SOURCE_DIR}/*.h)
    add_custom_target(check-format COMMAND ${CLANG_FORMAT} --dry-run --Werror ${CMAKE_SOURCE_DIR}/*.c ${CMAKE_SOURCE_DIR}/*.h)
endif()

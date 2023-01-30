cmake_minimum_required(VERSION 3.17)
project(ImGui)

set(EXEC_NAME "Example ImGui Application")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(LOG_FILE="example.txt")

if(WIN32)
    add_compile_definitions(OS_WINDOWS)
elseif(MSVC OR MSYS OR MINGW)
    add_compile_definitions(OS_WINDOWS)
    add_compile_definitions(OS_WINCROSS)
elseif(APPLE)
    add_compile_definitions(OS_MACOS)
elseif(UNIX)
    add_compile_definitions(OS_LINUX)
endif()

add_subdirectory(libs)

file(GLOB_RECURSE SRC_FILES "src/*.c" "src/*.cpp" "src/*.m" "src/*.mm")
file(GLOB_RECURSE RES_FILES "resources/*")

add_executable(${PROJECT_NAME} ${SRC_FILES} ${RES_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES 
    OUTPUT_NAME ${EXEC_NAME}
)

if (APPLE)
    set_source_files_properties(${RES_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        MACOSX_BUNDLE TRUE
    )
    target_sources(${PROJECT_NAME} PUBLIC ${RES_FILES})

    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    SET(EXTRA_LIBS ${METAL_LIBRARY} ${QUARTZCORE_LIBRARY})
    target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS})
else()
    file(COPY ${RES_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

target_link_libraries(${PROJECT_NAME} imgui plog stb_image nfd)
target_include_directories(${PROJECT_NAME} PUBLIC src)

add_compile_definitions(METAL_RENDERER)


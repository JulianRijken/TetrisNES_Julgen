cmake_minimum_required(VERSION 3.18)
project(TestGround)

add_subdirectory(Julgen)

set(TARGET_NAME TetrisNES)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


# Add all files from game
set(SOURCE_FILES
                                ${SOURCE_DIR}/GameEntry.cpp
    ${SOURCE_DIR}/Tetris.h
    ${SOURCE_DIR}/Piece.h       ${SOURCE_DIR}/Piece.cpp
    ${SOURCE_DIR}/GameState.h   ${SOURCE_DIR}/GameState.cpp
              
    ${SOURCE_DIR}/Block.h       ${SOURCE_DIR}/Block.cpp
    ${SOURCE_DIR}/Grid.h        ${SOURCE_DIR}/Grid.cpp
)

# Adds the source files to the executible
add_executable(${TARGET_NAME}
    ${SOURCE_FILES}
)

# Include source directories
target_include_directories(${TARGET_NAME} PUBLIC
    ${SOURCE_DIR}
)

# enable c++20 features
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
set_property(TARGET ${TARGET_NAME} PROPERTY
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS FALSE
)

# compile at max warning level + treat warnings as errors
target_compile_options(${TARGET_NAME} PUBLIC
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /MP>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Visual studio needs to be special
if(WIN32)
    # Fix startup project for visual stuidio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
    # Fix working directory for visual studio
    set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>")
endif()


target_link_libraries(${TARGET_NAME} PRIVATE julgen)

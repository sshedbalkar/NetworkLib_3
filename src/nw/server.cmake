set(PROJECT_NAME mmo_server)
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)
# Add source files
set(SERVER_SOURCE_FILES 
nw/MMO_Server.cpp
log/Logging.cpp
thread/asyncgetline.cpp
)

# the above paths and defines will be used in this build
add_executable(${PROJECT_NAME} WIN32 ${SERVER_SOURCE_FILES})
# set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG>)
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/local/include")

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/bin"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
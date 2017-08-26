########################################
#
# global cmake settings
cmake_minimum_required(VERSION 3.1)

project(daemon_inotify)

##########################################
#
# Setting headers and sources
#set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(SOURCES ${SOURCE_DIR}/daemon_inotify.c)
#set(HEADERS)
#include_directories(${INCLUDE_DIR})

#########################################
#
# Creating main executable target
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_executable(${PROJECT_NAME} ${SOURCES})
#add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
#target_link_libraries(${PROJECT_NAME} pthread)


# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

#########################################
#
# Additional targets/options

# additional options (can be specified in command line argument i.e: -DOPTIONNAME=ON/OFF
option(BUILDDOC "Generate Doxygen documentation" OFF)

# Creates the 'doc' build target that generates API documentation
if (BUILDDOC)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile.in @ONLY)
        add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile.in
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    endif(DOXYGEN_FOUND)
endif(BUILDDOC)
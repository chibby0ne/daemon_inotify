########################################
#
# global cmake settings
cmake_minimum_required(VERSION 3.1)

project(daemon_inotify)

##########################################
#
# Setting headers and sources
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(SOURCES ${SOURCE_DIR}/daemon_inotify.c ${SOURCE_DIR}/linked_list.c)
set(SOURCES_LINKED_LIST_TEST ${SOURCE_DIR}/test_linked_list.c ${SOURCE_DIR}/linked_list.c)
set(HEADERS ${INCLUDE_DIR}/linked_list.h)
include_directories(${INCLUDE_DIR})

#########################################
#
# Creating main executable target
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_executable(test_linked_list ${SOURCES_LINKED_LIST_TEST} ${HEADERS})
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

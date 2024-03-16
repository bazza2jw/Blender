#Common CMAKE definitons for 
# where to install
# Get the common definitons at the higher level - BOOST
#
# Common CMAKE definitions
set(CMAKE_CXX_STANDARD 20)
# Set project vaiables
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})  # Set the project source code location (location of this file)
# If the install dir changes check Common/common.h
set(CMAKE_INSTALL_PREFIX /usr/local)
message("Install Dir: ${CMAKE_INSTALL_PREFIX}")

# where the components go
set(EXE_DIR FaceTracker/bin)
set(LIB_DIR FaceTracker/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
#
# Debug and trace options
# enable this to insert breakpoints into throws and catches
add_definitions(-DTRAP_ENABLE)
# Enable debug trace in build
#add_definitions(-DTRACE_ON)
# NB: may on module by module basis use - target_compile_options(my_library PRIVATE -Werror -Wall -Wextra)
SET(CMAKE_CXX_FLAGS "-Wno-psabi ${CMAKE_CXX_FLAGS}")
#

    # Include wxWidgets directory
set(wxWidgets_USE_UNICODE ON)
set(wxWidgets_CONFIGURATION gtk3u)
find_package(wxWidgets REQUIRED html net core base xrc adv xml propgrid richtext webview)
include(${wxWidgets_USE_FILE})
# Set a variable with all the required wxWdiget library linker flags
SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
#


SET (LINKER_FLAGS "-lcrypt" )
SET(CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS} " )
SET(CMAKE_SHARED_LINKER_FLAGS "${LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS} ")

# flag a unix build
add_definitions(-D_UNIX)
#enable debug tracing
#add_definitions(-DTRACE_ON)
#
#
# Enable the GUI build
set(EXTRA_LIB_FLAGS  socket++ atomic pthread rt m )




# This is the cross compile target definition file
#
# cmake .. -DCMAKE_TOOLCHAIN_FILE=rpi.cmake
#
# Define our host system
#set(BOOST_DEBUG ON)
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
# Adjust to suit actual environment
SET(DEVROOT $ENV{HOME}/Work)
# where the sysroot is
SET(PIROOT ${DEVROOT}/opt/Rpi2)
# where the development tools are
SET(PITOOLS $ENV{HOME}/x-tools/arm-rpi-linux-gnueabihf/bin)
#
# Define the cross compiler locations
SET(CMAKE_C_COMPILER   ${PITOOLS}/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER ${PITOOLS}/arm-linux-gnueabihf-g++)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#
SET(CMAKE_C_FLAGS "-march=armv8-a -mtune=cortex-a53 -mfpu=crypto-neon-fp-armv8 ${CMAKE_C_FLAGS} -DDL_RASPBERRY_PI_BUILD")
SET(CMAKE_CXX_FLAGS "-march=armv8-a -mtune=cortex-a53 -mfpu=crypto-neon-fp-armv8 ${CMAKE_CXX_FLAGS} -DDL_RASPBERRY_PI_BUILD")
#
# Define the sysroot path for the RaspberryPi distribution in our tools folder
SET(CMAKE_SYSROOT ${PIROOT}) 
SET(CMAKE_FIND_ROOT_PATH ${PIROOT})
INCLUDE_DIRECTORIES(${PIROOT}/usr/include/arm-linux-gnueabihf ) # Use our definitions for compiler tools
INCLUDE_DIRECTORIES(${PIROOT}/usr/local/include)
LINK_DIRECTORIES(/lib/arm-linux-gnueabihf)
LINK_DIRECTORIES(/usr/lib/arm-linux-gnueabihf)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search for libraries and headers in the target directories only
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(BOOST_ROOT ${PIROOT}/usr)
SET(BOOST_DIR ${BOOST_ROOT})
SET(BOOST_PREFIX ${BOOST_DIR})
SET(BOOST_LIBRARYDIR ${PIROOT}/usr/lib/arm-linux-gnueabihf )
SET(BOOST_INCLUDEDIR ${PIROOT}/usr/include/boost)
# Force the linker to look in the sysroot where the libraries actually are - needed for some of the cmake tests
SET (PI_LINKER_FLAGS "-Wl,-rpath-link=${PIROOT}/lib/arm-linux-gnueabihf,-rpath-link=${PIROOT}/usr/lib/arm-linux-gnueabihf -L${PIROOT}/usr/lib/arm-linux-gnueabihf -lcrypt -lpigpio")
SET(CMAKE_EXE_LINKER_FLAGS "${PI_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}" )
SET(CMAKE_SHARED_LINKER_FLAGS "${PI_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")

add_definitions(-Wall )
 

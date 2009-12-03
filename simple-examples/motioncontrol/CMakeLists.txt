#CMAKE_MINIMUM_REQUIRED(VERSION 2.2.2)

PROJECT(RobotDemo)

SET( CMAKE_INSTALL_PREFIX ./install/ CACHE PATH "Installation directory" )
MESSAGE( "Setting installation directory to ${CMAKE_INSTALL_PREFIX}" )

SET( PROJ_SOURCE_DIR ${RobotDemo_SOURCE_DIR} )
SET( PROJ_BINARY_DIR ${RobotDemo_BINARY_DIR} )

SET( OROCOS_TARGET gnulinux CACHE STRING "The Operating System target.")
SET( OROCOS_INSTALL ${CMAKE_INSTALL_PREFIX} CACHE PATH "The Orocos installation directory.")

SET( KDL_INSTALL ${CMAKE_INSTALL_PREFIX} CACHE PATH "The KDL installation directory.")

SET( OCL_INSTALL ${CMAKE_INSTALL_PREFIX} CACHE PATH "The Orocos-Components installation directory.")

IF ( NOT CMAKE_BUILD_TYPE )
  SET( CMAKE_BUILD_TYPE Release )
  MESSAGE( "Setting build type to '${CMAKE_BUILD_TYPE}'" )
ELSE ( NOT CMAKE_BUILD_TYPE )
  MESSAGE( "Build type set to '${CMAKE_BUILD_TYPE}' by user." )
ENDIF ( NOT CMAKE_BUILD_TYPE )

INCLUDE(config/global_setup.cmake)

## Find Orocos RTT.

INCLUDE (${PROJ_SOURCE_DIR}/config/FindOrocosRTT.cmake)

## Find KDL

INCLUDE (${PROJ_SOURCE_DIR}/config/FindKDL.cmake)

## Find BFL

#INCLUDE (${PROJ_SOURCE_DIR}/config/FindBFL.cmake)

## Find Orocos-Components

INCLUDE (${PROJ_SOURCE_DIR}/config/FindComponents.cmake)

INCLUDE_DIRECTORIES( ${PROJ_SOURCE_DIR} )


SET(APP_INCLUDE ${KDL_INCLUDE_DIRS} ${OROCOS_RTT_INCLUDE_DIRS} ${OCL_INCLUDE_DIR})
SET(APP_LIBRARIES ${KDL_LIBS} ${KDLTK_LIBS} ${OROCOS_RTT_LIBS} ${OCL_LIBS} )

#LINK_DIRECTORIES( ${APP_LIBRARIES} )
INCLUDE_DIRECTORIES( ${APP_INCLUDE} )



OPTION( BUILD_APP "Build Demo" ON )
IF ( BUILD_APP )
    ADD_SUBDIRECTORY( src )
ENDIF (BUILD_APP )




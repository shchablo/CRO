################################################################################
# CMakeLists.txt for cro project.
# Shchablo Konstantin (Shchablo@gmail.com)
################################################################################

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(cro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)


# MODULE ----------------------------------------------------------------------

  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
  
  ## CERN_ROOT ----------------------------------------------------------------
	
  list(APPEND CMAKE_MODULE_PATH "$ENV{ROOTSYS}/etc/cmake")
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  find_package(ROOT QUIET)
  IF(NOT ROOT_FOUND)
    MESSAGE(STATUS "ROOT is not found")
    return()
  ENDIF()
	
	IF(NOT ROOT_INCLUDE_DIR)
	  find_path(ROOT_INCLUDE_DIR
	  NAMES TROOT.h
	  PATHS
	  /usr/include/root
	  /usr/local/include/root)
  ENDIF()
	
  find_package(ROOT CONFIG REQUIRED)
  set(ROOT_DIR $ENV{ROOTSYS})
  include("$ENV{ROOTSYS}/cmake/RootMacros.cmake")  
	
  include("${ROOT_USE_FILE}")
	
  ROOT_GENERATE_DICTIONARY(G__RoEvent ${CMAKE_CURRENT_SOURCE_DIR}/include/RoEvent.h LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/RoEventLinkDef.h) 
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs)
  add_library(RoEvent SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/RoEvent.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/include/RoEvent.h G__RoEvent.cxx)
	
	include_directories(${ROOT_INCLUDE_DIR})
	include_directories(./)
	## -------------------------------------------------------------------------
  
  FIND_PACKAGE(Boost REQUIRED)
  FIND_PACKAGE(Threads REQUIRED)
#-------------------------------------------------------------------------------

# PROJECT INCLUDES -------------------------------------------------------------
  include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  ## main 
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/OmaReader)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/DqAnalysis)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/DqFeatures)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/DqROOT)
#-------------------------------------------------------------------------------

# PROJECT SRC ------------------------------------------------------------------
file(GLOB MAIN "main.cc")
file(GLOB SIG "units/sig.cc")
file(GLOB PED "units/ped.cc")
file(GLOB REA "units/ready.cc")
file(GLOB CBV1 "units/cbv1.cc")
  ## main 
  file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
  file(GLOB SRComaReader "${CMAKE_CURRENT_SOURCE_DIR}/src/OmaReader/*.cc")
  file(GLOB SRCdqAnalysis "${CMAKE_CURRENT_SOURCE_DIR}/src/DqAnalysis/*.cc")
  file(GLOB SRCdqFeatures "${CMAKE_CURRENT_SOURCE_DIR}/src/DqFeatures/*.cc")
  file(GLOB SRCdqROOT "${CMAKE_CURRENT_SOURCE_DIR}/src/DqROOT/*.cc")
    ### inData
    set(CPPS    ${MAIN} ${SRC}  ${SRComaReader} ${SRCdqAnalysis}  ${SRCdqFeatures})
  set(SIGCPPS   ${SIG}  ${SRC}  ${SRComaReader} ${SRCdqAnalysis}  ${SRCdqFeatures})
  set(PEDCPPS   ${PED}  ${SRC}  ${SRComaReader} ${SRCdqAnalysis}  ${SRCdqFeatures})
  set(REACPPS   ${REA}  ${SRC}  ${SRComaReader} ${SRCdqAnalysis}  ${SRCdqFeatures}) 
  set(CBV1CPPS  ${CBV1} ${SRC}  ${SRComaReader} ${SRCdqAnalysis}  ${SRCdqFeatures} ${SRCdqROOT})


#-------------------------------------------------------------------------------

# COMPILER SETTING -------------------------------------------------------------

  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
  if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please update your C++ compiler.")
  endif()

add_definitions(-Wall -lpthread -lfftw3)

#Link to ROOT.

add_executable(${PROJECT_NAME} ${CPPS})
target_link_libraries(${PROJECT_NAME} RoEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(sig ${SIGCPPS})
target_link_libraries(sig RoEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
add_executable(ped ${PEDCPPS})
target_link_libraries(ped RoEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
add_executable(rea ${REACPPS})
target_link_libraries(rea RoEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
add_executable(cbv1 ${CBV1CPPS})
target_link_libraries(cbv1 RoEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

# LIBRARIES --------------------------------------------------------------------
file(GLOB PCM_FILES "${PROJECT_BINARY_DIR}/*.pcm")
file(COPY ${PCM_FILES} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#-------------------------------------------------------------------------------
################################################################################


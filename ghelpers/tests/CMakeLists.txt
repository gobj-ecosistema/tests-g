##############################################
#   CMake
##############################################
project(tests-ghelpers C)
cmake_minimum_required(VERSION 2.8)
include(CheckIncludeFiles)
include(CheckSymbolExists)

set(CMAKE_INSTALL_PREFIX /yuneta/development/yuneta/^gobj-ecosistema/tests-g/output)

set(INC_DEST_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(LIB_DEST_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(BIN_DEST_DIR ${CMAKE_INSTALL_PREFIX}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")

IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
  add_definitions(-DNOT_INCLUDE_LIBUNWIND=1)
ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")

ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")


##############################################
#   Source
##############################################
add_subdirectory(json_config)
add_subdirectory(output_upd_client)
add_subdirectory(rotatory)
add_subdirectory(glogger)
add_subdirectory(json_helper)
add_subdirectory(json_helper2)
add_subdirectory(gbuffer)
add_subdirectory(rc_list)
add_subdirectory(timeranger)
add_subdirectory(timeranger2)
add_subdirectory(timeranger3)
add_subdirectory(time_helper2)
add_subdirectory(json_xml)
add_subdirectory(tr_table1)
add_subdirectory(tr_table2)
add_subdirectory(tr_table3)
add_subdirectory(tr_table4)
add_subdirectory(stats)


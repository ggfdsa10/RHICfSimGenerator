
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(Compile)

set(COMPILE_SOURCE_DIRECTORY_LIST
  ${CMAKE_SOURCE_DIR}/source

  CACHE INTERNAL ""
)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

# find_package(ROOT REQUIRED)
find_package(ROOT REQUIRED COMPONENTS RIO Net Eve EG RGL Geom Gui Graf Graf3d Ged Matrix MathCore)

include(${ROOT_USE_FILE})

set(TOOLS_LIBRARIES_LIST ${ROOT_LIBRARIES} CACHE INTERNAL "")

link_directories(${LINK_DIRECTORIES})

foreach(_directory ${COMPILE_SOURCE_DIRECTORY_LIST})
  include_directories(${_directory})

  string(REPLACE ${CMAKE_SOURCE_DIR} "" tmpString ${_directory})
  string(REPLACE / _ _name ${tmpString})

  list(APPEND SOURCE_NAME_LIST ${_name})
  set(${_name}_G_NAME G__${_name}Dict)
  file(GLOB ${_name}_LINKDEF_FILE ${_directory}/LinkDef.h)
  file(GLOB ${_name}_SOURCE_FILES ${_directory}/*.cc)
  file(GLOB ${_name}_HEADER_FILES ${_directory}/*.hh)
endforeach(_directory)

add_definitions(${ROOT_CXX_FLAGS})

foreach(_name ${SOURCE_NAME_LIST})
  ROOT_GENERATE_DICTIONARY(${${_name}_G_NAME}
    ${${_name}_HEADER_FILES}
    LINKDEF ${${_name}_LINKDEF_FILE}
  )
endforeach(_name)

foreach(_name ${SOURCE_NAME_LIST})
  foreach(_file ${${_name}_SOURCE_FILES})
    list(APPEND LIB_LIST ${_file})
  endforeach(_file)
  list(APPEND LIB_LIST ${${_name}_G_NAME}.cxx)
endforeach(_name)

add_library(RHICfSim SHARED ${LIB_LIST})
target_link_libraries(RHICfSim ${TOOLS_LIBRARIES_LIST})

# =============== Do insert what you want to compiled code name below ====================
# add_executable(analysis ../analysis.cc ${${_name}_SOURCE_FILES} ${${_name}_HEADER_FILES})
# target_link_libraries(analysis SejongTPCsw)

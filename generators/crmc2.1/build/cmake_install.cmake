# Install script for directory: /home/shlee/workspace/rhicf/generators/crmc2.1

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/shlee/workspace/rhicf/generators/crmc2.1/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/crmc" TYPE FILE FILES "/home/shlee/workspace/rhicf/generators/crmc2.1/build/src/CRMCconfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/shlee/workspace/rhicf/generators/crmc2.1/build/libCrmc.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCrmc.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCrmc.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCrmc.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/shlee/workspace/rhicf/generators/crmc2.1/build/crmc")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/crmc" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/crmc")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/crmc")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/etc" TYPE FILE RENAME "crmc.param" FILES "/home/shlee/workspace/rhicf/generators/crmc2.1/build/crmc.param.install")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/crmc" TYPE FILE FILES
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/epos.inics"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/epos.inics.lhc"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/epos.inidi"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/epos.iniev"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/epos.inirj"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/epos.inirj.lhc"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/epos.initl"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/qgsdat-II-04.lzma"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/sectnu-II-04"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/qgsdat-III.lzma"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/tabs/sectnu-III"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  MESSAGE ("unpack share/crmc/qgsdat-III.lzma")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  EXECUTE_PROCESS(COMMAND unlzma share/crmc/qgsdat-III.lzma)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/crmc" TYPE FILE FILES
    "/home/shlee/workspace/rhicf/generators/crmc2.1/build/src/epos.inc"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/epos.incems"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/epos.inchy"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/epos.incico"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/epos.incpar"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/epos.incsem"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/crmc" TYPE FILE FILES
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/CRMC.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/CRMCstat.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/OutputPolicyNone.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/OutputPolicyLHE.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/CRMCoptions.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/build/src/CRMCinterface.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/OutputPolicyROOT.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/OutputPolicyHepMC3.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/CRMChepevt.h"
    "/home/shlee/workspace/rhicf/generators/crmc2.1/src/CRMChepmc3.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/shlee/workspace/rhicf/generators/crmc2.1/build/src/epos/cmake_install.cmake")
  include("/home/shlee/workspace/rhicf/generators/crmc2.1/build/src/qgsjetII-04/cmake_install.cmake")
  include("/home/shlee/workspace/rhicf/generators/crmc2.1/build/src/qgsjetIII/cmake_install.cmake")
  include("/home/shlee/workspace/rhicf/generators/crmc2.1/build/ExampleAnalyser/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/shlee/workspace/rhicf/generators/crmc2.1/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

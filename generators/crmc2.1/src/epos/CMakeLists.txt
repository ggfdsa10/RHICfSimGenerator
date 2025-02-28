INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")
FILE(GLOB epos_files
  ${CMAKE_CURRENT_SOURCE_DIR}/*.f
  ${CMAKE_CURRENT_BINARY_DIR}/*.f)



IF (CRMC_STATIC)
  add_library(CrmcBasic STATIC ${epos_files}) #always needed
ELSE (CRMC_STATIC)
  add_library(CrmcBasic SHARED ${epos_files}) #always needed
  add_library(Epos      SHARED ${base_files})
  INSTALL (TARGETS Epos
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    )
  target_link_libraries(Epos CrmcBasic)
ENDIF (CRMC_STATIC)
INSTALL (TARGETS CrmcBasic
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static)


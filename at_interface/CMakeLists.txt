include(AnalysisTree)

set(SOURCES
        PidFiller.cpp
  )

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")
add_library(PidAT SHARED ${SOURCES} G__PidAT.cxx)

message(${PROJECT_INCLUDE_DIRECTORIES})

include_directories(${PROJECT_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src)
ROOT_GENERATE_DICTIONARY(G__PidAT ${HEADERS} LINKDEF PidATLinkDef.h)
target_link_libraries(PidAT ${ROOT_LIBRARIES} AnalysisTreeBase AnalysisTreeInfra Pid)

install(TARGETS PidAT EXPORT AnalysisTreeFillerTargets
       LIBRARY DESTINATION lib
       ARCHIVE DESTINATION lib
       RUNTIME DESTINATION bin
       INCLUDES DESTINATION include
       )

install(
       FILES
       ${HEADERS}
       DESTINATION
       include
       COMPONENT
       Devel
)

set(PCM_FILE_NAME libPidAT)

install(
       FILES
       "${CMAKE_CURRENT_BINARY_DIR}/${PCM_FILE_NAME}_rdict.pcm"
       "${CMAKE_CURRENT_BINARY_DIR}/${PCM_FILE_NAME}.rootmap"
       DESTINATION
       lib
       OPTIONAL
)

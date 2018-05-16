set(SRC
    main.cpp
)

#source_group(apps\\esmtool FILES ${ESMTOOL})

# Main executable
openmw_add_executable(mission7
    ${SRC}
)

target_link_libraries(mission7
  components
)

#if (BUILD_WITH_CODE_COVERAGE)
  #add_definitions (--coverage)
  #target_link_libraries(esmtool gcov)
#endif()

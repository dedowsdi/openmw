set(SRC
    main.cpp
    command.cpp
)

# Main executable
openmw_add_executable(bt
    ${SRC}
)

target_link_libraries(bt
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  components
)

set(SRC
  main.cpp
  record.cpp
  labels.cpp
  command.cpp
  textnumber.cpp
)
source_group(apps\\et FILES ${SRC})

# Main executable
openmw_add_executable(et
    ${SRC}
)

target_link_libraries(et
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  components
)

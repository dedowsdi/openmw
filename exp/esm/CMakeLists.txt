set (SRC 
    exp_esmreader.cpp
    )

add_executable(exp_esmreader ${SRC})
target_link_libraries(exp_esmreader components)

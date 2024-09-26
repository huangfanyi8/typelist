file(GLOB TEST *.cpp)

MESSAGE("${TEST}")

foreach(test_file  ${TEST})
    GET_FILENAME_COMPONENT(file_name ${test_file} NAME_WE)
    message(${file_name})
    add_executable(${file_name} ${test_file})
    set_target_properties(${file_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endforeach ()

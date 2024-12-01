add_executable(day1 main.cpp)

add_custom_target(input SOURCES test.txt input.txt)
configure_file(test.txt ${CMAKE_CURRENT_BINARY_DIR}/test.txt COPYONLY)
configure_file(input.txt
               ${CMAKE_CURRENT_BINARY_DIR}/input.txt COPYONLY)

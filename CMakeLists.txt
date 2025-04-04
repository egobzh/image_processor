set(CMAKE_CXX_STANDARD 20)
add_executable(
    image_processor
    image_processor.cpp
    src/parse_tools.cpp
    src/file_tools.cpp
    src/image_obj.cpp
    src/filters.cpp
)

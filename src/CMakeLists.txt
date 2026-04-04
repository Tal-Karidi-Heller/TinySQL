#project(my_project)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
        app_logic
        parser.cpp
        tokenizer.cpp
        engine.cpp
)

add_executable(
        main
        main.cpp
)

target_link_libraries(
        main
        PRIVATE
        app_logic
)
#project(my_project)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
        app_logic
        parser.cpp
        tokenizer.cpp
        utils.cpp
)

add_executable(
        my_app
        main.cpp
)

target_link_libraries(
        my_app
        PRIVATE
        app_logic
)
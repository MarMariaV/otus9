cmake_minimum_required(VERSION 3.16)
project(otus9)

set(CMAKE_CXX_STANDARD 17)

add_executable(otus9 otus9.cpp)
add_library( libasync async.cpp async.h bulk_class.cpp bulk_class.h observer.cpp observer.h)

if (MSVC)
    target_compile_options(otus9 PRIVATE
            /W4
            )
else ()
    target_compile_options(otus9 PRIVATE
            -Wall -Wextra -pedantic
            )
endif()

target_link_libraries(otus9 PRIVATE
        libasync
        )

install(TARGETS otus9 RUNTIME DESTINATION lib)
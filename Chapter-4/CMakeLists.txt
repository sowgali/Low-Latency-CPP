file(GLOB SOURCES *.cpp)

include_directories(${PROJECT_SOURCE_DIR})

add_library(libcommon STATIC ${SOURCES})

list(APPEND LIBS libcommon)
list(APPEND LIBS pthread)

add_executable(thread_example thread_example.cpp)
target_link_libraries(thread_example PUBLIC ${LIBS})
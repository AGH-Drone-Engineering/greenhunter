set(TARGET CircleDetector)

set(SRCS
    inc/CircleDetector.h
    inc/CircleSquareDetector.h
    CircleDetector.cpp
    CircleSquareDetector.cpp
)

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
    Boost::boost
    Greenhunter
)

add_library(${TARGET} STATIC ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

add_executable(${TARGET}_demo demo.cpp)
target_link_libraries(${TARGET}_demo PRIVATE ${TARGET})

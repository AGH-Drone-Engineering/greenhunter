set(TARGET Scout)

set(SRCS
    inc/Scout.h
    Scout.cpp
)

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
    Drone
    CircleDetector
    CircleMap
)

add_library(${TARGET} STATIC ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

add_executable(${TARGET}_main main.cpp)
target_link_libraries(${TARGET}_main PRIVATE ${TARGET} Boost::thread)

set(TARGET Scout)

set(SRCS
    inc/Scout.h
    Scout.cpp
    main.cpp
)

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
    Boost::boost
    Boost::thread
    Drone
    CircleDetector
    CircleMap
)

add_executable(${TARGET} ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

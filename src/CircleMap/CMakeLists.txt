set(TARGET CircleMap)

set(SRCS
    inc/CircleMap.h
    inc/MapLocalizer.h
    inc/MapServer.h
    CircleMap.cpp
    MapLocalizer.cpp
    MapServer.cpp
)

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
    Boost::boost
    Boost::thread
    Drone
    CircleDetector
)

add_library(${TARGET} STATIC ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

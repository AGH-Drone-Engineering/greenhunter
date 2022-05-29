set(TARGET CircleMap)

set(SRCS
    inc/CircleMap.h
    inc/MapLocalizer.h
    CircleMap.cpp
    MapLocalizer.cpp
)

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
    Drone
)

add_library(${TARGET} STATIC ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

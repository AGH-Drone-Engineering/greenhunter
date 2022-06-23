set(TARGET CircleDetector)

set(SRCS
    inc/CircleDetector.h
    CircleDetector.cpp
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

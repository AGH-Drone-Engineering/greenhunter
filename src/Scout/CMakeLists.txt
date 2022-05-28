set(TARGET Scout)

set(SRCS
    inc/Scout.h
    Scout.cpp
)

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
)

add_library(${TARGET} STATIC ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

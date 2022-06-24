set(TARGET MissionLogger)

set(SRCS
    inc/MissionLogger.h
    MissionLogger.cpp
)

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
    Boost::boost
    Boost::thread
    Boost::filesystem
    Greenhunter
    CircleMap
)

add_library(${TARGET} STATIC ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

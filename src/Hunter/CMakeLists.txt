set(TARGET Hunter)

set(SRCS
    inc/Hunter.h
    inc/MavClient.h
    inc/MapClient.h
    Hunter.cpp
    MavClient.cpp
    MapClient.cpp
    main.cpp
    )

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
    Boost::boost
    Boost::thread
    Greenhunter
    CircleDetector
    CircleMap
    MissionLogger
    )

add_executable(${TARGET} ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

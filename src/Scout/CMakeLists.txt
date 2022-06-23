set(TARGET Scout)

set(SRCS
    inc/Scout.h
    inc/Drone.h
    inc/TelemetryServer.h
    Scout.cpp
    Drone.cpp
    TelemetryServer.cpp
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
)

add_executable(${TARGET} ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

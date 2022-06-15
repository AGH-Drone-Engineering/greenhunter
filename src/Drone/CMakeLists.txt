set(TARGET Drone)

set(SRCS
    inc/Drone.h
    inc/Telemetry.h
    inc/FrameTelemetry.h
    inc/CameraParams.h
    inc/TelemetryServer.h
    Drone.cpp
    TelemetryServer.cpp
)

set(DEPS
    PUBLIC
    ${OpenCV_LIBS}
    Boost::boost
    Boost::thread
)

add_library(${TARGET} STATIC ${SRCS})
target_include_directories(${TARGET} PUBLIC inc)
target_link_libraries(${TARGET} ${DEPS})

set(TARGET Greenhunter)

set(SRCS
    inc/CameraParams.h
    inc/Telemetry.h
    inc/FrameTelemetry.h
    inc/Circle.h
    inc/Position.h
)

set(DEPS
    INTERFACE
    ${OpenCV_LIBS}
    Boost::boost
)

add_library(${TARGET} INTERFACE ${SRCS})
target_include_directories(${TARGET} INTERFACE inc)
target_link_libraries(${TARGET} ${DEPS})

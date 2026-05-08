project(GD32G553V_START LANGUAGES C CXX ASM)

add_library(GD32G553V_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32G553V_START/gd32g553v_start.c
    )

target_include_directories(GD32G553V_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32G553V_START
    )

target_link_libraries(GD32G553V_START PUBLIC GD32G5x3_standard_peripheral)

project(GD32G553C_START LANGUAGES C CXX ASM)

add_library(GD32G553C_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32G553C_START/gd32g553c_start.c
    )

target_include_directories(GD32G553C_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32G553C_START
    )

target_link_libraries(GD32G553C_START PUBLIC GD32G5x3_standard_peripheral)

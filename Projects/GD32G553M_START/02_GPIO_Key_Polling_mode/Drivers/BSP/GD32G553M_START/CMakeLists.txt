project(GD32G553M_START LANGUAGES C CXX ASM)

add_library(GD32G553M_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32G553M_START/gd32g553m_start.c
    )

target_include_directories(GD32G553M_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32G553M_START
    )

target_link_libraries(GD32G553M_START PUBLIC GD32G5x3_standard_peripheral)

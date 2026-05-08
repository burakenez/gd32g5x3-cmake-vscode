project(GD32G553Q_EVAL LANGUAGES C CXX ASM)

add_library(GD32G553Q_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32G553Q_EVAL/gd32g553q_eval.c
    ${DRIVERS_DIR}/BSP/GD32G553Q_EVAL/gd32g553q_lcd_eval.c
    )

target_include_directories(GD32G553Q_EVAL PUBLIC
    ${DRIVERS_DIR}/BSP/GD32G553Q_EVAL
    )

target_link_libraries(GD32G553Q_EVAL PUBLIC GD32G5x3_standard_peripheral)

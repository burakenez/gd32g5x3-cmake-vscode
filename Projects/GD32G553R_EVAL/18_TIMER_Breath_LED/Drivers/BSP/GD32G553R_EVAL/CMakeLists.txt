project(GD32G553R_EVAL LANGUAGES C CXX ASM)

add_library(GD32G553R_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32G553R_EVAL/gd32g553r_eval.c
    )

target_include_directories(GD32G553R_EVAL PUBLIC
    ${DRIVERS_DIR}/BSP/GD32G553R_EVAL
    )

target_link_libraries(GD32G553R_EVAL PUBLIC GD32G5x3_standard_peripheral)

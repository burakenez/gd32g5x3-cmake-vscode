project(GD32G5x3_standard_peripheral LANGUAGES C CXX ASM)

# Comment-out unused source files.
add_library(GD32G5x3_standard_peripheral OBJECT
	${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_adc.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_can.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_cau.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_cau_aes.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_cau_des.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_cau_tdes.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_cla.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_cmp.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_cpdm.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_crc.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_dac.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_dbg.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_dma.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_exmc.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_exti.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_fac.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_fft.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_fmc.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_fmc_ramfunc.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_fwdgt.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_gpio.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_hpdf.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_hrtimer.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_i2c.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_lptimer.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_misc.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_pmu.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_qspi.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_rcu.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_rtc.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_spi.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_syscfg.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_timer.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_tmu.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_trigsel.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_trng.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_usart.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_vref.c
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Source/gd32g5x3_wwdgt.c
    )

target_include_directories(GD32G5x3_standard_peripheral PUBLIC
    ${DRIVERS_DIR}/GD32G5x3_standard_peripheral/Include
    )

# CMSIS header only library is linked.
target_link_libraries(GD32G5x3_standard_peripheral PUBLIC CMSIS)

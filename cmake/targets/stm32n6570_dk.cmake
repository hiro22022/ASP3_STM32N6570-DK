# STM32N6570-DK target settings (Makefile.target equivalent, phase 1 scope)

set(ASP3_TARGET_KERNEL_TRB
    "${ASP3_SRCDIR}/target/stm32n6570_dk/target_kernel.trb"
    CACHE FILEPATH "cfg pass 2 kernel template"
)
set(ASP3_TARGET_CHECK_TRB
    "${ASP3_SRCDIR}/target/stm32n6570_dk/target_check.trb"
    CACHE FILEPATH "cfg pass 3 check template"
)
set(ASP3_TARGET_KERNEL_CFG
    "${ASP3_SRCDIR}/target/stm32n6570_dk/target_kernel.cfg"
    CACHE FILEPATH "cfg supplemental kernel cfg"
)
set(ASP3_TARGET_OFFSET_TRB
    "${ASP3_SRCDIR}/arch/arm_m_gcc/common/core_offset.trb"
    CACHE FILEPATH "offset.h generation template"
)

set(ASP3_CFG_EXTRA_TABS
    --symval-table "${ASP3_SRCDIR}/arch/arm_m_gcc/common/core_sym.def"
)

set(ASP3_LDSCRIPT
    "${ASP3_SRCDIR}/target/stm32n6570_dk/STM32N657XX_LRUN_ns.ld"
)

set(ASP3_TARGET_INCLUDE_DIRS
    "${ASP3_SRCDIR}/target/stm32n6570_dk"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Inc"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Inc/Legacy"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/CMSIS/Include"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/CMSIS/Device/ST/STM32N6xx/Include"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/AppliNonSecure/Core/Inc"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Secure_nsclib"
    "${ASP3_SRCDIR}/syssvc"
    "${ASP3_SRCDIR}/kernel"
)

set(ASP3_ARCH_INCLUDE_DIRS
    "${ASP3_SRCDIR}/arch/arm_m_gcc/common"
    "${ASP3_SRCDIR}/arch/gcc"
    "${ASP3_SRCDIR}/arch/arm_m_gcc/stm32n6xx_stm32cube"
)

set(ASP3_TARGET_COMPILE_DEFINITIONS
    STM32N657xx
    USE_HAL_DRIVER
    USE_SYSTICK_AS_TIMETICK
    TOPPERS_CORTEX_M55
    __TARGET_FPU_FPV5
    TOPPERS_FPU_ENABLE
    TOPPERS_FPU_LAZYSTACKING
    TOPPERS_FPU_CONTEXT
    __TARGET_ARCH_THUMB=5
)

set(ASP3_TARGET_COMPILE_OPTIONS
    -mcpu=cortex-m55
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv5-d16
    -ffunction-sections
    -fdata-sections
)

set(ASP3_TARGET_LINK_OPTIONS
    -nostdlib
    --specs=nosys.specs
    -static
    -Wl,--start-group
    -lc
    -lm
    -Wl,--end-group
    -Wl,-u,_kernel_start
    -Wl,-T,${ASP3_LDSCRIPT}
)

# Kernel-side target sources (linked into libkernel)
set(ASP3_TARGET_KERNEL_SOURCES
    "${ASP3_SRCDIR}/target/stm32n6570_dk/target_kernel_impl.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/target_kernel_impl2.c"
    "${ASP3_SRCDIR}/arch/arm_m_gcc/stm32n6xx_stm32cube/core_timer.c"
)

# Startup objects (linked first into asp.elf)
set(ASP3_STARTUP_OBJECTS
    "${ASP3_SRCDIR}/arch/arm_m_gcc/common/start.S"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/Startup/startup_stm32n657xx.s"
)

# Application / HAL sources (Makefile.target TARGET_COBJS equivalent)
set(ASP3_TARGET_APPL_SOURCES
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/AppliNonSecure/Core/Src/system_stm32n6xx_ns.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/secure_nsc_stub.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/led_btn_joy.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/AppliNonSecure/Core/Src/main_kernel.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/AppliNonSecure/Core/Src/stm32n6xx_hal_msp.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/AppliNonSecure/Core/Src/stm32n6xx_it.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/AppliNonSecure/Core/Src/syscalls.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/AppliNonSecure/Core/Src/sysmem.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_cortex.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dma.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dma_ex.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_exti.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_gpio.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_pwr.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_pwr_ex.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rcc.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rcc_ex.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_tim.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_tim_ex.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_uart.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_uart_ex.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_ll_exti.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_ll_rcc.c"
    "${ASP3_SRCDIR}/target/stm32n6570_dk/stm32hcube/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_ll_utils.c"
)

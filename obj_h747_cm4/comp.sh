SRCS="../target/stm32h747_discovery_m7/stm32hcube/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c \
../target/stm32h747_discovery_m7/stm32hcube/CM7/Core/Src/main.c \
../target/stm32h747_discovery_m7/stm32hcube/CM7/Core/Src/stm32h7xx_hal_msp.c \
../target/stm32h747_discovery_m7/stm32hcube/CM7/Core/Src/stm32h7xx_it.c \
../target/stm32h747_discovery_m7/stm32hcube/CM7/Core/Src/syscalls.c \
../target/stm32h747_discovery_m7/stm32hcube/CM7/Core/Src/sysmem.c \
../target/stm32h747_discovery_m7/stm32hcube/CM7/Core/Startup/startup_stm32h747xihx.S \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c \
../target/stm32h747_discovery_m7/stm32hcube/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c"

TDIR=../target/stm32h747_discovery_m7/stm32hcube

INCLUDE="-I$TDIR/CM7/Core/Inc -I$TDIR/Drivers/STM32H7xx_HAL_Driver/Inc \
-I$TDIR/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I$TDIR/Drivers/CMSIS/Device/ST/STM32H7xx/Include \
-I$TDIR/Drivers/CMSIS/Include -I../syssvc -I../include \
-I../arch/gcc -I../arch/arm_m_gcc/stm32h7xx_stm32cube \
-I../arch/arm_m_gcc/common -I../target/stm32h747_discovery_m7"

GCC="arm-none-eabi-gcc -mcpu=cortex-m7 -std=gnu11 -g3 
        -DUSE_HAL_DRIVER -DSTM32H747xx -DCORE_CM7 -DDEBUG -c 
        -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP"

for SRC in $SRCS; do
    file=`basename $SRC`
    file=${file%.c}
    OPT="-MF objs/$file.d \
        -MT objs/$file.o --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb \
        -o objs/$file.o"
    echo $GCC $OPT $INCLUDE $SRC
    $GCC $OPT $INCLUDE $SRC
done

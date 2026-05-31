## 移植手順

  C:\Users\hiro2\.cursor\plans\n6570_カーネル起動移植_62949bf8.plan.md

## デバッグ
### 準備
  WSL: GDB 起動に必要
  ```
  % sudo apt install libncursesw5
  ```

### OpenOCD 起動
 ```
cd C:\cygwin64\home\hiro2\TECS\asp3\asp3_stm32n6570-dk\obj_n657
"C:\ST\STM32CubeIDE_2.1.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.openocd.win32_2.4.400.202601091506\tools\bin\openocd.exe" -s c:\ST\STM32CubeIDE_2.1.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.debug.openocd_2.3.300.202602021527\resources\openocd\st_scripts  -f interface/stlink-dap.cfg -f target/stm32n6x.cfg
 ```

### gdb 起動
 ```
cd C:\cygwin64\home\hiro2\TECS\asp3\asp3_stm32n6570-dk\obj_n657
C:\cygwin64\home\hiro2\arm-gcc\gcc-arm-none-eabi-10.3-2021.10\bin\arm-none-eabi-gdb.exe asp.elf
target remote localhost:3333
monitor reset halt
load
break sta_ker
break main_task
continue

break main
continue
info reg
backtrace
 ```
 
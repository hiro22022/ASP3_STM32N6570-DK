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

### LED 点滅周期の確認（タイマ修正後）

`dly_tsk` の RELTIM は **マイクロ秒**（0.5秒 = `500000`）。SysTick 周期は `CPU_CLOCK_HZ`（600MHz）と `TSTEP_HRTCNT`（1000）で 1ms/割込み。

```
continue
# sta_ker 通過後
print/x SysTick->LOAD          # 期待: 0x927c0 (600000)
print SystemCoreClock          # 期待: 600000000
print/x SCB->VTOR              # _kernel_vector_table アドレス

break target_hrt_handler
continue
# 数回停止して hrtcnt_current が 1000 ずつ増えることを確認

delete breakpoints
break main_task
continue
# n657_main_count が約 1 秒ごとに 2 増える（0.5秒 dly × 2）ことを確認
watch n657_main_count
continue
```

実機目視: LD1（緑）が **約 1Hz**（0.5秒 ON / 0.5秒 OFF）で点滅すれば OK。

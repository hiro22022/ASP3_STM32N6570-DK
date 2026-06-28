## デバッグ

### 動作確認済み（2026-06-28）

**LED 点滅 + syslog（Tera Term 115200）** まで確認済み。

#### ビルド

```bash
cd obj_n657 && make
```

成果物: `obj_n657/asp.elf`

#### 実機確認（GDB なしでも可）

1. OpenOCD 起動（下記）
2. GDB で `load` → `continue`（またはリセット後に OpenOCD だけ残して実行）
3. **Tera Term**: COM（ST-Link VCP）、**115200 8N1**
4. LD1（緑）が約 1Hz で点滅

#### 期待される syslog 出力

```
[5]
TOPPERS/ASP3 Kernel Release 3.5.0 for STM32N6570-DK (ARM Cortex-M55) (Jun 28 2026, ...)
Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
                            Toyohashi Univ. of Technology, JAPAN
Copyright (C) 2004-2019 by Embedded and Real-Time Systems Laboratory
            Graduate School of Information Science, Nagoya Univ., JAPAN

[5] System logging task is started.
[5] N657 sample started.
```

- 先頭の `[5]` は Cortex-M55 向け proc_char（旧ビルドでは `[?]` だった）
- FSBL の `FSBL initialized` 等は **別 UART 経路**（Tera Term には TOPPERS 側だけ見えることもある）

#### GDB で確認した起動順（参考）

```
main → sta_ker → tLogTaskMain_eLogTaskBody_main → main_task
```

### 起動・UART の設計（重要・現行）

| 段階 | 処理 | 備考 |
|------|------|------|
| `main()` | `HAL_Init()` → CKPER(HSI) → `SystemCoreClockUpdate()` → `sta_ker()` | **UART 初期化しない** |
| `sta_ker()` 内 `target_initialize()` | `core_initialize()` → `usart_early_init()` → `tPutLogTarget_initialize()` | **ここで USART1 初期化** |
| LogTask | `cSerialPort_open()` → syslog 出力 | tUsart は **ポーリング TX のみ** |

**禁止**: `main()` 内の `MX_USART1_UART_Init()`（`sta_ker` 前）。NS VTOR が TOPPERS ベクタ（`0x24100400`）のとき割込みが `core_int_entry` → 例外で落ちる。

**UART 実装の要点**（`target_kernel_impl.c` + `tUsart.c`）:

- HAL で USART1 を **TX 専用・115200 8N1** 初期化（CLKP/HSI、`hal_msp.c` で PE5/PE6）
- 初期化後 **RX・エラー・TX 割込みを CR1/CR3 で無効化**（syslog はポーリング送信）
- `eSIOPort_putChar` は TXE/TC を待ってから 1 文字送信
- `eSIOPort_enableCBR` は no-op（シリアルドライバの TX/RX 割込みコールバックを使わない）
- `eSIOPort_open` は UE 済みなら再初期化しない（LogTask の 2 回目 open で壊さない）

**文字化けの原因（解消済み）**: HAL UART と tUsart レジスタアクセスの混在、および送信バッファ満杯後の **TX 割込み経路** への切り替え。上記ポーリング TX 統一で解消。

**試行して却下したもの**: `prep_nkernel_vectors()` / `restore_toppers_irq_vectors()`（`sta_ker` 後の `target_initialize` で `main_task` 未到達・LED 不点滅の原因になったため **現行では未使用**）。

### 起動構成（重要）

STM32N6570-DK は TrustZone + FSBL 前提です。

1. **FSBL**（シリアルに `FSBL initialized` 等）→ Secure アプリへジャンプ（例: `0x3408756d`）
2. **Secure**（ボード内蔵 AppliSecure）→ NS ベクタ `0x24100400` を読んで NS へジャンプ
3. **asp.elf**（TOPPERS/ASP）→ `Reset_Handler` → `main()` → `sta_ker()`

Tera Term の FSBL メッセージは **正常** です。`asp.elf` のリンク先 `0x24100400` は Cube の `VTOR_TABLE_NS_START_ADDR`（`SRAM2_AXI_BASE_NS + 0x400`）と一致します。

`monitor reset halt` → `load` → `continue` だけだと、FSBL/Secure は動くが **NS に入る前に Secure 側で止まっている**、または **`load` 後に再度 reset して NS イメージを消している** 場合、`sta_ker` ブレークに到達しません。

### 準備
  WSL: GDB 起動に必要
  ```
  % sudo apt install libncursesw5
  ```

### OpenOCD 起動
 ```
cd <your_path> \asp3_stm32n6570-dk\obj_n657
"C:\ST\STM32CubeIDE_2.1.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.openocd.win32_2.4.400.202601091506\tools\bin\openocd.exe" -s c:\ST\STM32CubeIDE_2.1.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.debug.openocd_2.3.300.202602021527\resources\openocd\st_scripts  -f interface/stlink-dap.cfg -f target/stm32n6x.cfg
 ```

### gdb 起動（推奨手順）

**TrustZone 注意**: NS 領域 (`0x241......`) の `break`（ソフトウェア BP）は、Secure 側実行中に **メモリへ書き込めず** `Cannot access memory at address 0x24100d99` になることがあります。`Reset_Handler` には **`break` を置かない**（または 1 回止めたら `delete 1`）。`main` / `sta_ker` のみ使うのが安全です。

**方法 A: フル起動チェーン**（FSBL → Secure → NS）

```
cd <your_path>\asp3_stm32n6570-dk\obj_n657
C:\cygwin64\home\hiro2\arm-gcc\gcc-arm-none-eabi-10.3-2021.10\bin\arm-none-eabi-gdb.exe asp.elf
target remote localhost:3333
monitor reset halt
load
break main
break sta_ker
break main_task
break tLogTaskMain_eLogTaskBody_main
continue
```

- 1 回目で `Reset_Handler` に止めたい場合だけ `hbreak Reset_Handler`（ハードウェア BP）を使い、止まったら **`delete 1`** してから `continue`
- `break Reset_Handler` で止まったあと再 `continue` すると上記エラーになりやすい

**`Cannot access memory at address 0x24100d99` が出たとき**

```
delete breakpoints
break main
break sta_ker
continue
```

NS の `Reset_Handler` まで到達済みなら、あとは `main` → `sta_ker` で止まるはずです。

**方法 B: FSBL 完了後に NS を手動起動**（Secure が NS に入らないとき）

FSBL ログが出たあと GDB で `Ctrl-C` してから:

```
load
set $msp = *(unsigned int*)0x24100400
set $pc  = *(unsigned int*)0x24100404
break main
break sta_ker
continue
```

※ TrustZone のため、方法 B が効かない場合は方法 A を使うか、CubeIDE 同梱の Secure+NS デバッグ構成を参照してください。

**TECS シンボル名**: `eLogTaskBody_main` ではなく `tLogTaskMain_eLogTaskBody_main` を使う。

### OpenOCD 接続断（`Fail reading CTRL/STAT register`）

`main` で一度止まったあと `continue` すると、ST-Link/OpenOCD がターゲットと切れて次のメッセージが出ることがあります。

```
Fail reading CTRL/STAT register. Force reconnect
Program stopped.
```

これは **asp.elf の不具合というより SWD デバッグリンクの切断** です。`main` 到達まで行けているので NS 起動自体は成功しています。

**GDB 側の復旧**

```
disconnect
target remote localhost:3333
monitor reset halt
load
```

**再デバッグのコツ**

- `main` で止めてから長く `continue` せず、**`sta_ker` だけ**ブレークして一気に走らせる:

```
break sta_ker
break HardFault_Handler
continue
```

- `main` の中を追うときは `continue` より **`next` / `step`**（数行ずつ）
- 切断が頻発する場合は OpenOCD 起動前に SWD クロックを下げる（`openocd.cfg` またはコマンドライン）:

```
adapter speed 1000
```

（`interface/stlink-dap.cfg` の前に `-f` で小さな cfg を追加するか、OpenOCD コンソールで `adapter speed 1000`）

**GDB なしで動作確認**（syslog / LED）

1. 上記 `load` まで実施
2. `continue` して GDB から離れる（または OpenOCD だけ残してリセット）
3. Tera Term 115200 で TOPPERS バナー・LED 点滅を確認（期待出力は上記 **動作確認済み** 節）

### `_kernel_target_exit` で止まる / SIGINT でそこにいる

接続直後や `main` の `continue` 後に

```
_kernel_target_exit () at target_kernel_impl.c:161
161             while(1);
```

となるのは **カーネルが異常終了して `target_exit()` に入った** 状態です（`while(1)` で永久ループ）。

主な呼び出し元:

- `_kernel_default_exc_handler` … 未登録例外（HardFault 等）
- `_kernel_default_int_handler` … 未登録割込み
- `exit_kernel` … `ext_ker()` による正常終了（サンプルでは通常来ない）

`Program received signal SIGINT` は **Ctrl+C で止めた** 場合が多く、止まった時点で既に `target_exit` のループに入っていた、という意味です。`sta_ker` ブレークが出なくても、**`main` 通過後にカーネル起動中に落ちている**可能性があります。

**切り分け用ブレークポイント**

```
delete breakpoints
break sta_ker
break _kernel_default_exc_handler
break _kernel_default_int_handler
break HardFault_Handler
continue
```

- `_kernel_default_*` で止まったら `backtrace` / `info registers`、Tera Term に `Unregistered Exception` / `Unregistered Interrupt` が出ていないか確認
- `sta_ker` だけ止まって `continue` 後に `_kernel_default_*` → 初期化ルーチンか SysTick/タイマ周りの未登録割込みを疑う

**`main` から段階的に追う**（2 回目 `cont` で一気に走らせない）:

```
delete breakpoints
break sta_ker
continue
# main で止まったら
delete 1
next
next
next
next
next
# sta_ker 直前まで next。step で sta_ker に入る
step
```

**`target_exit` に入った直後**（次回同じ状態になったら Ctrl+C 前に）:

```
bt
info registers
x/4i $pc
```

### `_kernel_default_exc_handler` で止まったとき（最優先）

未登録 **CPU 例外**（HardFault / UsageFault / SecureFault 等）です。`sta_ker` より先にここで止まることもあります（VTOR が TOPPERS 側に切り替わった後の例外）。

**その場で実行**（`p_excinf=0x0` でも `$ipsr` / `$lr` は有効）:

```
print/x $ipsr
print/x $lr
print/x $sp
print/x $msp
print/x $psp
backtrace
info registers
print/x *(uint32_t*)0xE000ED28
print/x *(uint32_t*)0xE000ED2c
print/x *(uint32_t*)0xE000ED30
print/x *(uint32_t*)0xE000ED08
```

`$ipsr` の下位 8bit（例外番号）の目安:

| 番号 | 意味 |
|------|------|
| 3 | HardFault |
| 4 | MemManage |
| 5 | BusFault |
| 6 | UsageFault |
| 7 | SecureFault（TrustZone） |
| 11 | SVCall |
| 15 | SysTick（通常は `_kernel_default_int_handler` 側） |

`backtrace` に `sta_ker` / `initialize_tecs` / `target_initialize` / `core_initialize` が見えれば、カーネル初期化中の例外です。

**GDB の注意**: `delete breakpoints` と聞かれたら **`y` だけ** Enter。`break sta_ker` などと同じ行に書かない。

**`main` 内 UART 起因の症状**（`MX_USART1_UART_Init` を `sta_ker` 前に呼んだ場合）:

- `backtrace` に `MX_USART1_UART_Init` / `main`、#1 が `_kernel_core_int_entry`
- `sta_ker` BP に到達しない、`main_task` 未到達、LED 不点滅
- **対策**: `main_kernel.c` から UART 初期化を削除（現行どおり）

**`_kernel_default_exc_handler` その他**（上記 `main` UART 以外）: 上記「その場で実行」の `$ipsr` / `backtrace` で原因を特定。

**再ビルド**（Cygwin）:

```bash
cd obj_n657 && make
```

**修正後の切り分け GDB**:

```
monitor reset halt
load
delete breakpoints
y
break sta_ker
break main_task
continue
```

### 旧手順（参考・問題あり）
```
monitor reset halt
load
break sta_ker
continue
```
`load` のあと `monitor reset halt` を繰り返すと RAM/Flash 上の NS イメージが上書きされ、ブレークしない原因になります。

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

### 関連ソース（syslog / UART 現行）

| ファイル | 役割 |
|----------|------|
| `target/stm32n6570_dk/main_kernel.c` | HAL + CKPER → `sta_ker()`（UART なし） |
| `target/stm32n6570_dk/target_kernel_impl.c` | `usart_early_init()`、`target_initialize()` |
| `arch/arm_m_gcc/stm32n6xx_stm32cube/tUsart.c` | ポーリング TX、`eSIOPort_open` |
| `syssvc/tSysLog.c` | M55 向け proc_char `'5'` |
| `target/.../stm32n6xx_hal_msp.c` | USART1 PE5/PE6、CLKP |
| `sample/sample1_n657.cdl` | LogTask / syslog / Banner |

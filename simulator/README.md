# RasPi4 AUTOSAR Simulator

Raspberry Pi 4 が手元になくても、**PC 上（Linux / macOS / Windows）でそのまま動作確認できる**ソフトウェアシミュレータです。

クロスコンパイラは不要で、通常の `gcc` だけでビルド・実行できます。

---

## 概要

| 項目 | 実機 (RasPi4) | シミュレータ |
|------|--------------|-------------|
| コンパイラ | `arm-none-eabi-gcc` | `gcc` (ネイティブ) |
| 出力先 | HDMI / UART | 標準出力 (ターミナル) |
| GPIO/LED | 実 GPIO ピン | コンソールメッセージ |
| タスクスケジューラ | Trampoline RTOS | 時刻ベースの簡易スケジューラ |
| タイマー精度 | ハードウェアタイマー | POSIX `clock_gettime` / Win32 `GetTickCount` |

シミュレータが起動すると、`app/app.oil` に定義されたアラーム周期どおりに各 AUTOSAR タスクが呼ばれます。

```
Task100ms   →  100 ms ごと
TaskBlink   →  500 ms ごと
TaskSerial  → 1000 ms ごと
Task1000ms  → 1000 ms ごと
TaskProcess → UART 受信割り込みで起動（シミュレータでは動作しない）
```

---

## ディレクトリ構成

```
simulator/
├── build_sim.sh      Linux / macOS ビルドスクリプト
├── build_sim.bat     Windows (MinGW) ビルドスクリプト
├── sim_drivers.c     ハードウェア抽象化レイヤー (UART/GPIO/FB → stdout)
├── sim_os.c          AUTOSAR OS API + 時刻ベースタスクスケジューラ
└── README.md         このファイル
```

ビルドに使用するアプリケーションソースは `app/` ディレクトリのものをそのまま参照します。  
`app/main.c` と `app/tpl_os.h` は変更不要です。

---

## ビルドと実行

### Linux / macOS

```bash
cd simulator
bash build_sim.sh        # ビルドのみ
bash build_sim.sh run    # ビルド＆実行 (10 秒間シミュレート)
```

### Windows (MinGW)

MinGW-w64 をインストールし、`bin` ディレクトリを PATH に追加してください。  
（MSYS2 の場合: `pacman -S mingw-w64-x86_64-gcc`）

```bat
cd simulator
build_sim.bat            :: ビルドのみ
build_sim.bat run        :: ビルド＆実行 (10 秒間シミュレート)
```

### 手動でビルド（任意のシステム）

```bash
gcc -O2 -Wall -DSIMULATOR -DSIM_DURATION_SEC=10 \
    -I../app \
    ../app/main.c sim_os.c sim_drivers.c \
    -o sim_app
./sim_app
```

---

## 期待される出力例

```
[SIM] Framebuffer initialized (1024x768 @ 32bpp)
[SIM] GPIO initialized (LED on GPIO17)

================================================
 Trampoline AUTOSAR OS on Raspberry Pi 4
 HDMI Display Test
================================================
 Build: Mar  1 2026 00:00:00
================================================

[SIM] AUTOSAR task scheduler started
[SIM] Press Ctrl+C to stop the simulation

[100ms Task] Counter: 1
[100ms Task] Counter: 2
[SIM] LED ON
[100ms Task] Counter: 3
...
[100ms Task] Counter: 10
[TaskSerial] Count: 1 | Uptime: 1 sec
[1000ms Task] Counter: 1 | 100ms Counter: 10

[SIM] Simulation finished (10 seconds)
System Shutdown. Error code: 0x00000000
```

---

## シミュレーション時間のカスタマイズ

デフォルトは **10 秒**で終了します。  
時間を変更するには `-DSIM_DURATION_SEC=<秒数>` を指定します。

```bash
# 30 秒間シミュレート
gcc -O2 -Wall -DSIMULATOR -DSIM_DURATION_SEC=30 \
    -I../app ../app/main.c sim_os.c sim_drivers.c -o sim_app

# 無制限（Ctrl+C で停止）
gcc -O2 -Wall -DSIMULATOR \
    -I../app ../app/main.c sim_os.c sim_drivers.c -o sim_app
```

---

## QEMU による実機エミュレーション

PC ネイティブシミュレータに加え、QEMU でより実機に近い環境を再現することも可能です。

### 必要なもの

```bash
# Ubuntu / Debian
sudo apt-get install qemu-system-arm gcc-arm-none-eabi binutils-arm-none-eabi
```

### ビルドと実行

```bash
# クロスコンパイル（ARM バイナリ）
cd build
bash build.sh

# QEMU で Raspberry Pi 2 (Cortex-A15, ARMv7) として実行
# ※ RasPi4 (BCM2711) の完全エミュレーションは QEMU では未対応のため
#    raspi2 マシンタイプを使用してください
qemu-system-arm \
    -M raspi2 \
    -kernel build/output/kernel.img \
    -serial stdio \
    -display none
```

> **注意:** QEMU の `raspi2` マシンは Raspberry Pi 2 (BCM2836, Cortex-A7) をエミュレートします。  
> Raspberry Pi 4 (BCM2711) のフル対応は QEMU には含まれていないため、  
> ハードウェアレジスタへの直接アクセスは動作しない場合があります。  
> ペリフェラルに依存しない動作確認には PC ネイティブシミュレータを推奨します。

---

## ファイル構成の詳細

### `sim_drivers.c`

`app/uart_comm.c` と `app/framebuffer.c` の代替実装。  
ハードウェアレジスタアクセスを標準入出力に置き換えます。

| 関数 | 実機の動作 | シミュレータの動作 |
|------|-----------|-----------------|
| `uart_putc(c)` | UART0 DR レジスタに書き込み | `putchar(c)` |
| `uart_puts(s)` | UART0 経由で文字列送信 | `fputs(s, stdout)` |
| `gpio_led_toggle()` | GPIO17 を ON/OFF | `printf("[SIM] LED ON/OFF\n")` |
| `fb_puts(s)` | HDMI フレームバッファに文字描画 | `fputs(s, stdout)` |
| `fb_set_cursor(x, y)` | フレームバッファのカーソル移動 | ANSI エスケープシーケンス |
| `delay_ms(ms)` | ビジーウェイト | 何もしない（スケジューラが時間管理） |

### `sim_os.c`

`app/tpl_os_stubs.c` の代替実装。  
`StartOS()` がタスクスケジューラのメインループになっており、  
`clock_gettime()` (Linux/macOS) / `GetTickCount()` (Windows) で経過時間を計測しながら  
各タスク関数を適切な周期で呼び出します。

---

## トラブルシューティング

**`gcc: command not found`**  
→ `sudo apt-get install gcc` (Ubuntu) または `brew install gcc` (macOS) でインストール

**`clock_gettime` が見つからない**  
→ `-lrt` リンクオプションを追加してください（古い Linux の場合）  
```bash
gcc ... -lrt -o sim_app
```

**ANSI カラーが表示されない**  
→ Windows のコマンドプロンプトでは ANSI コードが無効な場合があります。  
  Windows Terminal または PowerShell を使用してください。

# Raspberry Pi 4 で動作する Trampoline AUTOSAR OS

[English](README.md) | [日本語](README.ja.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%204-red.svg)](https://www.raspberrypi.org/)
[![AUTOSAR](https://img.shields.io/badge/AUTOSAR-Compliant-blue.svg)](https://www.autosar.org/)

Raspberry Pi 4 上で動作する [Trampoline RTOS](https://github.com/TrampolineRTOS/trampoline)（AUTOSAR OS準拠）のベアメタル実装です。HDMI ディスプレイ出力と UART シリアル通信をサポートしています。

![Demo](docs/images/demo-screenshot.png)
*スクリーンショット: Raspberry Pi 4 で動作する Trampoline OS の HDMI 出力*

## ✨ 特徴

- 🎯 **AUTOSAR OS 準拠**: Trampoline RTOS を OIL 設定で使用
- 🖥️ **デュアル出力モード**: HDMI フレームバッファまたは UART シリアル通信
- 🔧 **ベアメタル実装**: Linux なし、直接ハードウェア制御
- 📝 **完全な ASCII フォント**: テキスト表示用の 8x16 ビットマップフォント (0x20-0x7E)
- 🛠️ **簡単なデプロイ**: 自動 SD カード書き込みツール付属
- 🔄 **リアルタイムマルチタスキング**: AUTOSAR スケジューリングによる周期タスク

## 🎥 デモ

実行すると以下が表示されます:
- HDMI ディスプレイまたはシリアルターミナル上のシステムバナー
- 1秒ごとの周期タスク出力
- リアルタイムメッセージ処理
- GPIO での LED アクティビティ

## 📋 必要なもの

### ハードウェア
- **Raspberry Pi 4** (BCM2711, Cortex-A72)
- **microSD カード** (8GB 以上、FAT32 フォーマット)
- **HDMI ディスプレイ** (HDMI 出力モード用) または **USB-シリアル変換ケーブル** (UART モード用)
- **5V 2.5A 以上の電源**

### ソフトウェア
- **ARM GNU Toolchain** (arm-none-eabi-gcc)
- **Python 3.8+** (PC ツール用)
- **Git**

## 🚀 クイックスタート

5分でセットアップできる [QUICKSTART.md](QUICKSTART.md) をご覧ください。

### 1. ARM Toolchain のインストール

**Windows:**
```powershell
# ダウンロード: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
# arm-gnu-toolchain-<version>-mingw-w64-i686-arm-none-eabi.exe をインストール
# インストール確認:
arm-none-eabi-gcc --version
```

**Linux/WSL:**
```bash
sudo apt-get update
sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi
```

### 2. クローンとセットアップ

```bash
git clone https://github.com/YOUR_USERNAME/trampoline-rpi4.git
cd trampoline-rpi4

# Trampoline RTOS をクローン
git clone https://github.com/TrampolineRTOS/trampoline.git ../trampoline

# Python 依存関係をインストール
cd pc_tools
pip install -r requirements.txt
```

### 3. ビルド

**Windows:**
```powershell
cd build
.\build.bat
```

**Linux:**
```bash
cd build
bash build.sh
```

### 4. SD カードへのデプロイ

```bash
# 自動デプロイ（Windows）
cd pc_tools
python sd_writer.py --list            # 利用可能なドライブを一覧表示
python sd_writer.py --drive E: --hdmi # SD カードに書き込み

# 手動デプロイ
# 1. SD カードを FAT32 でフォーマット
# 2. ファームウェアファイルをコピー（docs/setup_guide.md 参照）
# 3. build/output/kernel.img を SD カードにコピー
# 4. config.txt を作成（QUICKSTART.md 参照）
```

### 5. 起動とテスト

1. SD カードを Raspberry Pi 4 に挿入
2. HDMI ディスプレイ（HDMI モード用）または USB-シリアル（UART モード用）を接続
3. Raspberry Pi の電源を入れる
4. UART モードの場合、実行: `python pc_tools/serial_monitor.py --port COM3 --baud 115200`

期待される出力:
```
================================================
 Trampoline AUTOSAR OS on Raspberry Pi 4
 HDMI Display Test
================================================

[TaskSerial] Count: 1 | Uptime: 1 sec
[TaskSerial] Count: 2 | Uptime: 2 sec
```

## 📁 プロジェクト構成

```
trampoline-rpi4/
├── README.md                      # このファイル
├── README.ja.md                   # 日本語版 README
├── QUICKSTART.md                  # 5分セットアップガイド
├── LICENSE                        # MIT ライセンス
├── .gitignore                     # Git 除外ルール
├── app/                           # アプリケーションソース
│   ├── app.oil                    # AUTOSAR OIL 設定
│   ├── main.c                     # メインアプリケーション
│   ├── boot.S                     # ARM ブートコード
│   ├── framebuffer.c/h            # HDMI フレームバッファドライバ
│   ├── uart_comm.c/h              # UART シリアルドライバ
│   └── tpl_os_stubs.c             # Trampoline OS スタブ
├── build/                         # ビルドスクリプト
│   ├── build.bat                  # Windows ビルドスクリプト
│   ├── build.sh                   # Linux ビルドスクリプト
│   ├── link.ld                    # リンカスクリプト
│   └── output/                    # ビルド出力（除外）
├── firmware/                      # Raspberry Pi ファームウェア（除外）
│   ├── boot/                      # ブートファイル
│   └── README.md                  # ファームウェア説明
├── pc_tools/                      # PC ユーティリティ
│   ├── serial_monitor.py          # シリアル通信ツール
│   ├── sd_writer.py               # SD カードデプロイツール
│   ├── requirements.txt           # Python 依存関係
│   └── README.md                  # ツール説明
└── docs/                          # ドキュメント
    ├── setup_guide.md             # 詳細セットアップガイド
    ├── architecture.md            # システムアーキテクチャ
    └── hdmi_output.md             # HDMI 実装詳細
```

## 🎮 使用方法

### 出力モード

システムは2つの出力モードをサポートしています（`app/main.c` で設定）:

**HDMI モード（デフォルト）:**
```c
#define USE_HDMI  1  // HDMI ディスプレイ出力の場合は 1 に設定
```
- HDMI ディスプレイ上のテキストコンソール
- 1024x768 解像度、32 ビットカラー
- 8x16 ビットマップフォントレンダリング
- 追加ハードウェア不要

**UART モード:**
```c
#define USE_HDMI  0  // UART シリアル出力の場合は 0 に設定
```
- GPIO14/GPIO15 経由のシリアル通信
- 115200 baud
- USB-シリアル変換ケーブルが必要
- PC 上のインタラクティブターミナル

### PC ツール

**シリアルモニター** (`pc_tools/serial_monitor.py`):
```bash
# 利用可能なポートを一覧表示
python serial_monitor.py --list

# Raspberry Pi に接続
python serial_monitor.py --port COM3 --baud 115200
```

**SD カードライター** (`pc_tools/sd_writer.py`):
```bash
# 利用可能なドライブを一覧表示
python sd_writer.py --list

# SD カードに書き込み（HDMI モード）
python sd_writer.py --drive E: --hdmi

# SD カードに書き込み（UART モード）
python sd_writer.py --drive E: --uart

# ドライラン（テストのみ、書き込まない）
python sd_writer.py --drive E: --hdmi --dry-run
```

詳細なツールドキュメントは [pc_tools/README.md](pc_tools/README.md) をご覧ください。

## 🛠️ 設定

### AUTOSAR OIL 設定

システム設定は `app/app.oil` で定義されています:

```oil
CPU rpi4 {
  OS config {
    STATUS = EXTENDED;
    BUILD = TRUE {
      TRAMPOLINE_BASE_PATH = "../../trampoline";
      APP_SRC = "main.c";
      APP_NAME = "rpi4_app.elf";
      CFLAGS = "-O2 -Wall";
      LDFLAGS = "-T ../build/link.ld";
    };
  };
  
  TASK TaskSerial {
    PRIORITY = 10;
    ACTIVATION = 1;
    SCHEDULE = FULL;
  };
  
  ALARM AlarmSerial {
    COUNTER = SystemCounter;
    ACTION = ACTIVATETASK { TASK = TaskSerial; };
    AUTOSTART = TRUE { ALARMTIME = 100; CYCLETIME = 100; };
  };
};
```

### ハードウェア設定

**UART ピン (GPIO):**
- GPIO14 (TXD) → USB-シリアル RX
- GPIO15 (RXD) → USB-シリアル TX
- GND → USB-シリアル GND

**HDMI 出力:**
- フレームバッファ用のメールボックスプロパティインターフェース
- 1024x768 @ 32bpp
- 直接ピクセル操作

## 📚 ドキュメント

- [クイックスタートガイド](QUICKSTART.md) - 5分で開始
- [セットアップガイド](docs/setup_guide.md) - 詳細なインストール手順
- [アーキテクチャ](docs/architecture.md) - システム設計とコンポーネント
- [HDMI 出力](docs/hdmi_output.md) - フレームバッファ実装詳細
- [PC ツール](pc_tools/README.md) - シリアルモニターと SD ライターの使用方法

## 🐛 トラブルシューティング

### ビルドエラー

**エラー: `arm-none-eabi-gcc: command not found`**
- ARM toolchain がインストールされているか確認
- PATH に追加: `export PATH=$PATH:/path/to/arm-toolchain/bin`

**エラー: `Cannot find Trampoline source`**
- Trampoline をクローン: `git clone https://github.com/TrampolineRTOS/trampoline.git ../trampoline`
- `build.bat` または `build.sh` のパスを確認

### 起動の問題

**Raspberry Pi が起動しない:**
- SD カードに必要なファイルがすべて揃っているか確認 (bootcode.bin, start4.elf, fixup4.dat, config.txt, kernel.img)
- config.txt の設定を確認
- SD カードが FAT32 でフォーマットされているか確認
- 5V 2.5A 以上の電源を確認

**HDMI 出力がない:**
- config.txt でセーフ HDMI モードを試す: `hdmi_safe=1`
- HDMI ケーブルとディスプレイを確認
- モニターが信号を自動検出するはず

### シリアル通信

**シリアルポートが開けない:**
- ポート名を確認 (Windows: `COM*`, Linux: `/dev/ttyUSB*`)
- ドライバのインストールを確認 (Windows のデバイスマネージャー)
- Linux: ユーザーを dialout グループに追加: `sudo usermod -a -G dialout $USER`

**文字化けする:**
- ボーレートを確認: 115200
- TX/RX のクロスオーバーを確認 (Pi TX → Serial RX, Pi RX → Serial TX)
- GND 接続を確認

## 🤝 貢献

貢献を歓迎します！プルリクエストをお気軽に送信してください。大きな変更の場合:

1. リポジトリをフォーク
2. フィーチャーブランチを作成 (`git checkout -b feature/amazing-feature`)
3. 変更をコミット (`git commit -m 'Add amazing feature'`)
4. ブランチにプッシュ (`git push origin feature/amazing-feature`)
5. プルリクエストを開く

## 📄 ライセンス

このプロジェクトは MIT ライセンスの下でライセンスされています - 詳細は [LICENSE](LICENSE) ファイルを参照してください。

**注意:** このプロジェクトは [Trampoline RTOS](https://github.com/TrampolineRTOS/trampoline) を使用しており、独自のライセンスの下で配布されています。ライセンス条項については Trampoline プロジェクトを参照してください。

## 🙏 謝辞

- [Trampoline RTOS](https://github.com/TrampolineRTOS/trampoline) - AUTOSAR OS 実装
- [Raspberry Pi Foundation](https://www.raspberrypi.org/) - ハードウェアプラットフォーム
- [ARM](https://www.arm.com/) - プロセッサアーキテクチャとツールチェーン
- [AUTOSAR](https://www.autosar.org/) - 車載ソフトウェア標準

## 📬 お問い合わせ

質問、問題、提案については、GitHub で issue を開いてください。

---

**このリポジトリが役立つと思ったら、スター ⭐ をつけてください！**

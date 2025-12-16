# Quick Start Guide

Get Trampoline AUTOSAR OS running on Raspberry Pi 4 in under 20 minutes!

## Prerequisites

- **Raspberry Pi 4** (BCM2711, Cortex-A72)
- **microSD card** (8GB+, FAT32 formatted)
- **HDMI display** or **USB-Serial adapter**
- **ARM GNU Toolchain** (arm-none-eabi-gcc)
- **Python 3.8+**

## Step 1: Install Toolchain (5 min)

**Windows:**
```powershell
# Download from: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
# Install: arm-gnu-toolchain-<version>-mingw-w64-i686-arm-none-eabi.exe
# Verify:
arm-none-eabi-gcc --version
```

**Linux/WSL:**
```bash
sudo apt-get update
sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi
```

## Step 2: Clone and Setup (3 min)

```bash
# Clone this repository
git clone https://github.com/YOUR_USERNAME/trampoline-rpi4.git
cd trampoline-rpi4

# Clone Trampoline RTOS
git clone https://github.com/TrampolineRTOS/trampoline.git ../trampoline

# Clone Raspberry Pi firmware
git clone --depth=1 https://github.com/raspberrypi/firmware.git

# Install Python tools
cd pc_tools
pip install -r requirements.txt
cd ..
```

## Step 3: Build (2 min)

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

Expected output:
```
Building for Raspberry Pi 4...
Compiling boot.S...
Compiling main.c...
Compiling framebuffer.c...
Compiling uart_comm.c...
Linking kernel.elf...
Creating kernel.img...
Build complete: build/output/kernel.img
```

## Step 4: Deploy to SD Card (5 min)

### Option A: Automated (Recommended)

```bash
# Windows
cd pc_tools
python sd_writer.py --list              # List available drives
python sd_writer.py --drive E: --hdmi   # Write for HDMI mode
# or
python sd_writer.py --drive E: --uart   # Write for UART mode
```

### Option B: Manual

1. **Format SD card** as FAT32

2. **Copy firmware files** to SD card root:
   ```
   firmware/boot/bootcode.bin  → sdcard:/
   firmware/boot/start4.elf    → sdcard:/
   firmware/boot/fixup4.dat    → sdcard:/
   ```

3. **Copy kernel** to SD card:
   ```
   build/output/kernel.img     → sdcard:/
   ```

4. **Create config.txt** on SD card root:
   
   **For HDMI mode:**
   ```ini
   # Raspberry Pi 4 Configuration
   arm_64bit=0
   kernel=kernel.img
   kernel_address=0x8000
   
   # HDMI settings
   hdmi_safe=1
   hdmi_force_hotplug=1
   gpu_mem=64
   ```
   
   **For UART mode (add to above):**
   ```ini
   # UART settings
   enable_uart=1
   dtoverlay=disable-bt
   ```

## Step 5: Connect Hardware (3 min)

### HDMI Mode
1. Connect HDMI cable to Raspberry Pi and display
2. Insert SD card into Raspberry Pi
3. Power on Raspberry Pi
4. Watch the display for output

### UART Mode
1. **Wire connections:**
   - RPi GPIO14 (Pin 8)  → USB-Serial RX
   - RPi GPIO15 (Pin 10) → USB-Serial TX
   - RPi GND (Pin 6)     → USB-Serial GND

2. **Insert SD card** into Raspberry Pi

3. **Connect USB-Serial** to PC

4. **Start serial monitor:**
   ```bash
   cd pc_tools
   python serial_monitor.py --list              # Find your port
   python serial_monitor.py --port COM3 --baud 115200
   # Linux: --port /dev/ttyUSB0
   ```

5. **Power on** Raspberry Pi

## Expected Output

### HDMI Mode
You should see on the display:
```
================================================
 Trampoline AUTOSAR OS on Raspberry Pi 4
 HDMI Display Test
================================================

[TaskSerial] Count: 1 | Uptime: 1 sec
[TaskSerial] Count: 2 | Uptime: 2 sec
[TaskSerial] Count: 3 | Uptime: 3 sec
```

### UART Mode
You should see in the terminal:
```
================================================
 Trampoline AUTOSAR OS on Raspberry Pi 4
 Serial Communication Test
================================================

[TaskSerial] Count: 1 | Uptime: 1 sec
[TaskSerial] Count: 2 | Uptime: 2 sec
```

Type messages and press Enter to send to Raspberry Pi. You'll see echoed responses.

## 🎉 Success!

You now have Trampoline AUTOSAR OS running on Raspberry Pi 4!

## Next Steps

- Read [docs/setup_guide.md](docs/setup_guide.md) for detailed information
- Modify tasks in `app/main.c`
- Configure system in `app/app.oil`
- Check [docs/architecture.md](docs/architecture.md) to understand the system

## Troubleshooting

**No output on HDMI:**
- Try another HDMI cable/display
- Check `config.txt` has `hdmi_force_hotplug=1`
- Verify ACT LED blinks (SD card access)

**No serial output:**
- Check TX/RX crossover (Pi TX → Serial RX)
- Verify baud rate: 115200
- Check GND connection
- Try: `python serial_monitor.py --list` to find correct port

**Build fails:**
- Verify ARM toolchain: `arm-none-eabi-gcc --version`
- Check Trampoline path in `build.bat` or `build.sh`
- Ensure all dependencies are installed

**Raspberry Pi won't boot:**
- Verify all firmware files are on SD card
- Check SD card is FAT32 formatted
- Ensure config.txt is correct
- Use 5V 2.5A+ power supply

For more help, see [README.md](README.md) or open an issue on GitHub.

---

**Enjoy your AUTOSAR OS on Raspberry Pi! ⭐**

## よくある問題と解決

| 問題 | 解決策 |
|------|--------|
| ビルドエラー | arm-none-eabi-gccがPATHに追加されているか確認 |
| 何も表示されない | シリアルポート名、TX/RX配線を確認 |
| 文字化け | ボーレート115200bpsを確認 |
| Raspberry Pi起動しない | bootcode.bin, start.elf, config.txtを確認 |

詳細は [docs/setup_guide.md](docs/setup_guide.md) を参照してください。

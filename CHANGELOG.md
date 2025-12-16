# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-01-XX

### Added
- Initial release
- Bare-metal Trampoline RTOS implementation for Raspberry Pi 4
- HDMI framebuffer output support (1024x768 @ 32bpp)
- UART serial communication support (115200 baud)
- Complete ASCII font rendering (0x20-0x7E)
- PC tools for serial communication and SD card deployment
- AUTOSAR OIL configuration support
- Multi-tasking with periodic tasks
- Automated SD card writer tool (`sd_writer.py`)
- Serial monitor tool (`serial_monitor.py`)
- Comprehensive documentation
- Build scripts for Windows and Linux
- ARM Cortex-A72 support (Raspberry Pi 4)

### Features
- Dual output modes: HDMI display or UART serial
- Real-time task scheduling via AUTOSAR OS
- Direct hardware access (bare-metal)
- 8x16 bitmap font for text rendering
- Mailbox property interface for GPU communication
- GPIO control for peripherals

### Documentation
- Quick start guide (5-minute setup)
- Detailed setup guide
- Architecture documentation
- HDMI output implementation details
- PC tools usage guide
- Troubleshooting guide
- Contributing guidelines

## [Unreleased]

### Planned Features
- Multi-core support (utilize all 4 Cortex-A72 cores)
- I2C driver implementation
- SPI driver implementation
- PWM support for LED/servo control
- DMA transfers
- CAN bus communication
- Additional Raspberry Pi model support (Pi 3, Pi Zero)
- Performance benchmarks
- Unit tests
- CI/CD pipeline

---

For upgrade instructions and migration guides, see [docs/setup_guide.md](docs/setup_guide.md).

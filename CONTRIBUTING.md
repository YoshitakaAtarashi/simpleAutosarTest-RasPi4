# Contributing to Trampoline AUTOSAR OS on Raspberry Pi 4

Thank you for your interest in contributing! We welcome contributions from the community.

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue with:
- Clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- Your environment (OS, toolchain version, Raspberry Pi model)
- Relevant logs or error messages

### Suggesting Enhancements

Enhancement suggestions are welcome! Please:
- Check if the feature has already been requested
- Clearly describe the feature and its benefits
- Provide examples of how it would be used

### Pull Requests

1. **Fork the repository**
   ```bash
   git clone https://github.com/YOUR_USERNAME/trampoline-rpi4.git
   cd trampoline-rpi4
   ```

2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make your changes**
   - Write clear, commented code
   - Follow existing code style
   - Test your changes thoroughly

4. **Commit your changes**
   ```bash
   git add .
   git commit -m "Add: Brief description of changes"
   ```
   
   Commit message format:
   - `Add:` for new features
   - `Fix:` for bug fixes
   - `Update:` for updates to existing features
   - `Docs:` for documentation changes
   - `Refactor:` for code refactoring

5. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```

6. **Open a Pull Request**
   - Provide a clear description of changes
   - Reference any related issues
   - Include test results if applicable

### Code Style Guidelines

- **C Code:**
  - Use 4 spaces for indentation (no tabs)
  - Follow existing naming conventions
  - Comment complex logic
  - Keep functions focused and readable
  - Use descriptive variable names

- **Python Code:**
  - Follow PEP 8 style guide
  - Use 4 spaces for indentation
  - Add docstrings to functions and classes
  - Use type hints where appropriate

- **Documentation:**
  - Use Markdown for documentation
  - Keep language clear and concise
  - Include code examples where helpful
  - Update relevant docs when changing features

### Testing

Before submitting a PR:
- Build the project successfully
- Test on actual hardware if possible
- Verify both HDMI and UART modes
- Check that existing functionality still works

### Areas for Contribution

We especially welcome contributions in:
- Additional drivers (I2C, SPI, PWM, etc.)
- Performance optimizations
- Documentation improvements
- Testing on different Raspberry Pi models
- Multi-core support
- Additional examples
- Bug fixes

### Questions?

Feel free to open an issue for questions or discussions about potential contributions.

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

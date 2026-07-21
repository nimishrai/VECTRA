# Contributing to VECTRA

Thank you for your interest in contributing to VECTRA! We welcome contributions from the community and appreciate your help in making this project better.

---

## 🤝 How to Contribute

### Reporting Bugs

Before creating bug reports, please check the existing issues to avoid duplicates. When creating a bug report, include:

- **Clear description** of the problem
- **Steps to reproduce** the issue
- **Expected behavior** vs. **actual behavior**
- **Screenshots** if applicable
- **Environment details** (ESP32 board, Arduino IDE version, OS)
- **Additional context** (wiring diagrams, code snippets)

### Suggesting Enhancements

We welcome feature requests! Please:

- Use a clear and descriptive title
- Provide a detailed description of the proposed enhancement
- Explain why this enhancement would be useful
- Consider including mockups or examples if applicable

### Pull Request Process

1. **Fork the repository**
   ```bash
   git clone https://github.com/yourusername/VECTRA.git
   ```

2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make your changes**
   - Follow the coding standards outlined below
   - Add comments for complex logic
   - Update documentation as needed

4. **Commit your changes**
   ```bash
   git commit -m "Add your feature description"
   ```

5. **Push to the branch**
   ```bash
   git push origin feature/your-feature-name
   ```

6. **Create a Pull Request**
   - Provide a clear description of your changes
   - Reference any related issues
   - Include screenshots if applicable

---

## 📝 Coding Standards

### Arduino Code Style

- **Indentation**: 2 spaces (no tabs)
- **Line length**: Maximum 120 characters
- **Naming conventions**:
  - Variables: `camelCase` (e.g., `motorSpeed`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_SPEED`)
  - Functions: `camelCase` (e.g., `readGPSData()`)
  - Classes: `PascalCase` (e.g., `TelemetryManager`)

### Code Organization

- **Header includes**: At the top of the file
- **Constants**: After includes, before setup()
- **Global variables**: After constants, organized by function
- **Functions**: Organized logically (setup, loop, helpers)
- **Comments**: Use `//` for single-line, `/* */` for multi-line

### Example

```cpp
// ================= INCLUDES =================
#include <WiFi.h>
#include <WebServer.h>

// ================= CONSTANTS =================
const int MAX_SPEED = 255;
const int UPDATE_INTERVAL = 1000;

// ================= GLOBAL VARIABLES =================
int currentSpeed = 0;
unsigned long lastUpdate = 0;

// ================= SETUP =================
void setup() {
  // Initialization code
}

// ================= LOOP =================
void loop() {
  // Main loop code
}

// ================= HELPER FUNCTIONS =================
void updateSpeed(int newSpeed) {
  // Function implementation
}
```

---

## 🧪 Testing

### Hardware Testing

Before submitting changes that affect hardware:

1. Test on actual hardware when possible
2. Verify pin assignments match documentation
3. Check for proper voltage levels
4. Test edge cases (min/max values)

### Software Testing

- Test code compiles without errors
- Verify no memory leaks (check free memory)
- Test with different ESP32 board variants
- Verify WiFi functionality if modified

### Documentation Testing

- Ensure all new features are documented
- Update pin configuration if changed
- Add examples for new functions
- Verify links in documentation work

---

## 📚 Documentation Guidelines

### README.md Updates

When adding new features:
- Update the Features section
- Add to the Roadmap if applicable
- Update Getting Started if setup changes
- Add new images to the Gallery section

### Hardware Documentation

When modifying hardware:
- Update HARDWARE.md with new pin assignments
- Add new components to the Component List
- Update wiring diagrams
- Add troubleshooting steps for new issues

### Code Comments

- Add comments for complex algorithms
- Document non-obvious logic
- Explain hardware-specific considerations
- Include units in variable names where appropriate

---

## 🎯 Areas for Contribution

We're particularly interested in contributions for:

### High Priority
- [ ] Mobile app development (Android/iOS)
- [ ] Data logging to SD card
- [ ] Geofencing implementation
- [ ] OTA update functionality

### Medium Priority
- [ ] Additional controller support (Xbox, etc.)
- [ ] Enhanced dashboard features
- [ ] Autonomous navigation modes
- [ ] Computer vision integration

### Low Priority
- [ ] Code optimization
- [ ] Documentation improvements
- [ ] Additional sensor support
- [ ] Custom PCB design files

---

## 📧 Communication

### Questions

For questions about the project:
- Open a GitHub Discussion
- Check existing issues and documentation
- Be specific and provide context

### Security Issues

For security vulnerabilities:
- Do not open a public issue
- Send details to the maintainers privately
- Include steps to reproduce and impact assessment

---

## 🎨 Design Guidelines

### Dashboard Design

When contributing to the web dashboard:
- Maintain the dark theme aesthetic
- Ensure responsive design for mobile devices
- Keep dependencies minimal (no external libraries)
- Test on multiple browsers (Chrome, Firefox, Safari)

### Hardware Design

When proposing hardware changes:
- Consider cost implications
- Ensure availability of components
- Maintain backward compatibility when possible
- Provide clear wiring diagrams

---

## 📋 Pull Request Checklist

Before submitting your PR, ensure:

- [ ] Code follows the project's coding standards
- [ ] Self-reviewed the code changes
- [ ] Commented complex code sections
- [ ] Updated documentation accordingly
- [ ] No new warnings generated
- [ ] Added tests for new features (if applicable)
- [ ] All tests passing locally
- [ ] Commit messages are clear and descriptive
- [ ] PR description explains the changes clearly

---

## 🏆 Recognition

Contributors will be:
- Listed in the README.md
- Mentioned in release notes
- Recognized in project announcements
- Invited to collaborate on future features

---

## 📄 License

By contributing to VECTRA, you agree that your contributions will be licensed under the MIT License.

---

## 🙏 Thank You

We appreciate every contribution, no matter how small! Together, we can make VECTRA an even more powerful and useful platform for intelligent telemetry systems.

---

**Happy Contributing! 🚀**

# party-simulation-qt
Qt-based cocktail party simulation using C++

## Overview
This application models a "cocktail party" scenario at a scientific conference. The simulation populates Room A with N participants, each assigned a unique ID and one of four scientific interests.Unlike standard linear simulations, this version implements a highly parallel architecture where every participant operates as an independent agent (thread), searching for matches in real-time. When interests align, pairs move to a separate Lounge (Room B).
## Key Features
*   **High-Concurrency Engine:** Powered by QtConcurrent and QThreadPool. The system dynamically scales to handle thousands of independent threads, simulating massive interaction density.
*   **Thread-Safe Architecture:** Implements advanced synchronization primitives (QMutexLocker, std::atomic, and pointer-sorting to prevent Deadlocks) for rock-solid stability under extreme load.
*   **Real-time Visualization:** High-performance 2D rendering using QGraphicsScene with adaptive animations. Includes a "State Reconciliation" layer to ensure UI accuracy at 1000+ threads.
*   **Robust Randomization:** Uses the Mersenne Twister (mt19937) engine for high-quality statistical distribution of participant interests and encounter logic.
*   **Interactive Analytics:** Real-time event logging, participant tracking by ID, and comprehensive state reporting available at any moment during the simulation.

## Technical Stack
*   **Language:** C++17
*   **Framework:** Qt 5.12+ (Widgets, Gui, Core, Concurrent)
*   **Build System:** CMake 3.14+
*   **Concurrency:** Multi-threaded worker-object pattern with global thread pool management.
*   **Documentation:** Fully documented architecture ready for technical review.
## Prerequisites (Setting up the environment ubuntu 24)
If you are using a fresh Linux installation (Ubuntu/Debian), run the following commands to install all necessary dependencies (Qt5, CMake, and Google Test):

```bash
# Update package list
sudo apt-get update

# Install build tools and Qt5
sudo apt-get install build-essential cmake qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools libqt5widgets5 libqt5gui5 libqt5core5a

# Install Google Test (GTest)
sudo apt-get install libgtest-dev

# Build and install GTest libraries (Linux specific step)
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib/*.a /usr/lib
```

*Note: After running these commands, your system is ready to compile and run the simulation.*

## Cross-Platform Installation

The application is cross-platform but requires the following environment-specific setups:

### 🪟 Windows
1. **Download & Install:** Get the [Qt Online Installer](https://qt.io).
2. **Components:** Ensure you select **MinGW** (compiler) and **Qt 5.12+**.
3. **GTest:** The easiest way is via `vcpkg`:
   ```cmd
   vcpkg install gtest:x64-windows
   ```
4. **Build:** Use **Qt Creator** to open `CMakeLists.txt` directly.

### 🍎 macOS
1. **Install Homebrew:** if you haven't yet.
2. **Install Dependencies:**
   ```bash
   brew install qt@5 cmake googletest
   ```
3. **Path Setup:** Add Qt to your PATH or point CMake to it:
   ```bash
   export Qt5_DIR=$(brew --prefix qt@5)/lib/cmake/Qt5
   ```

### 🐧 CentOS / RHEL
1. **Install EPEL & Dev Tools:**
   ```bash
   sudo yum install epel-release
   sudo yum groupinstall "Development Tools"
   sudo yum install qt5-qtbase-devel gtest-devel cmake3
   ```
2. **Note:** Use `cmake3` instead of `cmake` on older CentOS versions.

## Compatibility Matrix

| OS | Status | Notes |
| :--- | :--- | :--- |
| **Ubuntu 18.04+** | ✅ Verified | Native development environment |
| **Windows 10/11** | ✅ Supported | Requires MinGW or MSVC 2019+ |
| **macOS Monterey+** | ✅ Supported | Tested with Clang |
| **Android / iOS** | ❌ Not Supported | Requires QML-based UI refactoring |


## How to Build on Linux (ubuntu 24)
1. Ensure Qt 5 and CMake are installed on your system.
2. Clone the repository:
   ```bash
   git clone https://github.com/iron-cat86/party-simulation-qt
   ```
3. Create a build directory and compile:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
4. Run the executable:
   ```bash
   ./PartySimulationUI [participants_count]
   ```
## Quality Assurance & Testing
The project is built with a "Test-Driven" mindset, featuring a comprehensive test suite powered by **Google Test (GTest)**. The testing architecture covers all application layers:

*   **Core Logic Tests:** Verification of the interaction engine, ensuring the "Conservation of Participants" law (people don't disappear) and interest-matching mathematical correctness.
*   **Integration Tests:** Validating the one-threaded "Worker-Object" lifecycle, ensuring stable thread startup and graceful shutdown.
*   **UI/UX Tests:** Automated verification of the `MainWindow` state, checking for the presence of all interactive components (Search, Logs, Reports) and Help menu triggers.
*   **Graphics Engine Tests:** Real-time monitoring of the `QGraphicsScene` state, verifying that visual objects correctly represent the underlying data and react to simulation events.
*   **Utility Tests:** Validation of helper functions, including resource loading (icons/splash) and timestamp formatting.

### Running Tests
To execute the automated test suite, run the following command from the build directory:
```bash
./AppTests && ./SimulationTests && ./MainWindowTests && ./GraphicsTests
```
## Author
**Anna Belova**  
Lead Systems Programmer, SUE "Moscow Metro", R&D Department, Center for Development and Competencies.


# party-simulation-qt
Qt-based cocktail party simulation using C++

## Overview
The application models a "cocktail party" scenario where participants with different scientific interests (Epidemiology, Statistics, Clinical Trials, Health Policy) interact in real-time. When two participants with matching interests meet, they form a pair and move to a separate lounge area.

## Key Features
*   **Event-Driven Logic:** Utilizes Qt's signal-broadcast system for decoupled interaction between simulation logic and UI.
*   **Real-time Visualization:** Dynamic 2D rendering using `QGraphicsScene` with smooth animations for participant movements.
*   **Robust Randomization:** Implements the **Mersenne Twister (mt19937)** engine for high-quality statistical distribution of interests and interactions.
*   **Thread-Safe Architecture:** Core logic is encapsulated to ensure stability and maintainability.
*   **Interactive Search:** Built-in tool to track any participant's status and location by their unique ID.

## Technical Stack
*   **Language:** C++17
*   **Framework:** Qt 5.12+ (Widgets, Gui, Core)
*   **Build System:** CMake 3.14+
*   **Documentation:** Fully documented architecture ready for technical review.

## How to Build
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
   ./PartySimulationUI
   ```

## Author
**Anna Belova**  
Lead Systems Programmer, SUE "Moscow Metro", R&D Department, Center for Development and Competencies, Team 5.


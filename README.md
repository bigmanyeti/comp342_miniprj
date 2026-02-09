# Interactive 2D Gravity Simulator

A real-time N-body gravity simulation using C++, OpenGL, GLFW, and GLEW. This project demonstrates celestial mechanics through interactive visualization, allowing users to observe gravitational interactions, orbital mechanics, and collisions between planetary bodies.

## Features

- **Realistic Physics**: N-body simulation using Newtonian gravity with accurate force calculations
- **Interactive Spawning**: Click and hold to create planets with varying masses
- **Solar System**: Pre-loaded with our 8 planets and the Sun in orbital configuration
- **Collision Detection**: Planets merge/destroy when they collide due to gravitational attraction
- **Cross-Platform**: Works on both Windows and Linux (Arch Linux tested)
- **Real-time Rendering**: Hardware-accelerated OpenGL rendering at 60 FPS

## Controls

- **Space**: To pause
- **Left Click & Hold**: Create a new planet (hold longer for larger mass/size)
- **Right Click**: Remove a planet at cursor position
- **R Key**: Reset to solar system
- **ESC**: Exit application

## Prerequisites

### Windows

1. **Visual Studio** (2019 or later) with C++ development tools
2. **vcpkg** for package management:
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   ```

3. Install dependencies:
   ```cmd
   .\vcpkg install glfw3:x64-windows
   .\vcpkg install glew:x64-windows
   .\vcpkg install glm:x64-windows
   ```

4. **CMake** (3.10 or later): Download from [cmake.org](https://cmake.org/download/)

### Arch Linux

1. Install dependencies:
   ```bash
   sudo pacman -S base-devel cmake glfw-x11 glew glm
   ```

### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libglfw3-dev libglew-dev libglm-dev
```

## Building the Project

### Using CMake (Recommended for both platforms)

1. Clone or download the project
2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. **On Linux:**
   ```bash
   cmake ..
   make
   ./GravitySimulator
   ```

4. **On Windows (with vcpkg):**
   ```cmd
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   .\Release\GravitySimulator.exe
   ```

### Alternative: Direct Compilation (Linux)

```bash
g++ main.cpp -o GravitySimulator -lGL -lGLEW -lglfw -lm -std=c++11
./GravitySimulator
```

## Project Structure

```
GravitySimulator/
├── main.cpp              # Main source code
├── CMakeLists.txt        # CMake build configuration
└── README.md             # This file
```

## How It Works

### Physics Engine

The simulator uses Newton's law of universal gravitation:

```
F = G * (m1 * m2) / r²
```

Where:
- `F` = gravitational force
- `G` = gravitational constant (6.674×10⁻¹¹ N⋅m²/kg²)
- `m1, m2` = masses of the two bodies
- `r` = distance between centers

### Rendering Pipeline

1. **GLFW**: Window management and input handling
2. **GLEW**: OpenGL extension loading
3. **GLM**: Mathematics library for vectors and matrices
4. **OpenGL**: Hardware-accelerated rendering using immediate mode for simplicity

### Collision Detection

Planets collide when their distance is less than the sum of their radii. Upon collision, both planets are removed from the simulation to demonstrate the destructive nature of gravitational collapse.

## Technical Details

- **Language**: C++11
- **Graphics API**: OpenGL 2.1 (for maximum compatibility)
- **Math Library**: GLM (OpenGL Mathematics)
- **Window Library**: GLFW 3
- **Extension Loading**: GLEW
- **Physics Timestep**: 0.016s (fixed timestep with substeps for accuracy)
- **Collision Algorithm**: Circle-circle intersection
- **Force Calculation**: O(n²) all-pairs gravitational interaction

## Customization

You can modify these constants in `main.cpp`:

```cpp
const float G = 6.674e-11f;           // Gravitational constant
const float SCALE_FACTOR = 1e9f;      // Distance scaling
const float TIME_STEP = 0.016f;       // Physics timestep
const float PHYSICS_SUBSTEPS = 5.0f;  // Accuracy vs performance
```

## Known Limitations

- No orbital prediction/trails (can be added)
- No planet merging (planets are destroyed on collision)
- 2D simulation only
- No relativistic effects
- Maximum ~100 planets for smooth performance

## Troubleshooting

### Linux: "Cannot find -lGL"
```bash
sudo apt-get install mesa-common-dev libgl1-mesa-dev
```

### Windows: "Cannot find GLFW/GLEW"
Make sure vcpkg is properly integrated:
```cmd
.\vcpkg integrate install
```

### Black screen on startup
The solar system might be off-screen. Try pressing 'C' to clear and create planets manually.

## Future Enhancements

- Zoom and pan controls
- Orbital trails/trajectory prediction
- Planet information display (mass, velocity, distance)
- Save/load scenarios
- Planet merging instead of destruction
- 3D visualization
- GPU-accelerated physics (compute shaders)

## License

This project is created for educational purposes. Feel free to use and modify.

## Credits

Developed as a Computer Graphics project demonstrating physics simulation and OpenGL rendering techniques.

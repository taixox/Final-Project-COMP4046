# Dynamic Lighting Comparison in Real-Time Rendering

This repository contains a **real-time OpenGL renderer** that implements and compares multiple illumination models under **dynamic multi-light conditions**. The project evaluates the **visual quality and performance trade-offs** of classical and physically motivated lighting models in a rasterization-based rendering pipeline.

The renderer supports **Phong**, **Blinn–Phong**, and **Cook–Torrance** shading models, allows **runtime switching** between them, and measures performance in terms of frames per second (FPS) while varying the number of active dynamic lights.

---

## Features

- Real-time rasterization using **OpenGL 3.3 Core Profile**
- Support for **multiple dynamic point lights** (up to 25)
- Runtime switching between lighting models:
  - Phong
  - Blinn–Phong
  - Cook–Torrance (microfacet-based)
- Forward rendering pipeline with per-fragment lighting
- OBJ/MTL scene loading using TinyOBJLoader
- Material properties derived from MTL files
- Dynamic light animation using trigonometric motion
- Real-time FPS measurement
- Final frame capture to PNG image

---

## System Requirements

- **C++17** or later
- **OpenGL 3.3** compatible GPU
- Operating system:
  - Windows / Linux / macOS

---

## Dependencies

This project uses the following libraries:

| Library | Purpose |
|---------|--------|
| GLFW | Window creation and input handling |
| GLAD | OpenGL function loading |
| GLM | Mathematics (vectors, matrices, transformations) |
| TinyOBJLoader | OBJ and MTL file loading |
| stb_image_write | Image capture and PNG export |

---

## Project Structure

```text
Final-Project-COMP4046/
├── include/       # Header files
├── lib/           # Libraries
├── scene/         # Scene files (OBJ/MTL)
├── shaders/       # Shader programs
├── src/           # Source code (.cpp files)
│   └── main.cpp
├── app.exe        # Compiled executable
├── final_render.png  # Example output
└── README.md
```

## Build Instructions

To build the project from source:

1. Ensure you have a C++17 compiler (e.g., `g++`) and all required libraries in `lib/`.
2. Open a terminal in the project root.
3. Compile using:

```bash
g++ src/main.cpp src/glad.c -Iinclude -Llib -lglfw3 -lopengl32 -lgdi32 -o app.exe
```

4. Make sure `shaders/` and `scene/` folders are in the same directory as `app.exe` when running.
5. After building, run the executable:

```bash
./app.exe
```

The program will load scene files and shaders automatically. The final rendered frame is saved as `final_render.png` upon exit.


## Controls
*The renderer starts automatically using the Phong lighting model, and the user can switch in real-time between Phong, Blinn–Phong, and Cook–Torrance models.*

| Key | Action                         |
|-----|--------------------------------|
| 1   | Switch to Phong lighting       |
| 2   | Switch to Blinn–Phong lighting |
| 3   | Switch to Cook–Torrance lighting|


*Lighting model changes occur in real-time without resetting the scene.*

## Lighting Models

### Phong
A classical analytic model using reflection vectors to compute specular highlights. It is computationally efficient but may produce less stable highlights under dynamic lighting.

### Blinn–Phong
A modification of the Phong model that uses a half-vector between the light and view directions. This model produces smoother highlights and improved numerical stability.

### Cook–Torrance
A physically motivated microfacet model incorporating:
- Normal Distribution Function (NDF)
- Fresnel term (Schlick approximation)
- Geometric attenuation

This model produces the most realistic reflections at a higher computational cost.

## Dynamic Light Configuration

- **Up to 25 point lights**
- Lights move dynamically in circular trajectories around the scene.
- Each light has:
  - Unique radius
  - Height
  - Angular speed
  - Phase offset
- All lights emit white light with identical intensity.
- The dynamic motion ensures continuously changing illumination patterns for evaluation.

## Performance Measurement

- FPS is measured over one-second intervals during execution.
- Average FPS is computed over 10-second runs.
- Performance scales linearly with the number of lights due to forward rendering.

## Image Capture

At program termination, the final rendered frame is captured using `glReadPixels` and saved as:

```bash
final_render.png
```

This allows qualitative comparison between lighting models.

## Results Summary

| # Lights | Phong FPS | Blinn–Phong FPS | Cook–Torrance FPS |
|----------|-----------|-----------------|-------------------|
| 2        | 1014.22   | 982.78          | 945.56            |
| 12       | 495.56    | 484.56          | 385.67            |
| 25       | 296.22    | 287.78          | 296.56            |

These results illustrate the trade-off between performance and visual realism in real-time lighting.

## References

- Phong, B. T. *Illumination for Computer Generated Pictures*, 1975
- Blinn, J. F. *Models of Light Reflection*, 1977
- Cook, R. L., Torrance, K. E. *A Reflectance Model for Computer Graphics*, 1981
- LearnOpenGL — Physically Based Rendering

## Future Work

- Deferred or clustered shading
- Shadow mapping
- Material-dependent roughness
- Physically based light attenuation
- GPU profiling and optimization

## License

This project is intended for educational and research purposes.  
You are free to use, modify, and extend the code with appropriate attribution.

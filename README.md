
# Shabnigroth

1. [Core](#core)
0. [OpenGL](#opengl) libray (developing)
0. [Neuronet](#neuronet) shabnigroth (planed)



# Run

[./make](./make)



# Current objects scheme

```mermaid
flowchart

ScenePayload
Log
Camera

math((math<br>libs))

Result --> Camera
Result --> Scene
Result --> Payload

Payload --> ScenePayload
Camera -.-> UserPayload
Log -.-> UserPayload

ScenePayload -->  UserPayload 
Log -.-> Scene
ScenePayload-.-> Scene

Log -.-> main


UserPayload -.-> main
Scene -.-> main


Point3 -.-> math
Matrix4 -.-> math
Point4 -.-> math

math -.-> Scene
```

1. Solid line - extends class
0. Dotted line - uses class

# Core

1. Core is contain the tiniy libraries for application with users controllers.



# OpenGL

1. User interface based on on [glfw](https://www.glfw.org/).
0. Supports:
    1. mouse events
    0. keyboards events
    0. math library for vectors operations
    0. matrix transformation



# Shabnigroth

1. Waiting.

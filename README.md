# ParaConquer

# Description
ParaConquer is a personal game engine that I maintain.
Think of it as a “personal car for a mechanic” build, maintain, and fine-tune it exactly how I want, experimenting with different features, systems.

# Core Features 
    - Ecs
    - Static Reflection at static initilization
    - Serialization

### Dependicies
- **Vulkan SDK** (version 1.2)
- **D3D12 SDK**
- **MotionCore** (Physics Engine)
- **Tracy** (Profiling)
- **Assimp** (Model Loading)
- **ImGui** (GUI)
- **DXC** (HLSL -> SPRIV)

### How to build

python BuildProject.py [OPTIONS]

| Option         | Description                                                                                  | Default |
|----------------|----------------------------------------------------------------------------------------------|---------|
| `--simd`       | SIMD instruction set to use. Options: `AUTO`, `SSE2`, `AVX`, `AVX2`, `NEON`, `SVE`, `NONE`   | `NONE`  |
| `--profiling`  | Enable or disable Tracy profiling. Options: `ON` / `OFF`                                     | `OFF`   |
| `--build_test` | Enable or disable building unit tests. Options: `ON` / `OFF`                                 | `OFF`   |

### TODO

- **DeferredDestroy Vulkan Resource**
- **Material Variant**(HLSL only)
-

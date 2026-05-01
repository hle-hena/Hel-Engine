# Hel Engine

> A personal rendering and game engine built from scratch in C++20, leveraging Vulkan for modern, GPU-driven graphics.

----------

## Table of Contents

-   [About](#about)
-   [Features](#features)
    -   [Entity Component System (ECS)](#entity-component-system-ecs)
    -   [Asset Manager](#asset-manager)
    -   [Docks](#docks)
    -   [Style Editor](#style-editor)
    -   [Cross-Platform Support](#cross-platform-support)
-   [Tech Stack](#tech-stack)
-   [Getting Started](#getting-started)
    -   [Windows](#windows)
    -   [Linux](#linux)
-   [Roadmap](#roadmap)

----------

## About

Hel is a personal project initiated by Hugo Le Henaff with the primary goal of deepening knowledge of low-level graphics APIs and modern computer graphics techniques. Built entirely from scratch in C++20, the engine uses Vulkan as its rendering backend.

The project serves as a hands-on exploration of engine architecture, covering areas such as scene management, asset pipelines, editor tooling, and rendering techniques. While it is not intended as a production-grade engine, it follows sound engineering practices and evolves continuously as new systems are implemented and refined.

----------

## Features

### Entity Component System (ECS)

The ECS is built around a sparse-set architecture. Each component type lives in its own `Pool`, which stores components in a tightly packed array alongside a parallel entity list and a sparse index array keyed by entity ID. Removal is O(1) — the last element is swapped into the freed slot to keep the dense array contiguous and iteration branch-free.

Entity handles are versioned 32-bit integers: 20 bits encode the slot index and the remaining 12 encode a generation counter. When an entity is destroyed, its slot is recycled with an incremented version, so any surviving handle to it will fail validation without any additional bookkeeping.

Querying is done through typed `View` objects that accept include and exclude component lists at compile time. The view selects the smallest include pool as its lead, then filters candidates against the remaining pools during iteration — only entities that satisfy all constraints are exposed to the caller.

GPU-resident components are a first-class concept. A component can opt in by declaring `gpuVisible = true`, at which point its pool owns a VMA-backed storage buffer. Writes are tracked at the individual element level: a `ModificationProxy` RAII wrapper, returned by `ComponentHandle::modify()`, automatically queues a dirty write on destruction. On flush, the pool either uploads the full buffer if its layout changed or patches only the modified slots. Superseded buffers are not immediately freed — they are held and released after `MAX_FRAMES_IN_FLIGHT` frames to avoid destroying resources still in use by the GPU.

### Asset Manager

The asset manager provides a type-safe, path-keyed cache for GPU resources. Internally it uses a two-level map: the outer key is a `std::type_index`, the inner key is the file path. This lets all asset types share a single cache structure without requiring a common base class — assets are stored as `shared_ptr<void>` and cast back to the concrete type on retrieval.

Loading is fully delegated to the asset type itself via a static `Asset::load(device, path)` convention. The manager never contains format-specific logic; it only decides whether a cached entry can be returned or a fresh load is needed.

Two opt-in behaviours are available to asset types. First, a type can declare an `AssetPool` alias to share its cache bucket with a related type, allowing, for instance, specialised variants to avoid re-loading a resource already cached under a parent type. Second, a type can declare `isLoadedFully()` to signal that a cached entry is only partially initialised — in that case the manager falls through and reloads rather than returning an incomplete asset.

Lifetime is managed entirely through `std::shared_ptr`. The cache holds one reference; any system or component holding a handle keeps the asset alive. When the last reference is dropped, the asset and its GPU resources are destroyed automatically.

### Docks
The docking system is implemented from scratch on top of Dear ImGui, without relying on ImGui's built-in docking branch. The layout is represented as a binary tree of `Dock` nodes. Each node is either a `Split` — holding two child docks and a directional split ratio — or a `TabGroup` — holding an ordered list of panels. The tree is kept minimal automatically: when a tab group loses all its panels, it signals its parent, which collapses the sibling node up to take its place.

Panels are defined by implementing a single `IPanel` interface. The docking system treats them as opaque; it only needs a label and a `render` method. This makes adding new panel types straightforward — the engine ships with several built-in panels, and the same interface is available for custom ones, with no changes required to the docking system itself.

Drag-and-drop is handled geometrically each frame. When a tab is being dragged over a dock, five drop zones are tested: four triangles subdividing the dock's rectangle (one per cardinal direction) trigger a directional split, while a fifth zone along the tab bar allows reordering within the same group. The insertion position in the tab bar is found by comparing the cursor's X coordinate against a cached array of tab edge positions recorded during the previous render.

When a split occurs, the current dock is converted into a `Split` node, two child docks are created, the existing panels migrate to one child, and the dragged panel is placed in the other. Split ratios are stored as absolute pixel values during use and serialised as proportions of the dock's size, so saved layouts restore correctly at any window resolution. The full layout tree serialises to and from JSON, allowing editor arrangements to persist across sessions.

### Style Editor

The style editor is a built-in panel that provides live control over the editor's visual theme. Rather than exposing every ImGui color slot individually, it works through a palette of named base colors — primary, secondary, tertiary, shadow, and highlight — which all UI elements reference by default. Changing a base color propagates instantly across every element that references it, making broad theme changes a matter of adjusting a handful of swatches.

Individual colors can also be overridden independently when finer control is needed, with a per-element alpha knob for opacity. The full theme serialises to JSON and is restored automatically on next launch.

### Cross-Platform Support

The engine targets both Windows and Linux. Platform-specific build configurations are managed through CMake presets, ensuring a consistent development experience across environments.

----------

## Tech Stack
|Layer|Technology|
|--|--|
|Language|C++20|
|Graphics API|Vulkan|
|UI|Dear ImGui|
|Windowing|GLFW|

----------

## Getting Started

### Windows

**Requirements**

-   Visual Studio with a compiler supporting at least C++20
-   CMake and Ninja
-   Vulkan SDK with validation layers enabled

**Steps**

1.  Clone the repository with all submodules:
    
    ```sh
    git clone --recurse-submodules https://github.com/hle-hena/Hel-Engine
    ```
    
2.  Open the project folder and configure with CMake.
    
3.  Build and run `hel.exe`.
    

----------

### Linux

**Clone the repository:**

```sh
git clone https://github.com/hle-hena/Hel-Engine
cd Hel-Engine
```

**Initialize submodules:**

```sh
git submodule update --init
```

**Run the setup script:**

```sh
./scripts/setup.sh
```

**Build — Debug:**

```sh
cmake --preset linux-debug && cmake --build build -j
```

**Build — Release:**

```sh
cmake --preset linux-release && cmake --build build -j
```

----------

## Roadmap

The following improvements and features are planned for future development:

-   **Validation system refactor** — Replace the current validation layer with `std::expected` for more expressive, value-based error handling.
-   **Light definition in engine** — Move light source definitions out of shaders and into the engine layer, enabling more flexible and data-driven lighting setups.
-   **Physics system** — Integrate a basic rigid-body physics simulation.
-   **Shadow rendering** — Add support for real-time shadow maps, starting with directional and point lights.
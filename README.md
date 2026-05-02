
# Hel Engine

> A personal rendering and game engine built from scratch in C++20, leveraging Vulkan for modern, GPU-driven graphics.

----------

## Table of Contents

-   [About](#about)
-   [Features](#features)
	-   [Vulkan wrapper](#vulkan)
	-   [Render wrapper](rendering)
    -   [Render queue](#render-queue)
    -   [Read queue](#read-queue)
    -   [Entity Component System (ECS)](#entity-component-system-ecs)
    -   [Asset Manager](#asset-manager)
    -   [Docks](#docks)
    -   [Style Editor](#style-editor)
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
### Vulkan wrapper
The Vulkan wrapper abstracts the most verbose parts of the API into focused, RAII-managed types. Each major concept — device selection, swapchain management, buffers, images, pipelines, and descriptors — is encapsulated independently, so higher-level systems interact with clean C++ objects rather than raw `Vk*` handles and sprawling `VkCreateInfo` chains.

Device initialisation handles physical device selection, queue family resolution, and logical device creation in one step, exposing only the queues and properties that the rest of the engine actually needs. The swapchain wrapper owns its images and image views, responds to resize events by rebuilding itself in place, and presents a simple acquire/present interface that hides synchronisation boilerplate.

Buffers and images are allocated through VMA, with helper factories for creating different configurations easily, allowing to make vertex, index, uniform, and staging buffers on the buffer side and color attachments, depth targets, and sampled textures on the image side. Ownership is tied to object lifetime, so destruction is always deterministic.

Pipeline construction is split across `Pipeline`, which owns a single `VkPipeline` and its lifecycle, and `PipelineMap`, which handles the fact that a pipeline must be compiled against a specific set of attachment formats. A `PipelineConfig` aggregates every `VkPipeline*CreateInfo` field into one place; `defaultPipelineconfig` seeds it with sensible defaults, and a templated `setVertexInputDescriptions<VertexType>` delegates binding and attribute descriptions directly to the vertex type. `PipelineMap` maintains a hash map keyed on color and depth format combinations, compiling a new pipeline on the first bind against an unseen configuration and returning the cached one on all subsequent calls. Layout creation is lazy and callback-driven — the caller receives the descriptor set layout and push constant vectors to populate, with a second callback available to override any config field after defaults are applied.

**Roadmap** — The Vulkan wrapper is planned to be extracted into a standalone library called `niflhel`, separating it cleanly from the engine layer and making it reusable independently of Hel.

---
### Render wrapper
The render wrapper is built around two cooperating types, `RenderPass` and `Renderer`, with draw calls expressed through a nested `Draw` builder.

`RenderPass` configures a dynamic rendering pass before it begins. Color and depth attachments are registered via `addColorWrite` and `addDepthWrite`, each of which transitions the image to the appropriate layout and records the format into a `RenderingConfig`. Load and store operations are configurable per attachment type. Calling `beginPass` commits the `VkRenderingInfo`, sets the viewport and scissor dynamically, and moves the pass into a `Renderer` — after which the `RenderPass` is no longer usable directly; its destructor calls `vkCmdEndRendering` on whatever command buffer it still holds.

`Renderer` is the live drawing context. Systems obtain a `Draw` builder through `drawCommand`, which already pre-binds the global UBO as a dynamic descriptor. The builder accumulates vertex buffers, an optional index buffer, descriptor sets — static or dynamic — and an optional push constant, all through a fluent interface. `submit` then issues the actual Vulkan commands: pipeline binding is skipped if the same `PipelineMap` was already bound for the previous draw call, and the final command is either `vkCmdDraw` or `vkCmdDrawIndexed` depending on whether an index buffer was registered.

---
### Render queue
`RenderQueue` is a simple static accumulator that decouples systems from the render loop. Systems push `RenderRequest` values — an entity handle, a screen-space origin, a main image, and an optional map of named secondary images — at any point during the frame. At render time, the queue is flushed in one call and the collected requests are consumed by whatever pass needs them. Requests are hashable and equality-comparable by entity handle, origin, and image dimensions, allowing callers to detect unchanged submissions without storing external state.

**Roadmap** — currently callers are responsible for creating and managing the `Image` objects they attach to a request. A planned improvement will invert this: the caller will supply only an image configuration, and the queue will own image creation and lifetime internally.

---
### Read queue
The read queue solves a fundamental GPU readback problem: results from a draw call cannot be read back on the same frame they are submitted, because the GPU has not finished writing them yet. This is made tractable by the deferred submission model of `DrawQueue` — because draw calls are not issued immediately but accumulated and sorted by level before execution, there is a well-defined point in the frame after which no more writes to a given image will occur. `Read::Queue` exploits this by letting systems register readback requests through a typed `Builder`, which records the source image, a pixel offset, and an extent. On `push`, a VMA-backed host-visible buffer sized for the requested region and element type is allocated and the request is appended to the queue. When `Read::Queue::execute` is called after draw submission, it walks the pending requests and issues `vkCmdCopyImageToBuffer` for each one. The resulting `Context` holds the buffer and the frame index at which the request was made, so the caller can defer actually reading the mapped memory until enough frames have elapsed for the GPU to have finished writing.

**Roadmap** — `push` currently allocates a fresh buffer for every request. A planned improvement will check an existing request cache first and reuse a buffer whose source image and region match a previous submission, avoiding redundant allocations for readbacks that recur every frame.

---
### Entity Component System (ECS)
The ECS is built around a sparse-set architecture. Each component type lives in its own `Pool`, which stores components in a tightly packed array alongside a parallel entity list and a sparse index array keyed by entity ID. Removal is O(1) — the last element is swapped into the freed slot to keep the dense array contiguous and iteration branch-free.

Entity handles are versioned 32-bit integers: 20 bits encode the slot index and the remaining 12 encode a generation counter. When an entity is destroyed, its slot is recycled with an incremented version, so any surviving handle to it will fail validation without any additional bookkeeping.

Querying is done through typed `View` objects that accept include and exclude component lists at compile time. The view selects the smallest include pool as its lead, then filters candidates against the remaining pools during iteration — only entities that satisfy all constraints are exposed to the caller.

GPU-resident components are a first-class concept. A component can opt in by declaring `gpuVisible = true`, at which point its pool owns a VMA-backed storage buffer. Writes are tracked at the individual element level: a `ModificationProxy` RAII wrapper, returned by `ComponentHandle::modify()`, automatically queues a dirty write on destruction. On flush, the pool either uploads the full buffer if its layout changed or patches only the modified slots. Superseded buffers are not immediately freed — they are held and released after `MAX_FRAMES_IN_FLIGHT` frames to avoid destroying resources still in use by the GPU.

---
### Asset Manager
The asset manager provides a type-safe, path-keyed cache for GPU resources. Internally it uses a two-level map: the outer key is a `std::type_index`, the inner key is the file path. This lets all asset types share a single cache structure without requiring a common base class — assets are stored as `shared_ptr<void>` and cast back to the concrete type on retrieval.

Loading is fully delegated to the asset type itself via a static `Asset::load(device, path)` convention. The manager never contains format-specific logic; it only decides whether a cached entry can be returned or a fresh load is needed.

Two opt-in behaviours are available to asset types. First, a type can declare an `AssetPool` alias to share its cache bucket with a related type, allowing, for instance, specialised variants to avoid re-loading a resource already cached under a parent type. Second, a type can declare `isLoadedFully()` to signal that a cached entry is only partially initialised — in that case the manager falls through and reloads rather than returning an incomplete asset.

Lifetime is managed entirely through `std::shared_ptr`. The cache holds one reference; any system or component holding a handle keeps the asset alive. When the last reference is dropped, the asset and its GPU resources are destroyed automatically.

---
### Docks
The docking system is implemented from scratch on top of Dear ImGui, without relying on ImGui's built-in docking branch. The layout is represented as a binary tree of `Dock` nodes. Each node is either a `Split` — holding two child docks and a directional split ratio — or a `TabGroup` — holding an ordered list of panels. The tree is kept minimal automatically: when a tab group loses all its panels, it signals its parent, which collapses the sibling node up to take its place.

Panels are defined by implementing a single `IPanel` interface. The docking system treats them as opaque; it only needs a label and a `render` method. This makes adding new panel types straightforward — the engine ships with several built-in panels, and the same interface is available for custom ones, with no changes required to the docking system itself.

Drag-and-drop is handled geometrically each frame. When a tab is being dragged over a dock, five drop zones are tested: four triangles subdividing the dock's rectangle (one per cardinal direction) trigger a directional split, while a fifth zone along the tab bar allows reordering within the same group. The insertion position in the tab bar is found by comparing the cursor's X coordinate against a cached array of tab edge positions recorded during the previous render.

When a split occurs, the current dock is converted into a `Split` node, two child docks are created, the existing panels migrate to one child, and the dragged panel is placed in the other. Split ratios are stored as absolute pixel values during use and serialised as proportions of the dock's size, so saved layouts restore correctly at any window resolution. The full layout tree serialises to and from JSON, allowing editor arrangements to persist across sessions.

---
### Style Editor
The style editor is a built-in panel that provides live control over the editor's visual theme. Rather than exposing every ImGui color slot individually, it works through a palette of named base colors — primary, secondary, tertiary, shadow, and highlight — which all UI elements reference by default. Changing a base color propagates instantly across every element that references it, making broad theme changes a matter of adjusting a handful of swatches.

Individual colors can also be overridden independently when finer control is needed, with a per-element alpha knob for opacity. The full theme serialises to JSON and is restored automatically on next launch.

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

-   **Validation system refactor** — Replace the current class validation layer with `std::expected` for more expressive, value-based error handling.
-   **Light definition in engine** — Move light source definitions out of shaders and into the engine layer, enabling more flexible and data-driven lighting setups.
-   **Physics system** — Integrate a basic rigid-body physics simulation.
-   **Shadow rendering** — Add support for real-time shadow maps, starting with directional light.

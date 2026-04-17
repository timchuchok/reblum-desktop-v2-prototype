# Reblum — Desktop v2 Prototype

Desktop image viewer with real-time highlight/shadow color grading, built on Qt 6 + RHI.

---

## Requirements

| Dependency | Version |
|---|---|
| Qt | 6.7+ (Core, Gui, GuiPrivate, Widgets, Concurrent, ShaderTools, Svg) |
| CMake | 3.20+ |
| C++ | 17 |
| Platform | macOS 13+, Windows 10/11 |

> **Note:** `Qt6::ShaderTools` is required — it compiles `.vert`/`.frag` sources into `.qsb` binaries at build time via `qt6_add_shaders`.

---

## Build

```bash
# 1. Clone
git clone https://github.com/timchuchok/reblum-desktop-v2-prototype.git
cd reblum-desktop-v2-prototype

# 2. Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Build
cmake --build build --parallel

# 4. Run
open build/reblum.app   # macOS
```

If Qt is installed in a non-standard path, point CMake to it:

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/macos
```

### Distribution (macOS)

`open build/reblum.app` works locally because Qt frameworks are found via `DYLD_LIBRARY_PATH`. To distribute the `.app` to another machine, bundle the frameworks first:

```bash
/path/to/Qt/6.x.x/macos/bin/macdeployqt build/reblum.app
```

To produce a distributable `.dmg`:

```bash
macdeployqt build/reblum.app -dmg
```

After `macdeployqt`, the `.app` is self-contained and can be zipped or distributed via `.dmg` without a Qt installation on the target machine.

---

## Building on Windows

### Prerequisites

1. **Qt 6.7+** — install via the [Qt Online Installer](https://www.qt.io/download-qt-installer). Select the component for your compiler:
   - `MSVC 2022 64-bit` — recommended
   - `MinGW 13.1 64-bit` — alternative if you don't have Visual Studio
2. **CMake 3.20+** — bundled with Visual Studio 2022, or install from [cmake.org](https://cmake.org/download/)
3. **Visual Studio 2022** (for MSVC) — install the **"Desktop development with C++"** workload

### Build (MSVC)

Open **"x64 Native Tools Command Prompt for VS 2022"**, then:

```bat
git clone https://github.com/timchuchok/reblum-desktop-v2-prototype.git
cd reblum-desktop-v2-prototype

cmake -B build -G "Visual Studio 17 2022" -A x64 ^
      -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2022_64"

cmake --build build --config Release
```

### Build (MinGW)

Open a regular Command Prompt with MinGW in `PATH`:

```bat
cmake -B build -G "MinGW Makefiles" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\mingw_64"

cmake --build build --parallel
```

### Run

The built executable won't find Qt DLLs until you deploy them next to it:

```bat
cd build\Release          # or build\ for MinGW

windeployqt --release reblum.exe
reblum.exe
```

`windeployqt` is in `C:\Qt\6.x.x\msvc2022_64\bin\` — make sure it's in `PATH` or call it by full path.

### RHI backend on Windows

By default Qt RHI uses **Direct3D 11**. You can override it with an environment variable:

```bat
set QSG_RHI_BACKEND=d3d12   # Direct3D 12
set QSG_RHI_BACKEND=vulkan  # Vulkan (requires Vulkan SDK + Qt built with Vulkan support)
set QSG_RHI_BACKEND=opengl  # OpenGL
reblum.exe
```

D3D11 is the safest choice for broad hardware compatibility.

---

## Usage

- **Open image** — drag & drop a file onto the window, or use **File → Open Image…** (`⌘O`)
- **Pan** — click and drag in the canvas
- **Zoom** — pinch gesture or scroll wheel; **Fit** / **100%** buttons in the bottom bar
- **Effects** — adjust Orange (highlights) and Green (shadows) opacity and threshold in the right panel; toggle visibility with the eye button
- **Theme** — **Theme → Dark / Light**

---

## Architecture

### Approach

The project uses a **layered + feature-oriented** decomposition with three isolated worlds:

```
UI (EffectPanel / ImageView)
   ↓ signal
MainWindow (UI wiring only)
   ↓
AppController (owns controllers + QUndoStack)
   ↓
ImageController / EffectsController
   ↓
EffectSettings / ViewState (core)
   ↓
Renderer (RHI)
   ↓
GPU shader
```

| Layer | Path | Rule |
|---|---|---|
| **Core** | `src/core/` | Pure C++ structs, no Qt UI, directly unit-testable |
| **App** | `src/app/` | Controllers + orchestration, owns `QUndoStack` |
| **UI** | `src/ui/` | Qt Widgets only — renders and emits signals, no business logic |
| **Rendering** | `src/rendering/` | Isolated RHI pipeline, knows nothing about widgets |

### Why this decomposition

**Core has zero UI dependency.** `EffectSettings`, `ViewState`, and `ImageModel` are plain structs. They can be tested, serialized, or reused without linking Qt Widgets. This is the main reason a future test suite can cover business logic without spinning up a QApplication.

**Controllers are thin orchestrators.** `ImageController` and `EffectsController` own state and emit typed signals — they never touch widgets directly. `AppController` wires controllers together via signals/slots and owns `QUndoStack`. This separation means undo/redo, future scripting, or a headless mode require no changes to the UI layer.

**Renderer is decoupled from `QWidget`.** `Renderer` only knows `QRhi*`, `QRhiCommandBuffer*`, and `EffectSettings`. `ImageView` (a `QRhiWidget`) owns the renderer and feeds it data — the renderer itself has no notion of layout, events, or Qt Widgets.

**Single shader pass, two effects.** Orange (highlights) and Green (shadows) both read from the original pixel and are applied in one `image.frag` pass. A two-pass approach would add an intermediate render target and a second pipeline for no quality benefit, since the effects are independent and non-compositing.

### Key structures

```cpp
// core/effects/EffectSettings.h
struct EffectSettings {
    float  opacity   = 0.0f;   // 0..1
    float  threshold = 0.5f;   // 0..1
    bool   enabled   = true;
    QColor color     = Qt::white;
};

// core/viewport/ViewState.h
struct ViewState {
    float   zoom   = 1.0f;
    QPointF offset = {0, 0};
};
```

### Shader UBO layout (std140)

```
VertexUBO (80 bytes)
  [0]  mat4  corrMatrix   — NDC correction for RHI backend differences
  [64] vec4  imgRect      — image rect in NDC (left, top, right, bottom)

FragUBO (64 bytes)
  [0]  float orangeOpacity
  [4]  float orangeThreshold
  [8]  float greenOpacity
  [12] float greenThreshold
  [16] int   orangeEnabled
  [20] int   greenEnabled
  [24] float[2] _pad
  [32] vec4  orangeColor
  [48] vec4  greenColor
```

### Extensibility

| Goal | What to touch |
|---|---|
| New effect (same pass) | Add fields to `EffectSettings` + `FragUBO`, extend `image.frag` |
| New effect (multi-pass) | Add intermediate `QRhiTexture` RT in `Renderer`, new `.frag` shader |
| Undo / Redo | Push `QUndoCommand` via `AppController` |
| New theme | New `.qss` in `assets/themes/`, register in `ThemeManager` |
| New language | New `.ts` in `assets/translations/` |
| Unit tests | `src/core/` has no UI dependency — test directly with Qt Test or Catch2 |



---

## Project structure

```
reblum-desktop-v2-prototype/
├── CMakeLists.txt
├── assets/
│   ├── shaders/
│   │   ├── fullscreen.vert
│   │   └── image.frag
│   ├── themes/
│   │   ├── dark.qss
│   │   └── light.qss
├── src/
│   ├── main.cpp
│   ├── app/                  # orchestration
│   │   ├── Application
│   │   ├── AppController     ← connects controllers, owns QUndoStack
│   │   ├── ImageController
│   │   ├── EffectsController
│   │   └── ThemeManager
│   ├── core/                 # business logic (no Qt UI)
│   │   ├── image/ImageModel
│   │   ├── effects/EffectSettings
│   │   └── viewport/ViewState
│   ├── rendering/            # RHI pipeline
│   │   └── Renderer
│   └── ui/                   # Qt Widgets
│       ├── main_window/MainWindow
│       ├── panels/           ← RightPanel, EffectPanel
│       └── widgets/          ← ImageView (QRhiWidget), GradientSlider, EyeButton
└── resources/
    └── resources.qrc
```

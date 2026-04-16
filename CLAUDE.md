# reblum-desktop-v2-prototype — Architecture

## Architectural Approach

**Layered + Feature-oriented** with a clear separation into three worlds:

- 🟦 **Core** — pure business logic, no knowledge of UI
- 🟪 **UI** — Qt Widgets, only renders and emits signals
- 🟥 **Rendering** — isolated RHI pipeline

## Data Flow

```
UI (EffectPanel / ImageView)
   ↓ signal
AppController (orchestrator)
   ↓
ImageController / EffectsController
   ↓
EffectSettings / ViewState (core)
   ↓
Renderer (RHI)
   ↓
GPU shader
```

## Project Structure

```
reblum-desktop-v2-prototype/
├── CMakeLists.txt
├── README.md
│
├── assets/
│   ├── shaders/
│   │   ├── fullscreen.vert
│   │   └── image.frag        ← single pass: orange + green in one shader
│   ├── themes/
│   │   └── dark.qss
│   └── translations/
│       ├── app_en.ts
│       └── app_uk.ts
│
├── src/
│   ├── main.cpp
│   │
│   ├── app/                          # orchestration
│   │   ├── Application.h/cpp
│   │   ├── AppController.h/cpp       ← connects controllers, owns QUndoStack
│   │   ├── ImageController.h/cpp     ← load, zoom, pan
│   │   ├── EffectsController.h/cpp   ← opacity, threshold, enabled
│   │   └── ThemeManager.h/cpp        ← themes + localization
│   │
│   ├── core/                         # business logic (no Qt UI)
│   │   ├── image/
│   │   │   ├── ImageModel.h
│   │   │   └── ImageModel.cpp
│   │   ├── effects/
│   │   │   └── EffectSettings.h      ← struct { float opacity, threshold; bool enabled; QColor color; }
│   │   └── viewport/
│   │       ├── ViewState.h           ← struct { float zoom; QPointF offset; }
│   │       └── ViewState.cpp
│   │
│   ├── rendering/                    # RHI pipeline
│   │   └── Renderer.h/cpp            ← setImage, setEffects, render(cb, rt, rect, dpr)
│   │
│   └── ui/                           # Qt Widgets
│       ├── main_window/
│       │   └── MainWindow.h/cpp      ← builds UI, handles DnD, delegates to controllers
│       ├── panels/
│       │   ├── RightPanel.h/cpp
│       │   └── EffectPanel.h/cpp     ← single class for both Orange and Green
│       ├── widgets/
│       │   ├── ImageView.h/cpp       ← QRhiWidget: events + viewport, owns Renderer
│       │   ├── GradientSlider.h/cpp
│       │   └── EyeButton.h/cpp
│       └── topbar/
│           ├── TopBar.h
│           └── TopBar.cpp
│
├── tests/
│   ├── CMakeLists.txt
│   └── core/
│       ├── test_ImageModel.cpp
│       ├── test_EffectSettings.cpp
│       └── test_ViewState.cpp
│
└── resources/
    └── resources.qrc
```

## Responsibilities

### AppController
- Pure orchestration — does not think, only connects
- Owns `QUndoStack`
- Connects controllers to each other via signals

```cpp
connect(imageController, &ImageController::imageLoaded,
        effectsController, &EffectsController::reset);
```

### ImageController
```cpp
void loadImage(const QString& path);
void setZoom(float zoom);
void pan(QPoint delta);
```

### EffectsController
```cpp
void setOrangeOpacity(float opacity);
void setOrangeThreshold(float threshold);
void setOrangeEnabled(bool enabled);
void setGreenOpacity(float opacity);
void setGreenThreshold(float threshold);
void setGreenEnabled(bool enabled);
```

### MainWindow
- Builds the UI
- Handles drag-and-drop at window level (QRhiWidget's Metal layer absorbs drops on macOS)
- Delegates everything to controllers

```cpp
connect(slider, &QSlider::valueChanged, this, [=](int v) {
    appController->effectsController()->setOrangeOpacity(v / 100.0f);
});
```

## Core Structures

```cpp
// core/effects/EffectSettings.h
struct EffectSettings {
    float  opacity   = 0.0f;    // 0..1
    float  threshold = 0.5f;    // 0..1
    bool   enabled   = true;
    QColor color     = Qt::white;
};

// core/viewport/ViewState.h
struct ViewState {
    float   zoom   = 1.0f;
    QPointF offset = {0, 0};
};

// ui/panels/EffectPanel.h
enum class EffectType { Orange, Green };
class EffectPanel : public QWidget {
    explicit EffectPanel(EffectType type, QWidget* parent = nullptr);
};
```

## Shader Logic

Single pass (`image.frag`). Both effects read from the original pixel — they do not
interact, so two separate passes would add complexity with no benefit.

```glsl
// Orange — highlights (bright pixels)
if (orangeEnabled && luminance > orangeThreshold)
    color = mix(color, orangeColor.rgb, orangeOpacity);

// Green — shadows (dark pixels)
if (greenEnabled && luminance < greenThreshold)
    color = mix(color, greenColor.rgb, greenOpacity);
```

Colors are passed via `FragUBO` (vec4 at offsets 32/48, std140). This means color
can be changed from the CPU side without touching the shader.

## Extensibility

| Task | What to change |
|---|---|
| New effect (same pass) | Add fields to `EffectSettings` + `FragUBO`, extend `image.frag` |
| New effect (multi-pass) | Add intermediate `QRhiTexture` RT in `Renderer`, new `.frag` shader |
| Undo/Redo | `QUndoCommand` pushed via `AppController` |
| New theme | New `.qss` file in `assets/themes/` |
| New language | New `.ts` file in `assets/translations/` |
| Unit tests | `core/` has no UI dependency — tested directly |

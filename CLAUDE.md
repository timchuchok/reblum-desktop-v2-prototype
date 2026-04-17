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
│   ├── icons/
│   │   ├── eye-show.svg
│   │   ├── slider-thumb.svg
│   │   ├── slider-thumb-hover.svg
│   │   └── slider-thumb-disabled.svg
│   └── themes/
│       ├── dark.qss
│       └── light.qss
│
├── src/
│   ├── main.cpp
│   │
│   ├── app/                          # orchestration
│   │   ├── Application.h/cpp
│   │   ├── AppController.h/cpp       ← owns controllers + QUndoStack
│   │   ├── ImageController.h/cpp     ← async image load via QtConcurrent
│   │   ├── EffectsController.h/cpp   ← opacity, threshold, enabled
│   │   └── ThemeManager.h/cpp        ← dark / light QSS themes
│   │
│   ├── core/                         # business logic (no Qt UI)
│   │   ├── image/
│   │   │   └── ImageModel.h          ← struct { QImage; QString path, fileName; }
│   │   ├── effects/
│   │   │   └── EffectSettings.h      ← struct { float opacity, threshold; bool enabled; QColor color; }
│   │   └── viewport/
│   │       └── ViewState.h           ← struct { float zoom; QPointF offset; }
│   │
│   ├── rendering/                    # RHI pipeline
│   │   └── Renderer.h/cpp            ← setImage, setEffects, render(cb, rt, rect, dpr)
│   │
│   └── ui/                           # Qt Widgets
│       ├── main_window/
│       │   └── MainWindow.h/cpp      ← builds UI, handles DnD, owns controllers
│       ├── panels/
│       │   ├── RightPanel.h/cpp
│       │   └── EffectPanel.h/cpp     ← single class for both Orange and Green
│       └── widgets/
│           ├── ImageView.h/cpp       ← QRhiWidget: events + viewport, owns Renderer
│           ├── GradientSlider.h/cpp
│           └── EyeButton.h/cpp
│
└── resources/
    └── resources.qrc
```

## Responsibilities

### AppController
- Owns `ImageController`, `EffectsController`, `QUndoStack`
- Wires inter-controller signals (e.g., `imageLoaded` → reset effects)
- Does not touch widgets

### MainWindow
- Builds the UI
- Owns `AppController`
- Handles drag-and-drop at window level (QRhiWidget's Metal layer absorbs drops on macOS)
- Wires UI signals to controllers and controller signals to UI

```cpp
auto* ic = m_appController->imageController();
auto* ec = m_appController->effectsController();
connect(ic, &ImageController::imageLoaded, this, &MainWindow::onImageLoaded);
```

### ImageController
```cpp
void loadImage(const QString& path);   // async via QtConcurrent::run
```

Zoom and pan are handled directly in `ImageView` — `ViewState` lives there.

### EffectsController
```cpp
void setOrangeOpacity(float opacity);
void setOrangeThreshold(float threshold);
void setOrangeEnabled(bool enabled);
void setGreenOpacity(float opacity);
void setGreenThreshold(float threshold);
void setGreenEnabled(bool enabled);
```

### ImageView (QRhiWidget)
- Owns `Renderer` and `ViewState`
- Handles pan (mouse drag), zoom (wheel + pinch gesture), resize
- Computes `imageRect()` in logical pixels → passes to `Renderer`

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
| Undo/Redo | Push `QUndoCommand` via `AppController` (already owns `QUndoStack`) |
| New theme | New `.qss` file in `assets/themes/`, register in `ThemeManager` |
| Localization | New `.ts` file in `assets/translations/`, load via `QTranslator` in `Application` |
| Unit tests | `core/` has no UI dependency — test directly with Qt Test or Catch2 |

## Future: Localization

Localization is not yet implemented but the architecture supports it cleanly.

**Planned structure:**
```
assets/translations/
    app_en.ts   ← source strings (English)
    app_uk.ts   ← Ukrainian
```

**How to wire it** — in `Application` constructor, load the translator before any window is created:
```cpp
QTranslator translator;
if (translator.load(":/translations/app_uk")) {
    installTranslator(&translator);
}
```

**Scope** — only user-visible strings in `src/ui/` need `tr()`. Core structs and `Renderer` have no user-visible strings. `ThemeManager` path strings are internal — no changes needed there.

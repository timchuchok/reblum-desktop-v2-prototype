#pragma once
#include <QtCore/qglobal.h>

namespace MacOSHelper {
// Sets NSWindow.backgroundColor (prevents white at window edges during resize).
void setWindowBackground(quintptr winId, bool dark);
// Sets a view's layer background (call after QRhiWidget::initialize so the
// CAMetalLayer exists; prevents white flashes between Metal frames).
void setViewBackground(quintptr viewId, bool dark);
}  // namespace MacOSHelper

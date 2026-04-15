#pragma once

#include <QPointF>

struct ViewState {
    float zoom = 1.0f;
    QPointF offset = {0.0f, 0.0f};
};

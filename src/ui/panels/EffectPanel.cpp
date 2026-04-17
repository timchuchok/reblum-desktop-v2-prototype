#include "EffectPanel.h"

#include <QEasingCurve>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "ui/widgets/EyeButton.h"
#include "ui/widgets/GradientSlider.h"

EffectPanel::EffectPanel(EffectType type, QWidget* parent) : QWidget(parent) {
    const bool isOrange = (type == EffectType::Orange);
    const QColor color =
        isOrange ? QColor(0xc8, 0x62, 0x2a) : QColor(0x3a, 0x8a, 0x50);
    const QString name = isOrange ? "Orange" : "Green";

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // ── Header (click outside eye button → collapse) ──────
    m_header = new QWidget(this);
    m_header->setCursor(Qt::PointingHandCursor);
    m_header->installEventFilter(this);

    auto* headerLayout = new QHBoxLayout(m_header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(6);

    m_eyeBtn = new EyeButton(m_header);
    auto* nameLabel = new QLabel(name, m_header);
    nameLabel->setObjectName("EffectName");

    headerLayout->addWidget(m_eyeBtn);
    headerLayout->addWidget(nameLabel);
    headerLayout->addStretch();

    // ── Body (sliders — collapsible) ──────────────────────
    m_body = new QWidget(this);
    auto* bodyLayout = new QVBoxLayout(m_body);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(6);

    auto* opacityLabel = new QLabel("Opacity", m_body);
    opacityLabel->setObjectName("SliderLabel");

    m_opacitySlider = new GradientSlider(QColor(40, 40, 40), color, m_body);

    auto* thresholdLabel = new QLabel("Threshold", m_body);
    thresholdLabel->setObjectName("SliderLabel");

    // Orange threshold: dark→light (highlights = bright pixels, right side)
    // Green  threshold: light→dark (shadows  = dark  pixels, left  side)
    m_thresholdSlider =
        isOrange
            ? new GradientSlider(QColor(20, 20, 20), QColor(180, 180, 180),
                                  m_body)
            : new GradientSlider(QColor(180, 180, 180), QColor(20, 20, 20),
                                  m_body);

    bodyLayout->addWidget(opacityLabel);
    bodyLayout->addWidget(m_opacitySlider);
    bodyLayout->addWidget(thresholdLabel);
    bodyLayout->addWidget(m_thresholdSlider);

    layout->addWidget(m_header);
    layout->addWidget(m_body);

    // ── Animation ─────────────────────────────────────────
    m_animation = new QPropertyAnimation(m_body, "maximumHeight", this);
    m_animation->setDuration(200);
    m_animation->setEasingCurve(QEasingCurve::InOutCubic);

    connect(m_animation, &QPropertyAnimation::finished, this, [this]() {
        if (m_collapsed) {
            m_body->setVisible(false);
        } else {
            m_body->setMaximumHeight(QWIDGETSIZE_MAX);
        }
    });

    connect(m_opacitySlider, &GradientSlider::valueChanged, this,
            &EffectPanel::opacityChanged);
    connect(m_thresholdSlider, &GradientSlider::valueChanged, this,
            &EffectPanel::thresholdChanged);

    // Eye button: hold to temporarily disable (compare mode), release to restore
    connect(m_eyeBtn, &QAbstractButton::pressed, this,
            [this]() { emit enabledChanged(false); });
    connect(m_eyeBtn, &QAbstractButton::released, this,
            [this]() { emit enabledChanged(true); });
}

bool EffectPanel::eventFilter(QObject* obj, QEvent* e) {
    if (obj == m_header && e->type() == QEvent::MouseButtonRelease) {
        // Only collapse if the click wasn't on the eye button itself
        auto* me = static_cast<QMouseEvent*>(e);
        if (!m_eyeBtn->geometry().contains(me->pos())) {
            setCollapsed(!m_collapsed);
            return true;
        }
    }
    return QWidget::eventFilter(obj, e);
}

void EffectPanel::setCollapsed(bool collapsed) {
    if (m_collapsed == collapsed) {
        return;
    }
    m_collapsed = collapsed;

    m_animation->stop();
    if (collapsed) {
        m_animation->setStartValue(m_body->height());
        m_animation->setEndValue(0);
        m_animation->start();
    } else {
        m_body->setMaximumHeight(0);
        m_body->setVisible(true);
        m_animation->setStartValue(0);
        m_animation->setEndValue(m_body->sizeHint().height());
        m_animation->start();
    }
}

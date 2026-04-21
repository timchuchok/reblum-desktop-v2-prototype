#include "EffectPanel.h"

#include <QEasingCurve>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
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

    // Precompute chevron pixmaps — QIcon handles DPR automatically
    const QSize iconSz(20, 20);
    m_pixRight      = QIcon(":/icons/caret-right.svg").pixmap(iconSz);
    m_pixRightHover = QIcon(":/icons/caret-right-hovered.svg").pixmap(iconSz);
    m_pixUp         = QIcon(":/icons/caret-up.svg").pixmap(iconSz);

    // ── Outer layout ──────────────────────────────────────────
    // Right margin = 0 so header (266px) and body (262px) set their own.
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 16);
    layout->setSpacing(12);

    setCursor(Qt::PointingHandCursor);

    // ── Header (266px wide) ───────────────────────────────────
    m_header = new QWidget(this);
    m_header->installEventFilter(this);

    auto* headerLayout = new QHBoxLayout(m_header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(6);

    m_eyeBtn = new EyeButton(m_header);
    auto* nameLabel = new QLabel(name, m_header);
    nameLabel->setObjectName("EffectName");
    nameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_chevron = new QLabel(m_header);
    m_chevron->setFixedSize(20, 20);
    m_chevron->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(m_eyeBtn);
    headerLayout->addWidget(nameLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_chevron);

    // ── Body (sliders — collapsible) ─────────────────────────
    m_body = new QWidget(this);
    auto* bodyLayout = new QVBoxLayout(m_body);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(12);

    auto makeSliderRow = [&](const QString& labelText) -> QLabel* {
        auto* row = new QWidget(m_body);
        auto* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 7, 0);
        rowLayout->setSpacing(0);

        auto* label = new QLabel(labelText, row);
        label->setObjectName("SliderLabel");

        auto* valueLabel = new QLabel("0", row);
        valueLabel->setObjectName("SliderLabel");

        rowLayout->addWidget(label);
        rowLayout->addStretch();
        rowLayout->addWidget(valueLabel);
        bodyLayout->addWidget(row);
        return valueLabel;
    };

    auto* opacityValue = makeSliderRow("Opacity");
    m_opacitySlider = isOrange
                          ? new GradientSlider(QColor(0x81, 0x29, 0x40),
                                               QColor(0xD7, 0x4C, 0x2E), m_body)
                          : new GradientSlider(QColor(0x30, 0x82, 0x59),
                                               QColor(0x1C, 0xE3, 0xB4), m_body);
    bodyLayout->addWidget(m_opacitySlider);

    auto* thresholdValue = makeSliderRow("Threshold");
    // Orange threshold: dark→light (highlights = bright pixels, right side)
    // Green  threshold: light→dark (shadows  = dark  pixels, left  side)
    m_thresholdSlider = isOrange
                            ? new GradientSlider(QColor(0x36, 0x36, 0x36),
                                                 QColor(0xB5, 0xB5, 0xB5), m_body)
                            : new GradientSlider(QColor(0xC1, 0xC1, 0xC1),
                                                 QColor(0x3B, 0x3B, 0x3B), m_body);
    bodyLayout->addWidget(m_thresholdSlider);

    layout->addWidget(m_header);
    layout->addWidget(m_body);

    // ── Animation (click outside eye button → collapse) ───────
    m_animation = new QVariantAnimation(this);
    m_animation->setDuration(200);
    m_animation->setEasingCurve(QEasingCurve::InOutCubic);

    connect(
        m_animation, &QVariantAnimation::valueChanged, this,
        [this](const QVariant& v) {
            const int bodyH = v.toInt();
            m_body->setMinimumHeight(bodyH);
            m_body->setMaximumHeight(bodyH);
            const QMargins cm = this->layout()->contentsMargins();
            const int sp = bodyH > 0 ? this->layout()->spacing() : 0;
            setFixedHeight(m_header->height() + sp + bodyH + cm.top() + cm.bottom());
        });

    connect(m_animation, &QVariantAnimation::finished, this, [this]() {
        if (!m_collapsed) {
            m_body->setMinimumHeight(0);
            m_body->setMaximumHeight(QWIDGETSIZE_MAX);
            setMinimumHeight(0);
            setMaximumHeight(QWIDGETSIZE_MAX);
        }
    });

    connect(m_opacitySlider, &GradientSlider::valueChanged, this,
            &EffectPanel::opacityChanged);
    connect(m_thresholdSlider, &GradientSlider::valueChanged, this,
            &EffectPanel::thresholdChanged);

    connect(m_opacitySlider, &GradientSlider::valueChanged, this,
            [opacityValue](float v) {
                opacityValue->setText(QString::number(qRound(v * 100)));
            });
    connect(m_thresholdSlider, &GradientSlider::valueChanged, this,
            [thresholdValue](float v) {
                thresholdValue->setText(QString::number(qRound(v * 100)));
            });

    // Eye button: hold to temporarily disable (compare mode), release to
    // restore
    connect(m_eyeBtn, &QAbstractButton::pressed, this,
            [this]() { emit enabledChanged(false); });
    connect(m_eyeBtn, &QAbstractButton::released, this,
            [this]() { emit enabledChanged(true); });

    m_header->setFixedHeight(m_header->sizeHint().height());
    updateChevron();
}

bool EffectPanel::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_header) {
        if (event->type() == QEvent::Enter) {
            m_headerHovered = true;
            updateChevron();
        } else if (event->type() == QEvent::Leave) {
            m_headerHovered = false;
            updateChevron();
        }
    }
    return QWidget::eventFilter(obj, event);
}

void EffectPanel::updateChevron() {
    if (m_collapsed) {
        m_chevron->setPixmap(m_headerHovered ? m_pixRightHover : m_pixRight);
    } else {
        m_chevron->setPixmap(m_pixUp);
    }
}

void EffectPanel::mousePressEvent(QMouseEvent* event) { event->accept(); }

void EffectPanel::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setCollapsed(!m_collapsed);
    }
    QWidget::mouseReleaseEvent(event);
}

void EffectPanel::setCollapsed(bool collapsed) {
    if (m_collapsed == collapsed) {
        return;
    }
    m_collapsed = collapsed;
    updateChevron();

    m_animation->stop();
    m_animation->setStartValue(m_body->height());
    m_animation->setEndValue(collapsed ? 0 : m_body->sizeHint().height());
    m_animation->start();
}

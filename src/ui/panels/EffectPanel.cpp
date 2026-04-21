#include "EffectPanel.h"

#include <QEasingCurve>
#include <QEvent>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>
#include <QVBoxLayout>

#include "ui/widgets/EyeButton.h"
#include "ui/widgets/GradientSlider.h"

static QPixmap colorize(const QString& path, const QColor& color) {
    QSvgRenderer renderer(path);
    QImage img(20, 20, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p(&img);
    renderer.render(&p);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(img.rect(), color);
    return QPixmap::fromImage(img);
}

EffectPanel::EffectPanel(EffectType type, QWidget* parent) : QWidget(parent) {
    const bool isOrange = (type == EffectType::Orange);
    const QColor color =
        isOrange ? QColor(0xc8, 0x62, 0x2a) : QColor(0x3a, 0x8a, 0x50);
    const QString name = isOrange ? "Orange" : "Green";

    // Precompute chevron pixmaps
    m_pixRightNormal = colorize(":/icons/caret-right.svg", QColor("#747477"));
    m_pixRightHover  = colorize(":/icons/caret-right.svg", QColor("#CACACA"));
    m_pixUpNormal    = colorize(":/icons/caret-up.svg",    QColor("#747477"));
    m_pixUpHover     = colorize(":/icons/caret-up.svg",    QColor("#CACACA"));

    // ── Outer layout ──────────────────────────────────────────
    // Right margin = 0 so header (266px) and body (262px) set their own.
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 0, 0, 16);
    layout->setSpacing(0);

    setCursor(Qt::PointingHandCursor);

    // ── Header (266px wide) ───────────────────────────────────
    m_header = new QWidget(this);
    m_header->installEventFilter(this);

    auto* headerLayout = new QHBoxLayout(m_header);
    headerLayout->setContentsMargins(0, 0, 15, 0);
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

    // ── Body (262px wide, collapsible) ────────────────────────
    m_body = new QWidget(this);
    auto* bodyLayout = new QVBoxLayout(m_body);
    bodyLayout->setContentsMargins(0, 8, 19, 0);
    bodyLayout->setSpacing(6);

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
    m_opacitySlider = new GradientSlider(QColor(40, 40, 40), color, m_body);
    bodyLayout->addWidget(m_opacitySlider);

    auto* thresholdValue = makeSliderRow("Threshold");
    m_thresholdSlider = isOrange
                            ? new GradientSlider(QColor(20, 20, 20),
                                                 QColor(180, 180, 180), m_body)
                            : new GradientSlider(QColor(180, 180, 180),
                                                 QColor(20, 20, 20), m_body);
    bodyLayout->addWidget(m_thresholdSlider);

    layout->addWidget(m_header);
    layout->addWidget(m_body);

    // ── Animation ─────────────────────────────────────────────
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
            setFixedHeight(m_header->height() + bodyH + cm.top() + cm.bottom());
        });

    connect(m_animation, &QVariantAnimation::finished, this, [this]() {
        if (!m_collapsed) {
            m_body->setMinimumHeight(0);
            m_body->setMaximumHeight(QWIDGETSIZE_MAX);
        }
        setMinimumHeight(0);
        setMaximumHeight(QWIDGETSIZE_MAX);
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
        m_chevron->setPixmap(m_headerHovered ? m_pixRightHover : m_pixRightNormal);
    } else {
        m_chevron->setPixmap(m_headerHovered ? m_pixUpHover : m_pixUpNormal);
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

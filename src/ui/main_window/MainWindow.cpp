#include "MainWindow.h"

#include <QActionGroup>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImageReader>
#include <QMenuBar>
#include <QMimeData>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "app/Application.h"
#include "app/ThemeManager.h"
#include "ui/panels/RightPanel.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Reblum");
    setMinimumSize(1000, 650);
    resize(1280, 800);
    setAcceptDrops(true);

    m_imageController = new ImageController(this);
    m_effectsController = new EffectsController(this);

    setupUi();
    setupMenu();
    connectSignals();
}

void MainWindow::setupUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Top bar ───────────────────────────────────────────
    auto* topBar = new QWidget(central);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(36);

    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(12, 0, 12, 0);
    topLayout->setSpacing(8);

    m_metaLabel = new QLabel("", topBar);
    m_metaLabel->setObjectName("ImageMeta");

    m_fileLabel = new QLabel("", topBar);
    m_fileLabel->setObjectName("FileName");

    auto* trialLabel = new QLabel("Free trial — 3 days left", topBar);
    trialLabel->setObjectName("TrialBadge");

    topLayout->addWidget(m_metaLabel);
    topLayout->addSpacing(12);
    topLayout->addWidget(m_fileLabel);
    topLayout->addStretch();
    topLayout->addWidget(trialLabel);

    // ── Content (image view + right panel) ───────────────
    auto* content = new QWidget(central);
    auto* contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    m_imageView = new ImageView(content);
    auto* rightPanel = new RightPanel(m_effectsController, content);

    contentLayout->addWidget(m_imageView, 1);
    contentLayout->addWidget(rightPanel);

    // ── Bottom bar ────────────────────────────────────────
    auto* bottomBar = new QWidget(central);
    bottomBar->setObjectName("BottomBar");
    bottomBar->setFixedHeight(36);

    auto* bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(12, 0, 12, 0);
    bottomLayout->setSpacing(4);

    auto* fitBtn = new QPushButton("Fit", bottomBar);
    fitBtn->setObjectName("ZoomFit");
    fitBtn->setCheckable(true);
    fitBtn->setChecked(true);

    auto* zoom100Btn = new QPushButton("100%", bottomBar);
    zoom100Btn->setObjectName("Zoom100");
    zoom100Btn->setCheckable(true);

    connect(fitBtn, &QPushButton::clicked, this, [=]() {
        m_imageView->fitToView();
        fitBtn->setChecked(true);
        zoom100Btn->setChecked(false);
    });

    connect(zoom100Btn, &QPushButton::clicked, this, [=]() {
        m_imageView->setZoom100();
        zoom100Btn->setChecked(true);
        fitBtn->setChecked(false);
    });

    auto* exportBtn = new QPushButton("↑ Export", bottomBar);
    exportBtn->setObjectName("ExportButton");

    bottomLayout->addWidget(fitBtn);
    bottomLayout->addWidget(zoom100Btn);
    bottomLayout->addStretch();
    bottomLayout->addWidget(exportBtn);

    // ── Assemble ──────────────────────────────────────────
    root->addWidget(topBar);
    root->addWidget(content, 1);
    root->addWidget(bottomBar);
}

void MainWindow::setupMenu() {
    auto* fileMenu = menuBar()->addMenu(tr("File"));
    auto* openAction = fileMenu->addAction(tr("Open Image…"));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openImage);

    auto* themeMenu = menuBar()->addMenu(tr("Theme"));
    auto* themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);

    auto* darkAction = themeMenu->addAction(tr("Dark"));
    darkAction->setCheckable(true);
    darkAction->setChecked(true);
    themeGroup->addAction(darkAction);

    auto* lightAction = themeMenu->addAction(tr("Light"));
    lightAction->setCheckable(true);
    themeGroup->addAction(lightAction);

    auto* tm = static_cast<Application*>(qApp)->themeManager();
    darkAction->setChecked(tm->current() == ThemeManager::Theme::Dark);
    lightAction->setChecked(tm->current() == ThemeManager::Theme::Light);

    connect(darkAction, &QAction::triggered, this,
            [tm]() { tm->apply(ThemeManager::Theme::Dark); });
    connect(lightAction, &QAction::triggered, this,
            [tm]() { tm->apply(ThemeManager::Theme::Light); });
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (!event->mimeData()->hasUrls()) {
        return;
    }
    for (const QUrl& url : event->mimeData()->urls()) {
        if (url.isLocalFile() &&
            !QImageReader::imageFormat(url.toLocalFile()).isEmpty()) {
            event->acceptProposedAction();
            return;
        }
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    for (const QUrl& url : event->mimeData()->urls()) {
        if (url.isLocalFile() &&
            !QImageReader::imageFormat(url.toLocalFile()).isEmpty()) {
            m_imageController->loadImage(url.toLocalFile());
            event->acceptProposedAction();
            return;
        }
    }
}

void MainWindow::connectSignals() {
    connect(m_imageController, &ImageController::imageLoaded, this,
            &MainWindow::onImageLoaded);

    auto syncEffects = [this]() {
        m_imageView->setEffects(m_effectsController->orangeSettings(),
                                m_effectsController->greenSettings());
    };
    connect(m_effectsController, &EffectsController::orangeSettingsChanged,
            this, [syncEffects](const EffectSettings&) { syncEffects(); });
    connect(m_effectsController, &EffectsController::greenSettingsChanged, this,
            [syncEffects](const EffectSettings&) { syncEffects(); });
}

void MainWindow::openImage() {
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Open Image"), {},
        tr("Images (*.png *.jpg *.jpeg *.tiff *.bmp)"));
    if (!path.isEmpty()) {
        m_imageController->loadImage(path);
    }
}

void MainWindow::onImageLoaded(const ImageModel& model) {
    m_imageView->setImage(model);
    m_fileLabel->setText(model.fileName);
    m_metaLabel->setText(QString("%1 × %2 px")
                             .arg(model.size().width())
                             .arg(model.size().height()));
}

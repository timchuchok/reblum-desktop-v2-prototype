#include "MainWindow.h"

#include <QActionGroup>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QImageReader>
#include <QMenuBar>
#include <QMimeData>
#include <QMouseEvent>
#include <QPushButton>
#include <QSizeGrip>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>

#include "app/Application.h"
#include "app/ThemeManager.h"
#include "ui/panels/RightPanel.h"
#ifdef Q_OS_MACOS
#include "platform/MacOSHelper.h"
#endif

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Reblum");
    setMinimumSize(1000, 650);
    resize(1280, 800);

    setAcceptDrops(true);

    m_appController = new AppController(this);

    setupUi();
    setupMenu();
    connectSignals();
}

void MainWindow::setupUi() {
    // ── Central widget ─────────────────────────────────────
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Top bar (custom title bar) ─────────────────────────
    m_topBar = new QWidget(central);
    m_topBar->setObjectName("MainToolbar");
    m_topBar->setFixedHeight(36);

    auto* topLayout = new QHBoxLayout(m_topBar);
    topLayout->setContentsMargins(80, 0, 12, 0);
    topLayout->setSpacing(4);

    m_metaLabel = new QLabel("", m_topBar);
    m_metaLabel->setObjectName("ImageMeta");

    m_fileLabel = new QLabel("", m_topBar);
    m_fileLabel->setObjectName("FileName");

    auto* midSpacer = new QWidget(m_topBar);
    midSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* trialLabel = new QLabel("Free trial — 3 days left", m_topBar);
    trialLabel->setObjectName("TrialBadge");

    topLayout->addWidget(m_metaLabel);
    topLayout->addWidget(m_fileLabel);
    topLayout->addWidget(midSpacer);
    topLayout->addWidget(trialLabel);

    // Install event filter for window drag on topBar and all its children
    m_topBar->installEventFilter(this);
    for (auto* child : m_topBar->findChildren<QWidget*>()) {
        child->installEventFilter(this);
    }

    root->addWidget(m_topBar);
    auto makeSep = [central]() -> QFrame* {
        auto* sep = new QFrame(central);
        sep->setFrameShape(QFrame::HLine);
        sep->setObjectName("PanelSeparator");
        return sep;
    };
    root->addWidget(makeSep());

    // ── Content (image view + right panel) ────────────────
    auto* content = new QWidget(central);
    auto* contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    m_imageView = new ImageView(content);
    m_imageView->setObjectName("ImageArea");
    auto* rightPanel =
        new RightPanel(m_appController->effectsController(), content);

    contentLayout->addWidget(m_imageView, 1);
    contentLayout->addWidget(rightPanel);

    // ── Bottom bar ────────────────────────────────────────
    auto* bottomBar = new QWidget(central);
    bottomBar->setObjectName("BottomBar");
    bottomBar->setFixedHeight(40);

    auto* bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(0);

    // Image section: Fit / 100% aligned to the right edge of the canvas
    auto* imageSection = new QWidget(bottomBar);
    auto* imageSectionLayout = new QHBoxLayout(imageSection);
    imageSectionLayout->setContentsMargins(12, 0, 12, 0);
    imageSectionLayout->setSpacing(4);

    auto* fitBtn = new QPushButton("Fit", imageSection);
    fitBtn->setObjectName("ZoomFit");
    fitBtn->setCheckable(true);
    fitBtn->setChecked(true);

    auto* zoom100Btn = new QPushButton("100%", imageSection);
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

    imageSectionLayout->addStretch();
    imageSectionLayout->addWidget(fitBtn);
    imageSectionLayout->addWidget(zoom100Btn);

    // Right panel section: Export button, matches right panel width
    auto* rightSection = new QWidget(bottomBar);
    rightSection->setFixedWidth(293);
    auto* rightSectionLayout = new QHBoxLayout(rightSection);
    rightSectionLayout->setContentsMargins(12, 6, 12, 6);
    rightSectionLayout->setSpacing(0);

    auto* exportBtn = new QPushButton("Export", rightSection);
    exportBtn->setObjectName("ExportButton");
    exportBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    rightSectionLayout->addWidget(exportBtn);

    auto* bottomVSep = new QFrame(bottomBar);
    bottomVSep->setFrameShape(QFrame::VLine);
    bottomVSep->setObjectName("PanelSeparatorV");

    bottomLayout->addWidget(imageSection, 1);
    bottomLayout->addWidget(bottomVSep);
    bottomLayout->addWidget(rightSection);

    // ── Assemble ───────────────────────────────────────────
    root->addWidget(content, 1);
    root->addWidget(makeSep());
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
    auto* lightAction = themeMenu->addAction(tr("Light"));
    lightAction->setCheckable(true);
    themeGroup->addAction(darkAction);
    themeGroup->addAction(lightAction);

    auto* tm = static_cast<Application*>(qApp)->themeManager();
    darkAction->setChecked(tm->current() == ThemeManager::Theme::Dark);
    lightAction->setChecked(tm->current() == ThemeManager::Theme::Light);

    connect(darkAction, &QAction::triggered, this,
            [tm]() { tm->apply(ThemeManager::Theme::Dark); });
    connect(lightAction, &QAction::triggered, this,
            [tm]() { tm->apply(ThemeManager::Theme::Light); });
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
#ifdef Q_OS_MACOS
    auto* tm = static_cast<Application*>(qApp)->themeManager();
    const bool dark = tm->current() == ThemeManager::Theme::Dark;
    MacOSHelper::setupFullSizeTitleBar(winId(), dark);
    MacOSHelper::setWindowBackground(winId(), dark);
#endif
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    auto* widget = qobject_cast<QWidget*>(obj);
    const bool inTopBar =
        widget && (widget == m_topBar || m_topBar->isAncestorOf(widget));

    if (inTopBar) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton &&
                !qobject_cast<QPushButton*>(obj)) {
                windowHandle()->startSystemMove();
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
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
            m_appController->imageController()->loadImage(url.toLocalFile());
            event->acceptProposedAction();
            return;
        }
    }
}

void MainWindow::connectSignals() {
    auto* ic = m_appController->imageController();
    auto* ec = m_appController->effectsController();

    connect(ic, &ImageController::imageLoaded, this,
            &MainWindow::onImageLoaded);

#ifdef Q_OS_MACOS
    auto* tm = static_cast<Application*>(qApp)->themeManager();
    connect(tm, &ThemeManager::themeChanged, this,
            [this](ThemeManager::Theme theme) {
                MacOSHelper::setWindowBackground(
                    winId(), theme == ThemeManager::Theme::Dark);
            });
#endif

    auto syncEffects = [this, ec]() {
        m_imageView->setEffects(ec->orangeSettings(), ec->greenSettings());
    };
    connect(ec, &EffectsController::orangeSettingsChanged, this,
            [syncEffects](const EffectSettings&) { syncEffects(); });
    connect(ec, &EffectsController::greenSettingsChanged, this,
            [syncEffects](const EffectSettings&) { syncEffects(); });
}

void MainWindow::openImage() {
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Open Image"), {},
        tr("Images (*.png *.jpg *.jpeg *.tiff *.bmp)"));
    if (!path.isEmpty()) {
        m_appController->imageController()->loadImage(path);
    }
}

void MainWindow::onImageLoaded(const ImageModel& model) {
    m_imageView->setImage(model);
    m_fileLabel->setText("  " + model.fileName);
    m_metaLabel->setText(QString("%1 × %2 px  ")
                             .arg(model.size().width())
                             .arg(model.size().height()));
}

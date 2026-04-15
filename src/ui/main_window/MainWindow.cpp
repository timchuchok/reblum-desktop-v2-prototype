#include "MainWindow.h"

#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Reblum");
    setMinimumSize(1000, 650);
    resize(1280, 800);
    setupUi();
}

void MainWindow::setupUi()
{
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

    auto* metaLabel = new QLabel("3440 × 5162 px", topBar);
    metaLabel->setObjectName("ImageMeta");

    auto* fileLabel = new QLabel("IMG_ROBERT-JAMES_Portrait_Retouch_Photo_Load_Edit.jpg", topBar);
    fileLabel->setObjectName("FileName");

    auto* trialLabel = new QLabel("Free trial — 3 days left", topBar);
    trialLabel->setObjectName("TrialBadge");

    topLayout->addWidget(metaLabel);
    topLayout->addSpacing(12);
    topLayout->addWidget(fileLabel);
    topLayout->addStretch();
    topLayout->addWidget(trialLabel);

    // ── Content (image area + right panel) ───────────────
    auto* content = new QWidget(central);
    auto* contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    auto* imageArea = new QWidget(content);
    imageArea->setObjectName("ImageArea");

    auto* rightPanel = new QWidget(content);
    rightPanel->setObjectName("RightPanel");
    rightPanel->setFixedWidth(200);

    auto* placeholder = new QLabel("Orange\n——\nGreen", rightPanel);
    placeholder->setObjectName("PanelPlaceholder");
    placeholder->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    auto* rpLayout = new QVBoxLayout(rightPanel);
    rpLayout->setContentsMargins(12, 16, 12, 16);
    rpLayout->addWidget(placeholder);
    rpLayout->addStretch();

    contentLayout->addWidget(imageArea, 1);
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

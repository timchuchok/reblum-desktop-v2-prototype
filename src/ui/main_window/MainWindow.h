#pragma once

#include <QLabel>
#include <QMainWindow>

#include <QTimer>

#include "app/AppController.h"
#include "ui/widgets/ImageView.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

protected:
    void showEvent(QShowEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUi();
    void setupMenu();
    void connectSignals();
    void openImage();
    void onImageLoaded(const ImageModel& model);

    AppController* m_appController = nullptr;
    ImageView* m_imageView = nullptr;
    QLabel* m_metaLabel = nullptr;
    QLabel* m_fileLabel = nullptr;
    QLabel* m_fpsLabel = nullptr;
    QLabel* m_guiFpsLabel = nullptr;
    int m_guiPaintCount = 0;
    QTimer* m_guiFpsTimer = nullptr;
    QWidget* m_metaBadge = nullptr;
    QWidget* m_topBar = nullptr;
    bool m_titleBarConfigured = false;
};

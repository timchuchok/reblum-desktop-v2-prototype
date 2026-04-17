#pragma once

#include <QLabel>
#include <QMainWindow>

#include "app/AppController.h"
#include "ui/widgets/ImageView.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

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
};

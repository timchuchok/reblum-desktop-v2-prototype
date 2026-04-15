#pragma once

#include <QLabel>
#include <QMainWindow>

#include "app/ImageController.h"
#include "ui/widgets/ImageView.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    void setupUi();
    void setupMenu();
    void connectSignals();
    void openImage();
    void onImageLoaded(const ImageModel& model);

    ImageController* m_imageController = nullptr;
    ImageView* m_imageView = nullptr;
    QLabel* m_metaLabel = nullptr;
    QLabel* m_fileLabel = nullptr;
};

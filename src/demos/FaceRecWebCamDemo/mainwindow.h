#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <opencv2/opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_captureButton_clicked();

    void on_addToGalleryButton_clicked();

    void on_identifyButton_clicked();

private:
    Ui::MainWindow *ui;
    QProgressBar* progressBar;
    std::vector<cv::Mat> faceTemplatesBuffer;
    std::map< QString, std::vector<cv::Mat> > gallery;

    double calculateReferenceMaxDistanceAndPosition(cv::VideoCapture cap, cv::CascadeClassifier frontal_face_cascade, cv::Point2d &referecePosition);
};

#endif // MAINWINDOW_H

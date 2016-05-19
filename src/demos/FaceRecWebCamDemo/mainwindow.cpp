#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "objectRecognition/FaceProcessor.h"
#include "tools/MatrixTools.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <QInputDialog>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->progressBar = new QProgressBar(ui->statusBar);
    this->progressBar->setAlignment(Qt::AlignRight);
    this->progressBar->setMaximumSize(180, 19);
    ui->statusBar->addPermanentWidget(this->progressBar);
    this->statusBar()->showMessage(tr("Ready"), 2000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete this->progressBar;
}

void MainWindow::on_captureButton_clicked()
{
    this->faceTemplatesBuffer.clear();
    this->ui->addToGalleryButton->setEnabled(false);
    this->ui->identifyButton->setEnabled(false);

    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier eyes_cascade;
    cv::CascadeClassifier frontal_face_cascade;

    string frontal_face_cascade_name = "D:\\Dependencies\\opencv2.4.3.6.0\\data\\lbpcascades\\lbpcascade_frontalface.xml";
    string face_cascade_name = "D:\\Dependencies\\opencv2.4.3.6.0\\data\\haarcascades\\haarcascade_frontalface_default.xml";
    string eyes_cascade_name = "D:\\Dependencies\\opencv2.4.3.6.0\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";

    //-- 1. Load the cascade
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); };
    if( !frontal_face_cascade.load( frontal_face_cascade_name ) ){ printf("--(!)Error loading body cascade\n"); };

    cv::VideoCapture cap(0);
    int facesFound = 0;
    this->progressBar->setMinimum(0);
    this->progressBar->setMaximum(20);

    cv::Point2d previousPosition;
    double maxDistance = this->calculateReferenceMaxDistanceAndPosition(cap, frontal_face_cascade, previousPosition);
    int framesSkipped = 0;
    while (facesFound < 20)
    {
        cv::Mat frame;
        cv::Mat face;
        cv::Mat normalizedFace;

        if(framesSkipped > 10)
        {
            maxDistance = MainWindow::calculateReferenceMaxDistanceAndPosition( cap, frontal_face_cascade, previousPosition );
            framesSkipped = 0;
        }

        cap >> frame;
        cv::Mat frame_gray;

        cv::cvtColor( frame, frame_gray, cv::COLOR_BGR2GRAY );
        cv::equalizeHist( frame_gray, frame_gray );

        std::vector<cv::Rect> frontal_faces;
        frontal_face_cascade.detectMultiScale(frame_gray, frontal_faces);
        if (frontal_faces.size() > 0)
        {
            double distance = cv::norm(cv::Mat(previousPosition),cv::Mat(cv::Point2d(frontal_faces[0].x,frontal_faces[0].y)));
            if (distance < maxDistance)
            {
                frame_gray(frontal_faces[0]).copyTo(face);

                cv::resize(face,normalizedFace,cv::Size(170,170),0,0,CV_INTER_AREA);

                rectangle(frame,frontal_faces[0],cv::Scalar(0,255,0));

                cv::Mat faceTemplate = FaceProcessor::GetOrientationsBinaryTemplate(normalizedFace);
                faceTemplatesBuffer.push_back(faceTemplate);
                facesFound++;
                this->progressBar->setValue(facesFound);
            }
            else
            {
                framesSkipped++;
            }
        }

        QImage image = QImage((const uchar *)frame.data, frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped();
        ui->imageDisplay->setPixmap(QPixmap::fromImage(image));

        qApp->processEvents();
    }
    this->ui->addToGalleryButton->setEnabled(true);
    this->ui->identifyButton->setEnabled(true);
}

void MainWindow::on_addToGalleryButton_clicked()
{
    QString name = QInputDialog::getText(this,"Add to gallery","Name:");

    if (!this->faceTemplatesBuffer.empty())
    {
        std::map<QString, vector<cv::Mat> >::iterator it = this->gallery.find(name);
        if (it != this->gallery.end())
        {
            it->second.insert(it->second.end(),this->faceTemplatesBuffer.begin(),this->faceTemplatesBuffer.end());
        }
        else
        {
            this->gallery.insert(std::pair<QString, vector<cv::Mat> >(name,this->faceTemplatesBuffer));
        }
    }
}

void MainWindow::on_identifyButton_clicked()
{
    int minAverageDistance = std::numeric_limits<int>::max();
    QString nameMinDistance("Match not found");
    if (!this->faceTemplatesBuffer.empty())
    {
        for(map<QString,std::vector<cv::Mat> >::iterator it = this->gallery.begin(); it != this->gallery.end(); it++)
        {
            int count = 0;
            int sum = 0;

            for (unsigned int i = 0; i < it->second.size(); i++)
            {

                for (vector<cv::Mat>::iterator bufferIt = this->faceTemplatesBuffer.begin(); bufferIt != this->faceTemplatesBuffer.end(); bufferIt++)
                {
                    sum += MatrixTools::HammingDistance(it->second[i],*bufferIt);
                    count++;
                }
            }


            if (count != 0)
            {
                int average = sum/count;

                if (average < minAverageDistance)
                {

                    nameMinDistance = it->first;
                    minAverageDistance = average;
                }
            }
        }
    }

    QMessageBox msgBox;
    msgBox.setText("Best match: " + nameMinDistance);
    msgBox.exec();

}

double MainWindow::calculateReferenceMaxDistanceAndPosition(cv::VideoCapture cap,cv::CascadeClassifier frontal_face_cascade, cv::Point2d &referencePosition)
{
    // find a real face (consistent detections)
    std::vector<double> faceDistancesBuffer;
    std::vector<cv::Point2d> facePositionsBuffer;
    cv::Point2d previousPosition(0,0);
    double sumDistances = 0;
    int facesFound = 0;
    while (facesFound < 10)
    {
        cv::Mat frame;
        cap >> frame;
        cv::Mat frame_gray;
        cv::cvtColor( frame, frame_gray, cv::COLOR_BGR2GRAY );
        cv::equalizeHist( frame_gray, frame_gray );

        std::vector<cv::Rect> frontal_faces;
        frontal_face_cascade.detectMultiScale(frame_gray, frontal_faces);
        if (!frontal_faces.empty())
        {
            facesFound++;
            cv::Point2d currentPosition(frontal_faces[0].x,frontal_faces[0].y);
            facePositionsBuffer.push_back(currentPosition);
            double distance = cv::norm(cv::Mat(previousPosition),cv::Mat(currentPosition));
            sumDistances += distance;
            faceDistancesBuffer.push_back(distance);
            previousPosition = currentPosition;

            rectangle(frame,frontal_faces[0],cv::Scalar(0,255,0));
        }
        QImage image = QImage((const uchar *)frame.data, frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped();
        ui->imageDisplay->setPixmap(QPixmap::fromImage(image));
        qApp->processEvents();
    }

    double averageDistance = sumDistances/faceDistancesBuffer.size();

    int countDistancesNoOutliers = 0;
    double sumDistancesNoOutliers = 0;
    for (unsigned int i = 0; i < faceDistancesBuffer.size(); i++)
    {
        if (faceDistancesBuffer[i] < 2*averageDistance)
        {
            countDistancesNoOutliers++;
            sumDistancesNoOutliers += faceDistancesBuffer[i];
            if (i<facePositionsBuffer.size())
            {
                referencePosition = facePositionsBuffer[i];
            }
        }
    }

    double goundTruthMaxDistance = 4*(sumDistancesNoOutliers/countDistancesNoOutliers);
    return goundTruthMaxDistance;
}

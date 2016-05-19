#-------------------------------------------------
#
# Project created by QtCreator 2014-08-17T14:08:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FaceRecWebCamDemo
TEMPLATE = app


SOURCES     +=  main.cpp\
                mainwindow.cpp \
                "../../src/tools/GaborFilters.cpp" \
                "../../src/tools/MatrixTools.cpp" \
                "../../src/objectRecognition/FaceProcessor.cpp"

HEADERS     +=  mainwindow.h \
                "../../include/tools/GaborFilters.h" \
                "../../include/tools/MatrixTools.h" \
                "../../include/objectRecognition/FaceProcessor.h"

INCLUDEPATH +=  "../../include/" \
                "../../include/tools" \
                "../../include/objectRecognition" \
                "D:\\Dependencies\\boost_1_49_0"

FORMS    += mainwindow.ui

## OpenCV settings for Unix/Linux
unix:!mac {
    message("* Using settings for Unix/Linux.")
    INCLUDEPATH += /usr/local/include/opencv

    LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc
}

## OpenCV settings for Mac OS X
macx {
    message("* Using settings for Mac OS X.")
    INCLUDEPATH += /usr/local/include/opencv

    LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc
}

## OpenCV settings for Windows and OpenCV 2.4.3.6.0
win32 {
    message("* Using settings for Windows.")
    INCLUDEPATH += "D:\\Dependencies\\opencv2.4.3.6.0\\build\\include" \
                   "D:\\Dependencies\\opencv2.4.3.6.0\\build\\include\\opencv2"

    CONFIG(debug, debug | release) {
        LIBS += -LD:\\Dependencies\\opencv2.4.3.6.0\\build\\x86\\vc10\\lib \
            -lopencv_objdetect246d \
            -lopencv_imgproc246d \
            -lopencv_core246d \
            -lopencv_highgui246d
    }

    CONFIG(release, debug | release) {
        LIBS += -LD:\\Dependencies\\opencv2.4.3.6.0\\build\\x86\\vc10\\lib \
            -lopencv_objdetect246 \
            -lopencv_imgproc246 \
            -lopencv_core246 \
            -lopencv_highgui246


        LIBS += -LD:\\Dependencies\\boost_1_49_0\\lib\\win32 \
            -llibboost_filesystem-vc100-mt-1_49
    }
}

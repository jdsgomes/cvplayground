/* 
 * File:   main.cpp
 * Author: joao
 *
 * Created on January 22, 2014, 11:32 PM
 */
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include "filters/GaborFilters.h"
#include "features/BinaryCV1Faces.h"
#include "tools/EnvironmentTools.h"


using namespace std;
using namespace cv;

void detectAndDisplay( Mat frame );

String frontal_face_cascade_name = "../data/cascades/lbpcascade_frontalface.xml";
String face_cascade_name = "../data/cascades/haarcascade_frontalface_default.xml";
String eyes_cascade_name = "../data/cascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier frontal_face_cascade;
Ptr<FaceRecognizer> lbpRec;
int appearencesBufferSize = 30;
int appearencesThreshold=5;
int numberOfVisitors = 0;
int numberOfNewVisitors = 0;
vector<int> appearances;
vector<bool> forgoten;
vector<bool> active;
vector<Mat> gallery;
vector<int> ids;
int thr = 400;
int forget_threshold = -1000;
int gallerySize = 0;

String window_name = "Capture - Face detection";

int main(int argc, char** argv) 
{
	VideoCapture capture;
	lbpRec  = createLBPHFaceRecognizer();
	Mat frame;

	//-- 1. Load the cascade
	//if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
	//if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); return -1; };
	if( !frontal_face_cascade.load( frontal_face_cascade_name ) ){ printf("--(!)Error loading frontal face cascade\n"); return -1; };

	//-- 2. Read the video stream
	capture.open( -1 );
	if ( ! capture.isOpened() ) 
	{ 
		printf("--(!)Error opening video capture\n"); 
		return -1; 
	}

	while ( capture.read(frame) )
	{
		if( frame.empty() )
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

        //-- 3. Apply the classifier to the frame
        detectAndDisplay( frame );

		//-- bail out if escape was pressed
		int c = waitKey(10);
		if( (char)c == 27 ) { break; }
	}
	return 0;
}

/**
 * @function detectAndDisplay
 */
void detectAndDisplay( Mat frame )
{
	std::vector<Rect> faces;
	std::vector<Rect> frontal_faces;
	Mat frame_gray;

	cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	// detect faces
	vector<Mat> frame_templates;
	frontal_face_cascade.detectMultiScale(frame_gray, frontal_faces);
	for( size_t i = 0; i < frontal_faces.size(); i++ )
	{
		Scalar color(0,255,0);
		Mat face;
		frame_gray(frontal_faces[i]).copyTo(face);
		Mat normalizedFace;
		resize(face,normalizedFace,Size(170,170),0,0,CV_INTER_AREA);
		frame_templates.push_back(normalizedFace);
	}

	vector<int> matched_idxs;
		
	if (gallerySize > 0)
	{
		for (size_t j = 0; j < frame_templates.size(); j++)
		{
			int label;
			double distance;
			lbpRec->predict(frame_templates[j], label, distance);
			if (distance < thr)
			{
				matched_idxs.push_back(label);
				if (appearances[label] < 0)
				{
					appearances[label] = 0;
				}
				if (appearances[label] < appearencesBufferSize)
				{
					appearances[label]++;
					if (appearances[label] == appearencesThreshold)
					{
						if (forgoten[label])
						{
							numberOfVisitors++;
							if (!active[label])
							{
								active[label] = true;
								numberOfNewVisitors++;
							}
							forgoten[label] = false;
						}
					}
				}
			}
			else
			{
				gallery.push_back(frame_templates[j]);
				ids.push_back(gallerySize);
				lbpRec->train(gallery,ids);
				gallerySize++;
				appearances.push_back(0);
				forgoten.push_back(true);
				active.push_back(false);
			}
		}
	}
	else
	{
		for (size_t j = 0; j < frame_templates.size(); j++)
		{	
			ids.push_back(j);
			gallery.push_back(frame_templates[j]);
			gallerySize++;
			appearances.push_back(0);
			forgoten.push_back(true);
			active.push_back(false);
		}
		lbpRec->train(gallery, ids);
	}

	for (int i = 0; i <gallerySize; i++)
	{
		if (find(matched_idxs.begin(), matched_idxs.end(), i) == matched_idxs.end())
		{
			appearances[i]--;
			if (appearances[i] == 0)
			{
				forgoten[i] = true;
			}
			if (appearances[i] < forget_threshold && !active[i])
			{
				// delete false match
			}
		}
	}

	// draw faces
	for (size_t i = 0; i < frontal_faces.size(); i++)
	{
		rectangle(frame,frontal_faces[i],Scalar(0,255,0));
	}

	ostringstream ss;
	ss << "Number of visits: " << numberOfVisitors;
	ostringstream ssNew;
	ssNew << "Number of new visits: " << numberOfNewVisitors;

	putText(frame,ss.str(),Point(10,frame.rows-10),CV_FONT_HERSHEY_SIMPLEX,1.0,Scalar(0,0,255),2);
	putText(frame,ssNew.str(),Point(10,frame.rows-50),CV_FONT_HERSHEY_SIMPLEX,1.0,Scalar(0,0,255),2);
    //-- Show what you got
    imshow( window_name, frame );
}


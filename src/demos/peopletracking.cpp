#include <iostream>
#include <opencv2/opencv.hpp>

// help http://www.pyimagesearch.com/2015/11/09/pedestrian-detection-opencv/
using namespace cv;
using namespace std;

int
main(int argc, char** argv) {
	int 				k;
	int				threshold_value;
	VideoCapture 			capture;
	HOGDescriptor 			hog;
	Mat 				frame;
	Mat				gray_frame;
	Mat 				previous_frame; 
	Mat				diff_frame;
	Mat				binary_frame;
	Mat 				resizedFrame;
	SimpleBlobDetector::Params 	params;
	vector<KeyPoint> 		keypoints;
	vector<vector<Point> > 		contours;
	vector<Vec4i>			hierarchy;
	static vector<float>    	detector		=	HOGDescriptor::getDefaultPeopleDetector();
	string 				detectionWindowName 	= 	"Display window";
	string 				diffWindowName 		= 	"Diff window";

	params.thresholdStep		= 10.0f;
    	params.minThreshold		= 50.0f;
    	params.maxThreshold		= 200.0f;
	params.minDistBetweenBlobs	= 100.0f;
	params.filterByInertia		= false;
	params.filterByConvexity	= false;
	params.filterByColor		= false;
	params.filterByCircularity	= false;

	cv::SimpleBlobDetector blob_detector(params);


	threshold_value = 60;
	if (argc != 2) {
		cout << "Usage: " << argv[0] << "input_filename" << endl;
	} 
	else {
		capture.open(argv[1]);
		if ( ! capture.isOpened() ) {
	        	cerr << "Error opening video capture from file " << argv[1] << endl << "Leaving" << endl;
		} 
		else {
			if ( !capture.read(frame) )
			{
				cout << "Failled to capture frame" << endl << "Leaving" << endl;
			}
			else {
				if( frame.empty() ) {
					cout << "Failled to capture frame" << endl << "Leaving" << endl;
				}
				cvtColor(frame, previous_frame, CV_RGB2GRAY);
				namedWindow( detectionWindowName, WINDOW_AUTOSIZE );
				namedWindow( diffWindowName, WINDOW_AUTOSIZE );
				hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
			
				vector<Rect> found, found_filtered;
				while ( capture.read(frame) ) {
					if( frame.empty() ) {
						cout << "Failled to capture frame" << endl << "Leaving" << endl;
						break;
					}
					cvtColor(frame, gray_frame, CV_RGB2GRAY);
					diff_frame = gray_frame-previous_frame;
					gray_frame.copyTo(previous_frame);
					threshold( diff_frame, binary_frame, threshold_value, 255, 0 );
					//findContours(binary_frame, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

					//drawContours( frame, contours, 3 <= 0 ? 3 : -1, Scalar(128,255,255), 3, LINE_8, hierarchy, std::abs(3) );
					blob_detector.detect(diff_frame, keypoints);
						
					for( size_t i = 0; i < keypoints.size(); i++) {
						circle(frame, keypoints[i].pt, keypoints[i].size, Scalar(255,255,255));
					}					

					resize(frame, resizedFrame, Size(), 1, 1);
					frame = resizedFrame;

					//hog.detectMultiScale(frame, found, 0, Size(), Size(), 1.05, 1);
					groupRectangles(found, 0, 0.5);
			
					for (size_t i=0; i<found.size(); i++) 
					{
						Rect r = found[i];
						rectangle(frame, r.tl(), r.br(), Scalar(0,255,0), 3);
					}	

					imshow(detectionWindowName, frame);
					imshow(diffWindowName, binary_frame);
					k = waitKey(10);
					if( (char)k == 27 ) {
						break;
					}
				}
			}
		}
	}	
	return 0;
}

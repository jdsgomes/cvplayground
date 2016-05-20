#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

typedef struct object {
	Rect previousPos;
	int lastSeen;
	Vec2d momentum;
	int id;
} object_t;

void updateTrackedObjs(vector<Rect> &detections, vector<object_t> &trackedObjs, int currentFrame, int &current_id) {
	vector<size_t> matches_idxs;
	for (size_t i = 0; i < detections.size(); i++) {
		Vec<float,2> momentum_best_match;
		size_t best_match_idx = 1000000000;
		int last_seen_best_match = -1;
		for(size_t j = 0; j < trackedObjs.size(); j++) {
			int size = (detections[i].width + detections[i].height + trackedObjs[j].previousPos.width + trackedObjs[j].previousPos.height)/4;
			Vec2d current_tl(detections[i].tl().x, detections[i].tl().y);
			Vec2d current_br(detections[i].br().x, detections[i].br().y);
			Vec2d old_tl(trackedObjs[j].previousPos.tl().x, trackedObjs[j].previousPos.tl().y);
			Vec2d old_br(trackedObjs[j].previousPos.br().x, trackedObjs[j].previousPos.br().y);
	
			Vec2d distanceVec = (current_br+current_tl)/2-(old_br+old_tl)/2;
			float distance = norm(distanceVec);
			if (distance < size/2)  {
				cout << "Detected objects overlaps with existing tracked object." << endl;
				//best match (closest and most recently detected)
				if (trackedObjs[j].lastSeen > last_seen_best_match) {
					best_match_idx = j;
					last_seen_best_match = trackedObjs[best_match_idx].lastSeen;
					momentum_best_match = distanceVec;
				}
			} 
		}
		if (best_match_idx != 1000000000)
		{
			cout << "Found match, update tracked objects " << best_match_idx << endl;
			trackedObjs[best_match_idx].lastSeen = currentFrame;
			trackedObjs[best_match_idx].momentum = momentum_best_match;	
			trackedObjs[best_match_idx].previousPos = detections[i];
			matches_idxs.push_back(best_match_idx);
		} 
		else {
			object_t new_detection;
			new_detection.previousPos = detections[i];
			new_detection.lastSeen = currentFrame;
			new_detection.momentum = Vec2d(0,0);
			new_detection.id = ++current_id;
			trackedObjs.push_back(new_detection);
		}
	}
	
	cout << "Predicting positions for objects not detected in the current frame." << endl;
	for( size_t i = 0; i < trackedObjs.size(); i++ ) {
		if (trackedObjs[i].lastSeen < currentFrame) {
			trackedObjs[i].previousPos.x = trackedObjs[i].previousPos.x + trackedObjs[i].momentum[0];
			trackedObjs[i].previousPos.y = trackedObjs[i].previousPos.y + trackedObjs[i].momentum[1];
		}
	}
}

void cleanUpTrackedObjs(vector<object_t> &trackedObjs, int currentFrame, int maxAge) {
	vector<size_t> to_delete;
	for (size_t i = 0; i < trackedObjs.size(); i++) {
		if (currentFrame - trackedObjs[i].lastSeen > maxAge) {
			to_delete.push_back(i);
		}
	}
	for (size_t i = 0; i < to_delete.size(); i++) {
		trackedObjs.erase(trackedObjs.begin() + to_delete[i]-i);
	}
}

int
main(int argc, char** argv) {
	int 				k;
	VideoCapture 			capture;
	HOGDescriptor 			hog;
	Mat 				frame;
	vector<object_t>		trackedObjs;
	vector<Rect>			found;
	int				max_age			= 	10;
	int 				frame_num		=	0;
	int 				current_id		=	0;
	static vector<float>    	detector		=	HOGDescriptor::getDefaultPeopleDetector();
	string 				detectionWindowName 	= 	"Display window";

	if (argc != 2) {
		cout << "Usage: " << argv[0] << "input_filename" << endl;
	} 
	else {
		capture.open(argv[1]);
		if ( ! capture.isOpened() ) {
	        	cerr << "Error opening video capture from file " << argv[1] << endl << "Leaving" << endl;
		} 
		else {
				namedWindow( detectionWindowName, WINDOW_AUTOSIZE );
				hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
			
				while ( capture.read(frame) ) {
					if( frame.empty() ) {
						cout << "Failled to capture frame" << endl << "Leaving" << endl;
						break;
					}
					cout << "Detecting objects." << endl;
					hog.detectMultiScale(frame, found, 0, Size(), Size(), 1.05, 1);
					groupRectangles(found, 0, 0.25);
					cout << "Update tracking objects." << endl;
					updateTrackedObjs(found, trackedObjs, frame_num, current_id);
					cout << "Cleaning up old objects." << endl;
					cleanUpTrackedObjs(trackedObjs, frame_num, max_age);
					cout << "Drawing objects and displaying frame." << endl;
					for (size_t i=0; i < trackedObjs.size(); i++) 
					{
						Rect r = trackedObjs[i].previousPos;
						rectangle(frame, r.tl(), r.br(), Scalar(0,255,0), 3);
						Vec2d tl(r.tl().x, r.tl().y);
			                        Vec2d br(r.br().x, r.br().y);
						Vec2d midle = (br+tl)/2;
						Vec2d end = midle + trackedObjs[i].momentum;
						line(frame, Point(midle), Point(end), Scalar(0,0,255));
					}	
					imshow(detectionWindowName, frame);
					
					k = waitKey(10);
					if( (char)k == 27 ) {
						break;
					}
					frame_num++;
				}
		}
	}	
	return 0;
}

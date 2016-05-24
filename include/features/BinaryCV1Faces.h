#ifndef FACEPROCESSOR_H
#define FACEPROCESSOR_H

#include <opencv2/core/core.hpp>

class BinaryCV1Faces 
{
	public:
		cv::Mat GetOrientationsBinaryTemplate(cv::Mat inputImage);
		int HammingDistance(cv::Mat templ1, cv::Mat templ2);
};

#endif

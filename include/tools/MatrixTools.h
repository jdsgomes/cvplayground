#ifndef MATRIXTOOLS_H
#define MATRIXTOOLS_H

#include "opencv2/core/core.hpp"

class MatrixTools 
{
	public:
		static int HammingDistance(cv::Mat templ1, cv::Mat templ2);
};


#endif
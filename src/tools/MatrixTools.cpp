#include "tools/MatrixTools.h"
#include "tools/GaborFilters.h"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "boost/filesystem.hpp" 
#include "boost/scoped_ptr.hpp"

using namespace std;

// change to real hamming distance once the GetBinaryTemplate returns a proper binary vector or matrix
int MatrixTools::HammingDistance(cv::Mat templ1, cv::Mat templ2)
{
	int distance = 0;

	if (templ1.rows != templ2.rows || templ1.cols != templ2.cols)
	{
		throw new exception("Matrices dimentions mismatch!");
	}
	if (templ1.type() != CV_8U || templ2.type() != CV_8U)
	{
		throw new exception("Wrong matrix type. Expecting CV_8U!");
	}

	for(int row = 0; row < templ1.rows; ++row) 
	{
		uchar* t1 = templ1.ptr(row);
		uchar* t2 = templ2.ptr(row);
		for(int col = 0; col < templ1.cols; ++col) 
		{
			int localDistance = std::abs(*t1++ - *t2++);
			if (localDistance == 3)
			{
				localDistance = 1;
			}
			distance += localDistance;
		}
	}

	return distance;
}

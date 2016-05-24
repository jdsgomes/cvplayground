#ifndef GABORFILTERS_H
#define GABORFILTERS_H
#include <opencv2/core/core.hpp>

class GaborFilters
{
	public:
		static cv::Mat MakeKernel(int ks, double sig, double th, double lm, double ps);
};

#endif

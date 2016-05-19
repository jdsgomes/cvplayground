#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include "features/BinaryCV1Faces.h"
#include "filters/GaborFilters.h"

using namespace std;

// binary coded as int:
// 00 = 0
// 01 = 1
// 10 = 2
// 11 = 3
cv::Mat BinaryCV1Faces::GetOrientationsBinaryTemplate(cv::Mat inputImage)
{
	int kernel_size=21;
    int pos_sigma= 5;
    int pos_lm = 50;
    float pos_th = 22.5f;
    int pos_psi = 90;
	int nBits = 4;
	vector<cv::Mat> gaborResponses;
	vector<float> th_offsets;

	th_offsets.push_back(0.0f);
	th_offsets.push_back(45.0f);
	th_offsets.push_back(67.5f);
	th_offsets.push_back(90.0f);

	int complexCellRf = 5;
	
	for (vector<float>::iterator offsetIt = th_offsets.begin(); offsetIt != th_offsets.end(); offsetIt++)
	{
		cv::Mat kernel = GaborFilters::MakeKernel(kernel_size, pos_sigma, pos_th + *offsetIt, pos_lm, pos_psi);
		cv::Mat response;
		cv::filter2D(inputImage, response, CV_32F, kernel);
		gaborResponses.push_back(response);
	}

	cv::Mat tmp(inputImage.rows/complexCellRf, inputImage.cols/complexCellRf, CV_8U);
	for (int i = 0; i < inputImage.rows/complexCellRf; i++)
	{
		for (int j = 0; j < inputImage.cols/complexCellRf; j++)
		{
			float absMaxLocalVal = -numeric_limits<float>::infinity();
			int maxLocalValIndex = 0;
			int orientationCode = 0; 
			vector<cv::Mat>::iterator responsesIt = gaborResponses.begin();
			while (responsesIt != gaborResponses.end())
			{
				cv::Mat roi = (*responsesIt)(cv::Rect(j*complexCellRf, i*complexCellRf, complexCellRf, complexCellRf));
				float maxLocalValForOrientation = *(max_element(roi.begin<float>(), roi.end<float>()));
				
				if (maxLocalValForOrientation > absMaxLocalVal)
				{
					absMaxLocalVal = maxLocalValForOrientation;
					maxLocalValIndex = orientationCode;
				}
				responsesIt++;
				orientationCode++;
			}
			tmp.at<unsigned char>(i,j) = maxLocalValIndex;
		}
	}
	return tmp;
}

// change to real hamming distance once the GetBinaryTemplate returns a proper binary vector or matrix
int BinaryCV1Faces::HammingDistance(cv::Mat templ1, cv::Mat templ2)
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

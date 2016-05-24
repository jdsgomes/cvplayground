#include <string>
#include <exception>
#include "boost/filesystem.hpp" 
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"


void LoadTokensFromFolder(std::string folder, std::vector<cv::Mat>& tokens)
{
	if (boost::filesystem::exists(folder))
	{
		boost::filesystem::directory_iterator end_itr;
		for(boost::filesystem::directory_iterator itr(folder); itr != end_itr; ++itr)
		{
			if(boost::filesystem::extension(itr->path()).compare("jpg"), CV_LOAD_IMAGE_COLOR)
			{
				// load file
				std::string imageName = itr->path().leaf().string();
				cv::Mat image = cv::imread(itr->path().string(), CV_LOAD_IMAGE_GRAYSCALE);

				// crop face area
				int cropX = 40;
				int cropY = 40;
				int cropWidth = 170; 
				int cropHeight = 170;
				if (image.cols < cropX+cropWidth || image.rows < cropY+cropY)
				{
					std::ostringstream oss;
					oss << "ERROR: Image size smaller than expected.  Filename: " << imageName << ". Cols = " << image.cols << ". Rows = " << image.rows << "." ;
					throw new std::exception(oss.str().c_str());
				}
				cv::Mat tokenImg = image(cv::Rect(cropX, cropY, cropWidth, cropHeight));
				tokens.push_back(tokenImg);
			}
		}
	}
	else
	{
		std::ostringstream oss;
		oss << "Unable to load images from " << folder << ". Leaving application.";
		throw new std::exception(oss.str().c_str());
	}
}
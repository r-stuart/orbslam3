#include <iostream>

#include <opencv2/core/core.hpp>

#include <System.h>

const std::size_t HEADER_SIZE = 8;

int main(int argc, char** argv) {

	if (argc != 4) {
		std::cerr << std::endl << "Usage: ./mono_wayve path_to_vocabulary path_to_settings image_count" << std::endl;
		return 1;
	}

	const int image_count = atoi(argv[3]);
	std::cout << "image_count: " << image_count << std::endl;

	ORB_SLAM3::System SLAM(argv[1], argv[2], ORB_SLAM3::System::MONOCULAR, false);
	float image_scale = SLAM.GetImageScale();

	std::cout << "image_scale: " << image_scale << std::endl;

	cv::Mat im;
	std::freopen(nullptr, "rb", stdin);

	for (int i=0; i<image_count; i++) {
		// get timestamp
		std::vector<unsigned char> ts_vec(HEADER_SIZE);
		std::fread(&ts_vec[0], sizeof(unsigned char), HEADER_SIZE, stdin);
		unsigned long long int ts = 0;
		for (int j=0; j<HEADER_SIZE; j++) {
			ts += (unsigned long long int) ts_vec[j] << ((HEADER_SIZE - j - 1) * 8);
		}

		// get next image size
		std::vector<unsigned char> header_vec(HEADER_SIZE);
		std::fread(&header_vec[0], sizeof(unsigned char), HEADER_SIZE, stdin);

		unsigned long long int image_size = 0;
		for (int j=0; j<HEADER_SIZE; j++) {
			image_size += (unsigned long long int) header_vec[j] << ((HEADER_SIZE - j - 1) * 8);
		}

		// get next image
		std::vector<unsigned char> image_vec(image_size);
		std::fread(&image_vec[0], sizeof(unsigned char), image_size, stdin);
		im = cv::imdecode(image_vec, cv::IMREAD_UNCHANGED);

		std::cout << "ts: " << ts << std::endl;

		// time tracking vars
		double t_resize = 0.f;
		double t_track = 0.f;

		// Rescaling image
		if (image_scale != 1.f) {
			int width = im.cols * image_scale;
			int height = im.rows * image_scale;
			cv::resize(im, im, cv::Size(width, height));
		}

		// Tracking frame
		double tframe = ((double) ts) * 1e-9;
		SLAM.TrackMonocular(im, tframe);
	}

	SLAM.Shutdown();
	SLAM.SaveTrajectoryEuRoC("/out/CameraTrajectory.txt");
	SLAM.SaveKeyFrameTrajectoryEuRoC("/out/KeyFrameTrajectory.txt");

	return 0;
}

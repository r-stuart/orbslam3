#include <iostream>
#include <limits>

#include <opencv2/core/core.hpp>

#include <System.h>

const std::size_t HEADER_SIZE = 8;
const unsigned long long int ULLI_MAX = std::numeric_limits<unsigned long long int>::max();

unsigned long long int read_bytes(int length) {
        std::vector<unsigned char> vec(length);
        std::fread(&vec[0], sizeof(unsigned char), length, stdin);
        unsigned long long int result = 0;
        for (int i=0; i<length; i++) {
                result += (unsigned long long int) vec[i] << ((length - i - 1) * 8);
        }
        return result;
}

int main(int argc, char** argv) {

	if (argc != 3) {
		std::cerr << std::endl << "Usage: ./mono_loop path_to_vocabulary path_to_settings" << std::endl;
		return 1;
	}

	ORB_SLAM3::System SLAM(argv[1], argv[2], ORB_SLAM3::System::MONOCULAR, false);
	float image_scale = SLAM.GetImageScale();

	std::cout << "image_scale: " << image_scale << std::endl;

	cv::Mat im;
	std::freopen(nullptr, "rb", stdin);

	unsigned long long int ts = 0;

	while ((ts = read_bytes(HEADER_SIZE)) != ULLI_MAX) {
        unsigned long long int image_size = read_bytes(HEADER_SIZE);

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

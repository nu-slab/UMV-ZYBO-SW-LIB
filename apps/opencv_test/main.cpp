#include <opencv2/opencv.hpp>
#include <string>

int g_slider, g_slider_max;

void trackbar(int, void *)
{
  if (g_slider % 2 == 0)
    g_slider++;
}

int main(void)
{
  std::string filename = "lenna.ppm";
  cv::Mat image = cv::imread(filename, 0), sobel;

  g_slider = 0;
  g_slider_max = 21;
  
  cv::namedWindow(filename, cv::WINDOW_GUI_EXPANDED);
  cv::createTrackbar("Trackbar name", filename,
		     &g_slider, g_slider_max, trackbar);

  while (true) {
    cv::Sobel(image, sobel, CV_8U, 1, 0, g_slider);
    cv::imshow(filename, sobel);
    int key = cv::waitKey(1);
    if (key == 'q') break;
  }
  
  cv::destroyAllWindows();
  return 0;
}

#include "live_plot.hpp"

#define PLOT_BUFSIZE  128
#define PLOT_DELAY_MS 100
#define PLOT_WIDTH    450
#define PLOT_HEIGHT   300

extern char    key;  
extern double  kp, ki, kd, bias;
extern int8_t  accel_l, accel_r;
extern int32_t left_rotation, right_rotation;

struct plot_data
{
  int8_t  accel_l, accel_r; 
  int32_t left_rotation, right_rotation;
};

void live_plot(void)
{
  /* open gnuplot via pipe */
  FILE *gp = popen("gnuplot > /dev/null 2>&1", "w");
  if (gp == nullptr) {
    std::cerr << "[ERROR] gnuplot popen failed." << std::endl;
    exit(1);
  }

  /* gnuplot configration */
  fprintf(gp, "set term qt size %d, %d\n", PLOT_WIDTH, PLOT_HEIGHT);
  fprintf(gp, "set grid \n");
  fprintf(gp, "set ylabel \"rotation value\" \n");
  fprintf(gp, "set xrange [0:%d] \n", PLOT_BUFSIZE - 1);
  fprintf(gp, "set yrange [0:200] \n");
  // fprintf(gp, "unset xtics \n");
  fprintf(gp, "set key left \n");
  fprintf(gp, "set key horizontal \n");
  fflush(gp); 

  std::vector<plot_data> data(PLOT_BUFSIZE, plot_data{0, 0, 0, 0});
  while (key != 'q') {
    /* push latest data */
    data[PLOT_BUFSIZE - 1].accel_r        = accel_r;
    data[PLOT_BUFSIZE - 1].accel_l        = accel_l;
    data[PLOT_BUFSIZE - 1].right_rotation = right_rotation;
    data[PLOT_BUFSIZE - 1].left_rotation  = left_rotation;

    /* plot data */
    fprintf(gp, "set title \"k_p = %.1lf, k_i = %.1lf, k_d = %.1lf, bias = %.1lf\" \n",
	    kp, ki, kd, bias);
    fprintf(gp, "plot '-' with lines lw 2 lt rgbcolor '#44FF4500' dt \".\" title \"right target\", ");
    fprintf(gp, "     '-' with lines lw 2 lt rgbcolor '#4400FFFF' dt \".\" title \"left target\", ");
    fprintf(gp, "     '-' with lines lw 3 lt rgbcolor '#88FF4500'          title \"right rotation\", ");
    fprintf(gp, "     '-' with lines lw 3 lt rgbcolor '#8800FFFF'          title \"left rotation\" \n");
    for (int i = 0; i < PLOT_BUFSIZE; i++) 
      fprintf(gp, "%d\n", std::abs(data[i].accel_r));
    fprintf(gp, "e\n");
    for (int i = 0; i < PLOT_BUFSIZE; i++) 
      fprintf(gp, "%d\n", std::abs(data[i].accel_l));
    fprintf(gp, "e\n");
    for (int i = 0; i < PLOT_BUFSIZE; i++)
      fprintf(gp, "%d\n", data[i].right_rotation);
    fprintf(gp, "e\n");
    for (int i = 0; i < PLOT_BUFSIZE; i++) 
      fprintf(gp, "%d\n", data[i].left_rotation);
    fprintf(gp, "e\n");
    fflush(gp); 

    /* rotate data (ring buffer) */
    std::rotate(data.begin(), data.begin() + 1, data.end());
    std::this_thread::sleep_for(std::chrono::milliseconds(PLOT_DELAY_MS));
  }

  fprintf(gp, "quit\n"); fprintf(gp, "e\n");
  fflush(gp); 
  pclose(gp);
}

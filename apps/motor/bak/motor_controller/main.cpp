#include <iostream>
#include <slab/system.hpp>
#include <thread>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

namespace slab {
	class Motor {
		private   :
			char key;
			char* devname;
			bool loop = true;
			bool kbhit() {
				struct termios oldt, newt;
				int ch;
				int oldf;

				tcgetattr(STDIN_FILENO, &oldt);
				newt = oldt;
				newt.c_lflag &= ~(ICANON | ECHO);
				tcsetattr(STDIN_FILENO, TCSANOW, &newt);
				oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
				fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

				ch = getchar();

				tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
				fcntl(STDIN_FILENO, F_SETFL, oldf);

				if (ch != EOF)
				{
					ungetc(ch, stdin);
					return true;
				}

				return false;
			}
		protected :
		public    :
			Motor(char* dev) {
				devname = dev;
			}
			~Motor() {
			}
			void Motor_ctrl() {
				slab::System fpga(devname);
				const int8_t ACCEL_MAX = 127, ACCEL_MIN = -127, ACCEL_INC = 16;
				const int8_t STEER_MAX = 127, STEER_MIN = -127, STEER_INC = 16;
				bool  hit;
				int8_t accel = 0;
				int8_t steer = 0;

				float p = 50.0;
				float i = 0;
				float d = 0;

				int32_t p_r = (int32_t)(roundf(p*(1 << 8)));
				int32_t i_r = (int32_t)(roundf(i*(1 << 8)));
				int32_t d_r = (int32_t)(roundf(d*(1 << 8)));

				int32_t p_l = (int32_t)(roundf(p*(1 << 8)));
				int32_t i_l = (int32_t)(roundf(i*(1 << 8)));
				int32_t d_l = (int32_t)(roundf(d*(1 << 8)));

				fpga.set_pid_gain(p_r, i_r, d_r, 0, true);
				fpga.set_pid_gain(p_l, i_l, d_l, 0, false);
				while (loop) {
					hit = kbhit();
					if (hit) {
						key = getchar();
						switch (key) {
							case 'h':
								steer = (steer < STEER_MIN + STEER_INC) ? STEER_MIN : steer - STEER_INC;
								break;
							case 'j':
								accel = (accel < ACCEL_MIN + ACCEL_INC) ? ACCEL_MIN : accel - ACCEL_INC;
								break;
							case 'k':
								accel = (accel > ACCEL_MAX - ACCEL_INC) ? ACCEL_MAX : accel + ACCEL_INC;
								break;
							case 'l':
								steer = (steer > STEER_MAX - STEER_INC) ? STEER_MAX : steer + STEER_INC;
								break;
							case 'r':
								accel = 0;
								steer = 0;
								break;
							case 'q':
								accel = 0;
								steer = 0;
								loop  = false;
								break;
							case 'p':
								p = p + 1.0;

								p_r = (int32_t)(roundf(p*(1 << 8)));
								p_l = (int32_t)(roundf(p*(1 << 8)));
                              					fpga.set_pid_gain(p_r, i_r, d_r, 0, true);
								fpga.set_pid_gain(p_l, i_l, d_l, 0, false);
							default:
								break;
						}
						fpga.send_accel(accel); // accel
						fpga.send_steer(steer); // steer
					}
					printf("\raccel: %d, steer: %d, rotation_r: %d, rotation_l: %d, p: %3.3lf  ", accel, steer,  fpga.recv_rotation(true), fpga.recv_rotation(false), p);
					fflush(stdout);
				}
				printf("\n");
			}
	};
};

int main(int argc, char* argv[]) {
	slab::Motor motor("/dev/uio0");
	motor.Motor_ctrl();

	return 0;
}

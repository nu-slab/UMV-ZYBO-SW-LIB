//-----------------------------------------------------------------------------
// <system.hpp>
//  - Header of slab::System
//    - Declared of slab::System class
//      - This class inherits from the slab::PSPL class
//-----------------------------------------------------------------------------
// Version 1.00 (Sep. 23, 2020)
//  - Added declaration of slab::System class
//-----------------------------------------------------------------------------
// (C) 2020-2021 Naofumi Yoshinaga, Taichi Saikai. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SYSTEM_SLAB_H
#define SYSTEM_SLAB_H

#include <slab/pspl.hpp>
#include <vector>

#define RADDR_WHELL_ROTATION_R              0x00
#define RADDR_WHELL_ROTATION_L              0x01

#define RADDR_FINDCONTOURS_READY            0x02
#define RADDR_FINDCONTOURS_POINTS_NUM       0x03
#define RADDR_FINDCONTOURS_POINT_DATA       0x04

#define RADDR_DETECTION                     0x1e
#define RADDR_SW                            0x1f

#define WADDR_WHEEL_ACCEL_R                 0x00
#define WADDR_WHEEL_ACCEL_L                 0x01
#define WADDR_WHEEL_BRAKE                   0x02
#define WADDR_WHEEL_PID_KP_R                0x03
#define WADDR_WHEEL_PID_KP_L                0x04
#define WADDR_WHEEL_PID_KI_R                0x05
#define WADDR_WHEEL_PID_KI_L                0x06
#define WADDR_WHEEL_PID_KD_R                0x07
#define WADDR_WHEEL_PID_KD_L                0x08
#define WADDR_WHEEL_PID_BIAS_R              0x09
#define WADDR_WHEEL_PID_BIAS_L              0x0a
#define WADDR_FINDCONTOURS_EDGE_THRES       0x0b
#define WADDR_FINDCONTOURS_ES_THRES         0x0c
#define WADDR_FINDCONTOURS_ES_COLOR         0x0d
#define WADDR_FINDCONTOURS_FILTER_TYPE      0x0e
#define WADDR_FINDCONTOURS_FL_OVERLAY       0x0f
#define WADDR_FINDCONTOURS_LINE_WIDTH       0x10
#define WADDR_FINDCONTOURS_THRES_WIDHT      0x11
#define WADDR_FINDCONTOURS_DETECT_LINES     0x12
#define WADDR_FINDCONTOURS_WRITE_PROTECT    0x13
#define WADDR_FINDCONTOURS_RADDR            0x14
#define WADDR_FINDCONTOURS_BINZ_THRESHOLD   0x15
#define WADDR_FINDCONTOURS_VIDEO_MODE       0x16

namespace slab
{
	typedef struct Point_t {
		uint32_t d, h, v;
	} Point_t;

	class System : public PSPL
	{
		private:
		protected:
		public:
			using PSPL::PSPL;

			/* LSD */
			std::vector<Point_t> points_;
			void set_findContoursParams(uint32_t reg_num, uint32_t value);
			void fetch_pointsFrame();

			/* wheel */
			void send_accel(int8_t accel_l, int8_t accel_r);
			void send_brake(int8_t brake);
			void set_pid_gain(uint32_t kp, uint32_t ki, uint32_t kd, uint32_t bias, bool r_l);
			uint16_t recv_rotation(bool r_l);

			/* detection (doll) */
			int recv_detection(void);

			/* debug */
			void send_led(uint8_t led);
			uint16_t recv_sw(void);
	};
}

#endif // SYSTEM_SLAB_H

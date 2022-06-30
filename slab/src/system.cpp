//-----------------------------------------------------------------------------
// <system.cpp>
//  - Defined functions of slab::System class
//-----------------------------------------------------------------------------
// Version 1.00 (Sep. 23, 2020)
//  - Added definition for functions of slab::System class
//-----------------------------------------------------------------------------
// (C) 2020-2021 Naofumi Yoshinaga, Taichi Saikai All rights reserved.
//-----------------------------------------------------------------------------

#include <slab/system.hpp>
#include <iostream>

namespace slab
{
	//------------------------------------------------------
	// Find Contours
	//------------------------------------------------------

	void System::set_findContoursParams(uint32_t reg_num, uint32_t value)
	{ 
		switch(reg_num){
			case WADDR_FINDCONTOURS_EDGE_THRES:
				write(WADDR_FINDCONTOURS_EDGE_THRES  , value);
				break;
			case WADDR_FINDCONTOURS_ES_THRES:
				write(WADDR_FINDCONTOURS_ES_THRES    , value);
				break;
			case WADDR_FINDCONTOURS_ES_COLOR:
				write(WADDR_FINDCONTOURS_ES_COLOR    , value);
				break;
			case WADDR_FINDCONTOURS_FILTER_TYPE:
				write(WADDR_FINDCONTOURS_FILTER_TYPE , value);
				break;
			case WADDR_FINDCONTOURS_FL_OVERLAY:
				write(WADDR_FINDCONTOURS_FL_OVERLAY  , value);
				break;
			case WADDR_FINDCONTOURS_LINE_WIDTH:
				write(WADDR_FINDCONTOURS_LINE_WIDTH  , value);
				break;
			case WADDR_FINDCONTOURS_THRES_WIDHT:
				write(WADDR_FINDCONTOURS_THRES_WIDHT , value);
				break;
			case WADDR_FINDCONTOURS_DETECT_LINES:
				write(WADDR_FINDCONTOURS_DETECT_LINES, value);
				break;
			case WADDR_FINDCONTOURS_BINZ_THRESHOLD:
				write(WADDR_FINDCONTOURS_BINZ_THRESHOLD, value);
				break;
			case WADDR_FINDCONTOURS_VIDEO_MODE:
				write(WADDR_FINDCONTOURS_VIDEO_MODE, value);
				break;
			default:
				break;
		}
	}

	void System::fetch_pointsFrame()
	{	
		uint32_t num_of_points;
		uint32_t point_data;
		Point_t point;

		write(WADDR_FINDCONTOURS_WRITE_PROTECT, 0x1);        // set write_protect
		while (!read(RADDR_FINDCONTOURS_READY))              // wait buffer-ready
			usleep(10000);
		num_of_points = read(RADDR_FINDCONTOURS_POINTS_NUM); // get number of points

		/* fetch points in a frame */
		points_.clear();
		for (int n = 0; n < num_of_points; n++) {
			write(WADDR_FINDCONTOURS_RADDR, n);                // set read-address
			point_data = read(RADDR_FINDCONTOURS_POINT_DATA);
			point.d = (point_data >> 21) & 1;
			point.h = (point_data >> 10) & 0x7ff; 
			point.v = point_data  & 0x3ff; 
			points_.push_back(point);
		}

		write(WADDR_FINDCONTOURS_WRITE_PROTECT, 0x0); // unset write_protect
	}

	//------------------------------------------------------
	// wheel
	//------------------------------------------------------

	void System::send_accel(int8_t accel_l, int8_t accel_r)
	{
		write(WADDR_WHEEL_ACCEL_L, accel_l);
		write(WADDR_WHEEL_ACCEL_R, accel_r);
	}

	void System::send_brake(int8_t brake) { write(WADDR_WHEEL_BRAKE, brake); }

	void System::set_pid_gain(uint32_t kp, uint32_t ki, uint32_t kd, uint32_t bias, bool r_l)
	{
		uint8_t addr_wheel_pid_kp   = r_l ? WADDR_WHEEL_PID_KP_R   : WADDR_WHEEL_PID_KP_L;
		uint8_t addr_wheel_pid_ki   = r_l ? WADDR_WHEEL_PID_KI_R   : WADDR_WHEEL_PID_KI_L;
		uint8_t addr_wheel_pid_kd   = r_l ? WADDR_WHEEL_PID_KD_R   : WADDR_WHEEL_PID_KD_L;
		uint8_t addr_wheel_pid_bias = r_l ? WADDR_WHEEL_PID_BIAS_R : WADDR_WHEEL_PID_BIAS_L;
		write(addr_wheel_pid_kp,   kp); 
		write(addr_wheel_pid_ki,   ki); 
		write(addr_wheel_pid_kd,   kd); 
		write(addr_wheel_pid_bias, bias); 
	}

	uint16_t System::recv_rotation(bool r_l)
	{
		uint8_t addr_wheel_rotation = r_l ? RADDR_WHELL_ROTATION_R : RADDR_WHELL_ROTATION_L;
		return read(addr_wheel_rotation);
	}

	//------------------------------------------------------
	// yolo detection
	//------------------------------------------------------

	int System::recv_detection(void) 
	{ 
		return read(RADDR_DETECTION) & 0x3; 
	}
	
	// return value
	// case 2'b00: not detected
	// case 2'b01: detect doll
	// case 2'b10: detect traffic light
	// case 2'b11: detect both doll and traffic light

	//------------------------------------------------------
	// debug
	//------------------------------------------------------

	void System::send_led(uint8_t led)
	{
		// write(WADDR_LED, led);
	}

	uint16_t System::recv_sw(void) 
	{
		return 1;
		// return read(RADDR_SW); 
	}

} // namespace slab

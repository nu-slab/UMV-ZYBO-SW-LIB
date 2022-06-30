#include <iostream>
#include <cstdio>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cctype>
#include <stdint.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <slab.hpp>
#include <slab/pcam5c.hpp>
#include <slab/system.hpp>
#include <fcntl.h>
#include <sys/mman.h>
#include <slab/bsp/xparameters.h>
#include <time.h>

#define REG_BASE_ADDR   XPAR_SYSTEM_ZYNQ_PROCESSOR_0_S00_AXI_BASEADDR
#define MEM_BASE_ADDR_0 (XPAR_DDR_MEM_BASEADDR + 0x09000000)
#define MEM_BASE_ADDR_1 (XPAR_DDR_MEM_BASEADDR + 0x0A000000)
#define GAMMA_BASE_ADDR XPAR_SYSTEM_PCAMINTERFACE_AXI_GAMMACORRECTION_0_BASEADDR
#define RESOLUTION_H    1280
#define RESOLUTION_V     720
#define RED             "\e[0;31m"
#define GRN             "\e[0;32m"
#define VNC

/* for Debug */
void live_stream(digilent::AXI_VDMA<digilent::ScuGicInterruptController>& vdma_driver_0, digilent::AXI_VDMA<digilent::ScuGicInterruptController>& vdma_driver_1);
void edit_param(slab::System pl_connection);

typedef struct bgr_t { uint8_t b, g, r; } bgr_t;

unsigned int width = RESOLUTION_H / 2, height = RESOLUTION_V / 2;
bool lsd_overlay = false;
bool kill_flag = false;

int main(int argc, char* argv[])
{
	/* Declare Instances */
	digilent::OV5640_cfg::mode_t mode_pcam5c = digilent::OV5640_cfg::MODE_720P_1280_720_60fps;
	digilent::Resolution         res_hdmi    = digilent::Resolution::R1280_720_60_PP;

	digilent::pcam5c cam("i2c-0", mode_pcam5c, digilent::OV5640_cfg::awb_t::AWB_ADVANCED); // on-board Pcam5C

	digilent::VideoOutput               vid_0(XPAR_VTC_0_DEVICE_ID), vid_1(XPAR_VTC_1_DEVICE_ID);
	digilent::ScuGicInterruptController irpt_ctl(static_cast<uint16_t>(XPAR_PS7_SCUGIC_0_DEVICE_ID));

	digilent::AXI_VDMA<digilent::ScuGicInterruptController>
		vdma_driver_0(static_cast<uint16_t>(XPAR_AXIVDMA_0_DEVICE_ID),
				static_cast<uint32_t>(MEM_BASE_ADDR_0),            // frame buffer base addr
				irpt_ctl,                                          // interrupt control 
				XPAR_FABRIC_AXIVDMA_0_MM2S_INTROUT_VEC_ID,         // read interrupt ID
				XPAR_FABRIC_AXIVDMA_0_S2MM_INTROUT_VEC_ID),        // write interrupt ID
		vdma_driver_1(static_cast<uint16_t>(XPAR_AXIVDMA_1_DEVICE_ID),
				static_cast<uint32_t>(MEM_BASE_ADDR_1),                                      
				irpt_ctl,                                                            
				XPAR_FABRIC_AXIVDMA_1_MM2S_INTROUT_VEC_ID,     
				XPAR_FABRIC_AXIVDMA_1_S2MM_INTROUT_VEC_ID);

	/* Initialize pxam5c */
	cam.work(mode_pcam5c);

	/* Initialze Video Timing Controller */
	vid_0.reset();
	vid_0.configure(res_hdmi);
	vid_0.enable();
	vid_1.reset();
	vid_1.configure(res_hdmi);
	vid_1.enable();

	/* Initialize VDMA Drivers */
	vdma_driver_0.resetWrite();
	vdma_driver_0.configureWrite(digilent::timing[static_cast<int>(res_hdmi)].h_active, digilent::timing[static_cast<int>(res_hdmi)].v_active);
	vdma_driver_0.enableWrite();
	vdma_driver_0.resetRead();
	vdma_driver_0.configureRead(digilent::timing[static_cast<int>(res_hdmi)].h_active, digilent::timing[static_cast<int>(res_hdmi)].v_active);
	vdma_driver_0.enableRead();
	
	vdma_driver_1.resetWrite();
	vdma_driver_1.configureWrite(digilent::timing[static_cast<int>(res_hdmi)].h_active, digilent::timing[static_cast<int>(res_hdmi)].v_active);
	vdma_driver_1.enableWrite();
	vdma_driver_1.resetRead();
	vdma_driver_1.configureRead(digilent::timing[static_cast<int>(res_hdmi)].h_active, digilent::timing[static_cast<int>(res_hdmi)].v_active);
	vdma_driver_1.enableRead();

	/* Initialize PL Parameter */
	slab::System pl_connection("/dev/uio0");
	pl_connection.set_findContoursParams(WADDR_FINDCONTOURS_EDGE_THRES  , 60); // 0 : edge threshold       
	pl_connection.set_findContoursParams(WADDR_FINDCONTOURS_ES_THRES    , 5);  // 1 : conv threshold       
	pl_connection.set_findContoursParams(WADDR_FINDCONTOURS_ES_COLOR    , 1);  // 2 : edge selection color 
	pl_connection.set_findContoursParams(WADDR_FINDCONTOURS_FILTER_TYPE , 1);  // 3 : filter type          
	pl_connection.set_findContoursParams(WADDR_FINDCONTOURS_FL_OVERLAY  , 1);  // 4 : fl overlay           
	pl_connection.set_findContoursParams(WADDR_FINDCONTOURS_LINE_WIDTH  , 60); // 5 : line width           
	pl_connection.set_findContoursParams(WADDR_FINDCONTOURS_THRES_WIDHT , 10); // 6 : threshold width      
	pl_connection.set_findContoursParams(WADDR_FINDCONTOURS_DETECT_LINES, 6);  // 7 : detect lines         
	
	/* live view application work on another thread */
	std::thread t(live_stream, std::ref(vdma_driver_0), std::ref(vdma_driver_1));

	const std::string usage = "Usage: <exit|x> | <reset|r> | <edit|e>";
	
	std::string buf;
	
	while (true) {
		std::cout << usage << std::endl;
		std::cout << ">> "; std::cin >> buf;

		if (buf == "exit" || buf == "x" | std::cin.fail()) break;
		if (buf == "test") {
			const uint16_t addr = 0x503d; // debug mode addr
			uint8_t data;
			std::cout << "Enter 'x' to exit here" << std::endl;
			std::cout << ">> [addr] 0x" << std::hex << addr << std::endl;
			while (true) {
				std::cout << ">> [data] 0x"; std::cin  >> buf;
				if (buf == "x") break;
				data = static_cast<uint8_t>(std::stoi(buf, nullptr, 16));
				cam.writeReg(addr, data);
			}
		}

		if (buf == "reset" | buf == "r") {
			cam.work(mode_pcam5c);
		}

		if (buf == "edit" | buf == "e"){
			edit_param(pl_connection);
		}
		//if (buf == "i2c") {
		//	uint16_t addr;
		//	uint8_t  data;
		//	std::cout << "Enter 'x' to exit here" << std::endl;
		//	while (true) {
		//		std::cout << ">> [addr] 0x"; std::cin >> buf;
		//		if (buf == "x") break;
		//		addr = static_cast<uint16_t>(std::stoi(buf, nullptr, 16));
		//		std::cout << ">> [data] 0x"; std::cin >> buf;
		//		if (buf == "x") break;
		//		data = static_cast<uint8_t>(std::stoi(buf, nullptr, 16));
		//		cam.writeReg(addr, data);
		//	}
		//}

		//if (buf == "gamma") {
		//	std::cout << "  Please press the key corresponding to the desired Gamma factor: " << std::endl;
		//	std::cout << "    1. Gamma Factor = 1    " << std::endl;
		//	std::cout << "    2. Gamma Factor = 1/1.2" << std::endl;
		//	std::cout << "    3. Gamma Factor = 1/1.5" << std::endl;
		//	std::cout << "    4. Gamma Factor = 1/1.8" << std::endl;
		//	std::cout << "    5. Gamma Factor = 1/2.2" << std::endl;
		//	std::cout << ">> "; std::cin >> buf; 
		//	uint8_t g = static_cast<uint8_t>(std::stoi(buf, nullptr, 10)); 
		//	if ((g > 0) && (g < 6)) {
		//		Xil_Out32(GAMMA_BASE_ADDR, g - 1);
		//	}
		//}
	}
	kill_flag = true;

	t.join();

	return 0;
}

inline void get_framebuffer(bgr_t* frame_buf, bgr_t* img, uint32_t pixels, uint32_t frameBytes, const uint8_t frame_index)
{
	memcpy(img, frame_buf + (pixels * frame_index), frameBytes);
}

inline bgr_t hsv2rgb(float in_h, float in_s, float in_v)
{
	float h = in_h;          // 0 - 360
	float s = in_s / 255.0f; // 0.0 - 1.0
	float v = in_v / 255.0f; // 0.0 - 1.0

	float r, g, b; // 0.0 - 1.0

	int   hi = (int)(h / 60.0f) % 6;
	float f  = (h / 60.0f) - hi;
	float p  = v * (1.0f - s);
	float q  = v * (1.0f - s * f);
	float t  = v * (1.0f - s * (1.0f - f));

	switch (hi) {
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
	}

	bgr_t bgr =
	{static_cast<uint8_t>(255 * b),
		static_cast<uint8_t>(255 * g),
		static_cast<uint8_t>(255 * r)};
	return bgr;
}

void live_stream(digilent::AXI_VDMA<digilent::ScuGicInterruptController>& vdma_driver_0, digilent::AXI_VDMA<digilent::ScuGicInterruptController>& vdma_driver_1)
{
	const uint32_t pixels      = RESOLUTION_H * RESOLUTION_V;
	const int      n_frame     = 3;
	const uint32_t frameBytes  = pixels * sizeof(bgr_t);
	const int      text_offset_h = 25, text_offset_v = 50;

	/* open mem */
	off_t fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1) {
		std::cerr << RED << "[ERROR] open failed: /dev/mem" << std::endl;
		exit(1);
	}

	/* mmap */
	bgr_t *frame_buf_0 = (bgr_t*)mmap(0, n_frame * frameBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, vdma_driver_0.getFrameBufferAddr() & ~MAP_MASK);
	bgr_t *frame_buf_1 = (bgr_t*)mmap(0, n_frame * frameBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, vdma_driver_1.getFrameBufferAddr() & ~MAP_MASK);
	if (frame_buf_0 == nullptr) {
		std::cerr << RED << "[ERROR] mmap failed: frame_buf_0" << std::endl;
	}
	if (frame_buf_1 == nullptr) {
		std::cerr << RED << "[ERROR] mmap failed: frame_buf_1" << std::endl;
	}

	/* show video */
	cv::Mat frame_0(RESOLUTION_V, RESOLUTION_H, CV_8UC3);
	cv::Mat frame_1(RESOLUTION_V, RESOLUTION_H, CV_8UC3);
	cv::Mat cat, view;
	
	slab::System pl_connection("/dev/uio0");  
	while (true) {
		get_framebuffer(frame_buf_0, (bgr_t *)frame_0.data, pixels, frameBytes, vdma_driver_0.getCurrReadFrame());
		get_framebuffer(frame_buf_1, (bgr_t *)frame_1.data, pixels, frameBytes, vdma_driver_1.getCurrReadFrame());

		// front (another board)
		clock_t start = clock();    // スタート時間
		pl_connection.fetch_pointsFrame();
		clock_t end = clock();      // 終了時間
		
		const int n_point = pl_connection.points_.size(); 
		
		for (int i = 0; i < n_point; i++) {
			cv::circle(
					frame_1,
					cv::Point(pl_connection.points_[i].h, pl_connection.points_[i].v),
					2,
					cv::Scalar(0, pl_connection.points_[i].d * 255.0, !pl_connection.points_[i].d * 255),
					-1,
					cv::LINE_AA
			);
		}
		cv::putText(frame_1, 
				"detected points:" + std::to_string(n_point) + "  duration:" + std::to_string((double)(end - start) / CLOCKS_PER_SEC) + "sec",
				cv::Point(text_offset_h, text_offset_v),
				cv::FONT_HERSHEY_SIMPLEX,
				1.2,
				cv::Scalar(0, 255, 255),
				2,
				CV_AA
		);

		//cv::imshow("Pcam5C", frame_1);
		cv::vconcat(frame_1, frame_0, cat);
		cv::resize(cat, view, cv::Size(), width / (float)cat.cols, 2 * height / (float)cat.rows);
		//cv::resize(frame_0, view, cv::Size(), width / (float)frame_0.cols, height / (float)frame_0.rows);
		cv::imshow("Pcam5C", view);

		int key = cv::waitKey(1);
		if (key == 'q') break;
		if (key == 's') {
			static unsigned int num = 0;
			char filename[64];
			sprintf(filename, "img/%08d.png", num);
			num++;
			cv::imwrite(filename, frame_1);
			std::clog << GRN << "[INFO] Your images is saved!" << std::endl;
		}
		if (kill_flag) break;
	}
	cv::destroyAllWindows();
}

void edit_param(slab::System pl_connection){

	std::string buf;
	int reg_num, value;

	std::cout << "Please input register number. \n \
	0 : edge threshold        \n \
	1 : conv threshold        \n \
	2 : edge selection color  \n \
	3 : filter type           \n \
	4 : fl overlay            \n \
	5 : line width            \n \
	6 : threshold width       \n \
	7 : detect lines          \n >> ";
	std::cin >> buf;
	
	reg_num = std::stoi(buf, nullptr, 10); 
	std::cout << "Please input write value. \n >> ";
	std::cin >> buf;
	value  = std::stoi(buf, nullptr, 10); 

	pl_connection.set_findContoursParams(reg_num + 11, value);
}

#include <slab/video/VideoOutput.hpp>

namespace digilent
{
	VideoOutput::VideoOutput(u32 VTC_dev_id)
	{
		XVtc_Config *psVtcConfig;
		XStatus Status;
		psVtcConfig = XVtc_LookupConfig(VTC_dev_id);
		if (NULL == psVtcConfig) 
			throw std::runtime_error(__FILE__ ":" LINE_STRING);
		Status = XVtc_CfgInitialize(&sVtc_, psVtcConfig, psVtcConfig->BaseAddress);
		if (Status != XST_SUCCESS) 
			throw std::runtime_error(__FILE__ ":" LINE_STRING);
	}

	void VideoOutput::reset() { XVtc_Reset(&sVtc_); }

	void VideoOutput::configure(Resolution res)
	{
		size_t i;
		for (i = 0; i < sizeof(timing)/sizeof(timing[0]); i++)
			if (timing[i].res == res) break;

		if (i < sizeof(timing)/sizeof(timing[0])) {
			XVtc_Timing sTiming   = {}; // Will init to 0 (C99 6.7.8.21)
			sTiming.HActiveVideo  = timing[i].h_active;
			sTiming.HFrontPorch   = timing[i].h_fp;
			sTiming.HBackPorch    = timing[i].h_bp;
			sTiming.HSyncWidth    = timing[i].h_sync;
			sTiming.HSyncPolarity = (u16)timing[i].h_pol;
			sTiming.VActiveVideo  = timing[i].v_active;
			sTiming.V0FrontPorch  = timing[i].v_fp;
			sTiming.V0BackPorch   = timing[i].v_bp;
			sTiming.V0SyncWidth   = timing[i].v_sync;
			sTiming.VSyncPolarity = (u16)timing[i].v_pol;

			printf("  sTiming.HActiveVideo  : %d\n", sTiming.HActiveVideo);
			printf("  sTiming.HFrontPorch   : %d\n", sTiming.HFrontPorch);
			printf("  sTiming.HBackPorch    : %d\n", sTiming.HBackPorch);
			printf("  sTiming.HSyncWidth    : %d\n", sTiming.HSyncWidth);
			printf("  sTiming.HSyncPolarity : %d\n", sTiming.HSyncPolarity);
			printf("  sTiming.VActiveVideo  : %d\n", sTiming.VActiveVideo);
			printf("  sTiming.V0FrontPorch  : %d\n", sTiming.V0FrontPorch);
			printf("  sTiming.V0BackPorch   : %d\n", sTiming.V0BackPorch);
			printf("  sTiming.V0SyncWidth   : %d\n", sTiming.V0SyncWidth);
			printf("  sTiming.V1FrontPorch  : %d\n", sTiming.V1FrontPorch);
			printf("  sTiming.V1BackPorch   : %d\n", sTiming.V1BackPorch);
			printf("  sTiming.V1SyncWidth   : %d\n", sTiming.V1SyncWidth);
			printf("  sTiming.VSyncPolarity : %d\n", sTiming.VSyncPolarity);
			printf("  sTiming.Interlaced    : %d\n", sTiming.Interlaced);
			printf("\n");

			XVtc_SetGeneratorTiming(&sVtc_, &sTiming);

			printf("  sVtc_.Config.DeviceID    : 0x%04X\n", sVtc_.Config.DeviceId);
			printf("  sVtc_.Config.BaseAddress : 0x%08X\n", sVtc_.Config.BaseAddress);
			printf("  sVtc_.IsReady            : 0x%08X\n", sVtc_.IsReady);
			printf("\n");

			XVtc_RegUpdateEnable(&sVtc_);
		}
	}

	void VideoOutput::enable() { XVtc_EnableGenerator(&sVtc_); }
};

/*
 * AXI_VDMA.h
 *
 *  Created on: Sep 2, 2016
 *      Author: Elod
 */

#ifndef AXI_VDMA_H_
#define AXI_VDMA_H_

#include <stdexcept>
#include <functional>
#include <slab/bsp/xaxivdma.h>
#include <slab/bsp/xscugic.h>
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

namespace digilent
{
	/*!
	 * \brief Driver class for Xilinx AXI VDMA IP. Needs to have stable clocks before
	 *        instantiation to be able to complete hardware reset.
	 */
	class AXI_VDMA
	{
		typedef struct vdma_context_t
		{
			/* The state variable to keep track if the initialization is done */
			unsigned int init_done;
			/* The XAxiVdma_DmaSetup structure contains all the necessary information to
			 * start a frame write or read. */
			XAxiVdma_DmaSetup ReadCfg;
			XAxiVdma_DmaSetup WriteCfg;
			/* Horizontal size of frame */
			unsigned int hsize;
			/* Vertical size of frame */
			unsigned int vsize;
			/* Buffer address from where read and write will be done by VDMA */
			unsigned int buffer_address;
			/* Flag to tell VDMA to interrupt on frame completion */
			unsigned int enable_frm_cnt_intr;
			/* The counter to tell VDMA on how many frames the interrupt should happen */
			unsigned int number_of_frame_count;
		} vdma_context_t;
		public:
		// Shim function to extract function object from CallbackRef and call it
		// This should call our member function handlers below
		template <typename Func>
			static void MyCallback(void* CallbackRef, uint32_t mask_or_type)
			{
				auto pfn = static_cast<Func*>(CallbackRef);
				pfn->operator()(mask_or_type);
			}

		AXI_VDMA(uint16_t dev_id, uint32_t frame_buf_base_addr):
			context_{},
			frame_buf_base_addr_(frame_buf_base_addr)
			{
				XAxiVdma_Config* psConf;
				XStatus Status;

				psConf = XAxiVdma_LookupConfig(dev_id);
				if (!psConf) 
					throw std::runtime_error(__FILE__ ":" LINE_STRING);

				// Initialize driver instance and reset VDMA
				Status = XAxiVdma_CfgInitialize(&drv_inst_, psConf, psConf->BaseAddress);
				if (Status != XST_SUCCESS) 
					throw std::runtime_error(__FILE__ ":" LINE_STRING);
			}

		void resetRead()
		{
			// XAxiVdma_ChannelStop(&drv_inst_.ReadChannel);
			// while (XAxiVdma_ChannelIsRunning(&drv_inst_.ReadChannel)) ;
			XAxiVdma_ChannelReset(&drv_inst_.ReadChannel);
			int Polls = RESET_POLL;
			while (Polls && XAxiVdma_ChannelResetNotDone(&drv_inst_.ReadChannel)) 
				--Polls;
			if (!Polls) 
				throw std::runtime_error(__FILE__ ":" LINE_STRING);
		}

		void resetWrite()
		{
			// XAxiVdma_ChannelStop(&drv_inst_.WriteChannel);
			// while (XAxiVdma_ChannelIsRunning(&drv_inst_.WriteChannel)) ;
			XAxiVdma_ChannelReset(&drv_inst_.WriteChannel);
			int Polls = RESET_POLL;
			while (Polls && XAxiVdma_ChannelResetNotDone(&drv_inst_.WriteChannel))
				--Polls;
			if (!Polls) 
				throw std::runtime_error(__FILE__ ":" LINE_STRING);
		}

		void configureRead(uint16_t h_res, uint16_t v_res)
		{
			XStatus status;
			context_.ReadCfg.HoriSizeInput = h_res * drv_inst_.ReadChannel.StreamWidth;
			context_.ReadCfg.VertSizeInput = v_res;
			context_.ReadCfg.Stride = context_.ReadCfg.HoriSizeInput;
			context_.ReadCfg.FrameDelay = 1;
			context_.ReadCfg.EnableCircularBuf = 1;
			context_.ReadCfg.EnableSync = 1;
			context_.ReadCfg.PointNum = 0;
			context_.ReadCfg.EnableFrameCounter = 0;
			context_.ReadCfg.FixedFrameStoreAddr = 0; // park it on 0 until we sync
			status = XAxiVdma_DmaConfig(&drv_inst_, XAXIVDMA_READ, &context_.ReadCfg);
			if (XST_SUCCESS != status)
				throw std::runtime_error(__FILE__ ":" LINE_STRING);
			uint32_t addr = frame_buf_base_addr_;
			for (int iFrm = 0; iFrm < drv_inst_.MaxNumFrames; ++iFrm) {
				context_.ReadCfg.FrameStoreStartAddr[iFrm] = addr;
				printf("VDMA Frame %d Addr: 0x%08x\r\n", iFrm, addr);
				// memset((void*)addr, 0, context_.ReadCfg.HoriSizeInput * context_.ReadCfg.VertSizeInput);
				addr += context_.ReadCfg.HoriSizeInput * context_.ReadCfg.VertSizeInput;
			}
			status = XAxiVdma_DmaSetBufferAddr(&drv_inst_, XAXIVDMA_READ, context_.ReadCfg.FrameStoreStartAddr);
			if (XST_SUCCESS != status)
				throw std::runtime_error(__FILE__ ":" LINE_STRING);
			// Clear errors in SR
			XAxiVdma_ClearChannelErrors(&drv_inst_.ReadChannel, XAXIVDMA_SR_ERR_ALL_MASK);
			// Enable read channel error and frame count interrupts
			XAxiVdma_IntrEnable(&drv_inst_, XAXIVDMA_IXR_ERROR_MASK, XAXIVDMA_READ);
		}

		void enableRead()
		{
			XStatus status;
			// Start read channel
			status = XAxiVdma_DmaStart(&drv_inst_, XAXIVDMA_READ);
			if (XST_SUCCESS != status)
				throw std::runtime_error(__FILE__ ":" LINE_STRING);
		}

		void configureWrite(uint16_t h_res, uint16_t v_res)
		{
			XAxiVdma_ClearDmaChannelErrors(&drv_inst_, XAXIVDMA_WRITE, XAXIVDMA_SR_ERR_ALL_MASK);
			XStatus status;
			context_.WriteCfg.HoriSizeInput = h_res * drv_inst_.WriteChannel.StreamWidth;
			context_.WriteCfg.VertSizeInput = v_res;
			context_.WriteCfg.Stride = context_.WriteCfg.HoriSizeInput;
			context_.WriteCfg.FrameDelay = 0;
			context_.WriteCfg.EnableCircularBuf = 1;
			context_.WriteCfg.EnableSync = 1; // Gen-Lock
			context_.WriteCfg.PointNum = 0;
			context_.WriteCfg.EnableFrameCounter = 0;
			context_.WriteCfg.FixedFrameStoreAddr = 0; // ignored, since we circle through buffers
			status = XAxiVdma_DmaConfig(&drv_inst_, XAXIVDMA_WRITE, &context_.WriteCfg);
			if (XST_SUCCESS != status)
				throw std::runtime_error(__FILE__ ":" LINE_STRING);
			uint32_t addr = frame_buf_base_addr_;
			for (int iFrm = 0; iFrm < drv_inst_.MaxNumFrames; ++iFrm) {
				context_.WriteCfg.FrameStoreStartAddr[iFrm] = addr;
				addr += context_.WriteCfg.HoriSizeInput * context_.WriteCfg.VertSizeInput;
			}
			status = XAxiVdma_DmaSetBufferAddr(&drv_inst_, XAXIVDMA_WRITE, context_.WriteCfg.FrameStoreStartAddr);
			if (XST_SUCCESS != status)
				throw std::runtime_error(__FILE__ ":" LINE_STRING);
			// Clear errors in SR
			XAxiVdma_ClearChannelErrors(&drv_inst_.WriteChannel, XAXIVDMA_SR_ERR_ALL_MASK);
			// Unmask error interrupts
			XAxiVdma_MaskS2MMErrIntr(&drv_inst_, ~XAXIVDMA_S2MM_IRQ_ERR_ALL_MASK, XAXIVDMA_WRITE);
			// Enable write channel error and frame count interrupts
			XAxiVdma_IntrEnable(&drv_inst_, XAXIVDMA_IXR_ERROR_MASK, XAXIVDMA_WRITE);
		}

		void enableWrite()
		{
			XStatus status;
			// Start read channel
			status = XAxiVdma_DmaStart(&drv_inst_, XAXIVDMA_WRITE);
			if (XST_SUCCESS != status)
				throw std::runtime_error(__FILE__ ":" LINE_STRING);
		}

		u32 getCurrReadFrame()
		{
			return XAxiVdma_CurrFrameStore(&drv_inst_, XAXIVDMA_READ);
		}

		u32 getCurrWriteFrame()
		{
			return XAxiVdma_CurrFrameStore(&drv_inst_, XAXIVDMA_WRITE);
		}

		u32 getFrameBufferAddr()
		{
			return frame_buf_base_addr_;
		}

		~AXI_VDMA() = default;

		private:
		XAxiVdma drv_inst_;
		vdma_context_t context_;
		uint32_t frame_buf_base_addr_;
		int const RESET_POLL = 1000;
	};

} // namespace digilent

#endif /* AXI_VDMA_H_ */

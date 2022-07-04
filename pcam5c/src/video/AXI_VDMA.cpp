#include <slab/video/AXI_VDMA.hpp>

namespace digilent
{
	template <typename Func>
		static void AXI_VDMA::MyCallback(void* CallbackRef, uint32_t mask_or_type)
		{
			auto pfn = static_cast<Func*>(CallbackRef);
			pfn->operator()(mask_or_type);
		}

	AXI_VDMA::AXI_VDMA(uint16_t dev_id, uint32_t frame_buf_base_addr, IrptCtl& irpt_ctl, uint16_t rd_irpt_id, uint16_t wr_irpt_id) 
	{
		XAxiVdma_Config* psConf;
		XStatus Status;

		psConf = XAxiVdma_LookupConfig(dev_id);
		if (!psConf) {
			throw std::runtime_error(__FILE__ ":" LINE_STRING);
		}

		// Initialize driver instance and reset VDMA
		Status = XAxiVdma_CfgInitialize(&drv_inst_, psConf, psConf->BaseAddress);
		if (Status != XST_SUCCESS) {
			throw std::runtime_error(__FILE__ ":" LINE_STRING);
		}

		// Set error interrupt error handlers, which for some reason need completion handler defined too
		if(rd_irpt_id){
			XAxiVdma_SetCallBack(&drv_inst_, XAXIVDMA_HANDLER_GENERAL,
					reinterpret_cast<void*>(&MyCallback<decltype(rd_handler_)>), &rd_handler_, XAXIVDMA_READ);
			XAxiVdma_SetCallBack(&drv_inst_, XAXIVDMA_HANDLER_ERROR,
					reinterpret_cast<void*>(&MyCallback<decltype(rd_err_handler_)>), &rd_err_handler_, XAXIVDMA_READ);
		}   
		if(wr_irpt_id){
			XAxiVdma_SetCallBack(&drv_inst_, XAXIVDMA_HANDLER_GENERAL,
					reinterpret_cast<void*>(&MyCallback<decltype(wr_handler_)>), &wr_handler_, XAXIVDMA_WRITE);
			XAxiVdma_SetCallBack(&drv_inst_, XAXIVDMA_HANDLER_ERROR,
					reinterpret_cast<void*>(&MyCallback<decltype(wr_err_handler_)>), &wr_err_handler_, XAXIVDMA_WRITE);
		}
		// Register the IIC handler with the interrupt controller
		if(rd_irpt_id){
			printf("vdma dev-id:%d set read channel irpt \n", dev_id);
			irpt_ctl_.registerHandler((uint32_t)rd_irpt_id, (Xil_InterruptHandler)&XAxiVdma_ReadIntrHandler, &drv_inst_);
			irpt_ctl_.enableInterrupt(rd_irpt_id);
		}
		if(wr_irpt_id){
			printf("vdma dev-id:%d set write channel irpt \n", dev_id);
			irpt_ctl_.registerHandler((uint32_t)wr_irpt_id, (Xil_InterruptHandler)&XAxiVdma_WriteIntrHandler, &drv_inst_);
			irpt_ctl_.enableInterrupt(wr_irpt_id);
		}
		irpt_ctl_.enableInterrupts();
	}

	void AXI_VDMA::resetRead()
	{
		// XAxiVdma_ChannelStop(&drv_inst_.ReadChannel);
		// while (XAxiVdma_ChannelIsRunning(&drv_inst_.ReadChannel)) ;

		XAxiVdma_ChannelReset(&drv_inst_.ReadChannel);

		int Polls = RESET_POLL;

		while (Polls && XAxiVdma_ChannelResetNotDone(&drv_inst_.ReadChannel)) {
			--Polls;
		}

		if (!Polls) {
			throw std::runtime_error(__FILE__ ":" LINE_STRING);
		}
	}

	void AXI_VDMA::resetWrite()
	{
		// XAxiVdma_ChannelStop(&drv_inst_.WriteChannel);
		// while (XAxiVdma_ChannelIsRunning(&drv_inst_.WriteChannel)) ;

		XAxiVdma_ChannelReset(&drv_inst_.WriteChannel);

		int Polls = RESET_POLL;

		while (Polls && XAxiVdma_ChannelResetNotDone(&drv_inst_.WriteChannel)) {
			--Polls;
		}

		if (!Polls) {
			throw std::runtime_error(__FILE__ ":" LINE_STRING);
		}
	}

	void AXI_VDMA::configureRead(uint16_t h_res, uint16_t v_res)
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
		for (int iFrm=0; iFrm<drv_inst_.MaxNumFrames; ++iFrm) {
			context_.ReadCfg.FrameStoreStartAddr[iFrm] = addr;
			printf("VDMA Frame %d Addr: 0x%08x\r\n", iFrm, addr);
			// memset((void*)addr,0,context_.ReadCfg.HoriSizeInput * context_.ReadCfg.VertSizeInput);
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

	void AXI_VDMA::enableRead()
	{
		XStatus status;
		// Start read channel
		status = XAxiVdma_DmaStart(&drv_inst_, XAXIVDMA_READ);
		if (XST_SUCCESS != status)
			throw std::runtime_error(__FILE__ ":" LINE_STRING);
	}

	void AXI_VDMA::configureWrite(uint16_t h_res, uint16_t v_res)
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
		//Clear errors in SR
		XAxiVdma_ClearChannelErrors(&drv_inst_.WriteChannel, XAXIVDMA_SR_ERR_ALL_MASK);
		//Unmask error interrupts
		XAxiVdma_MaskS2MMErrIntr(&drv_inst_, ~XAXIVDMA_S2MM_IRQ_ERR_ALL_MASK, XAXIVDMA_WRITE);
		//Enable write channel error and frame count interrupts
		XAxiVdma_IntrEnable(&drv_inst_, XAXIVDMA_IXR_ERROR_MASK, XAXIVDMA_WRITE);
	}

	void AXI_VDMA::enableWrite()
	{
		XStatus status;
		//Start read channel
		status = XAxiVdma_DmaStart(&drv_inst_, XAXIVDMA_WRITE);
		if (XST_SUCCESS != status)
			throw std::runtime_error(__FILE__ ":" LINE_STRING);
	}

	void AXI_VDMA::readHandler(uint32_t irq_types)
	{
		std::cout << "VDMA:read complete" << std::endl;
	}

	void AXI_VDMA::writeHandler(uint32_t irq_types)
	{
		std::cout << "VDMA:write complete" << std::endl;
	}

	void AXI_VDMA::readErrorHandler(uint32_t mask)
	{
		std::cout << "VDMA:read error" << std::endl;
	}

	void AXI_VDMA::writeErrorHandler(uint32_t mask)
	{
		std::cout << "VDMA:write error" << std::endl;
	}

	~AXI_VDMA() = default;

	private:
	XAxiVdma drv_inst_;
	std::function<void(uint32_t)> rd_handler_;
	std::function<void(uint32_t)> wr_handler_;
	std::function<void(uint32_t)> rd_err_handler_;
	std::function<void(uint32_t)> wr_err_handler_;
	vdma_context_t context_;
	uint32_t frame_buf_base_addr_;
	IrptCtl& irpt_ctl_;
	int const RESET_POLL = 1000;
}


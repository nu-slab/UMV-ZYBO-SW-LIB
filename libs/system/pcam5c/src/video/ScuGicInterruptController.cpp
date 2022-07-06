/*
 * IInterruptController.h
 *
 *  Created on: May 27, 2016
 *      Author: Elod
 */

#include <slab/video/ScuGicInterruptController.hpp>
#include <slab/bsp/xscugic.h>

#include <stdexcept>

namespace digilent
{
  ScuGicInterruptController::ScuGicInterruptController(uint16_t dev_id) 
  {
    XScuGic_Config* config = XScuGic_LookupConfig(dev_id);
    if (config == NULL)
      throw std::runtime_error(__FILE__ ":" LINE_STRING);

    XStatus Status;
    // Initialize the Intc driver so that it is ready to use.
    Status = XScuGic_CfgInitialize(&drv_inst_, config, config->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
      throw std::runtime_error(__FILE__ ":" LINE_STRING);
    }
    Status = XScuGic_SelfTest(&drv_inst_);
    if (Status != XST_SUCCESS) {
      throw std::runtime_error(__FILE__ ":" LINE_STRING);
    }
  }

  ScuGicInterruptController::Errc ScuGicInterruptController::enableInterrupts()
  {
    // Connect the interrupt controller interrupt handler to the hardware
    // interrupt handling logic in the ARM processor.

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler) XScuGic_InterruptHandler,
                                 &drv_inst_);

    // Enable interrupts in the ARM
    Xil_ExceptionEnable();

    return XST_SUCCESS;
  }

  ScuGicInterruptController::Errc ScuGicInterruptController::disableInterrupts()
  {
    Xil_ExceptionDisable();
    return XST_SUCCESS;
  }

  template <typename ...Arg>
  ScuGicInterruptController::Errc
  ScuGicInterruptController::registerHandler(uint32_t irpt_id, Xil_InterruptHandler handler, Arg&& ...args)
  {
    XStatus Status;

    Status = XScuGic_Connect(&drv_inst_, irpt_id,
                             (Xil_InterruptHandler) handler,
                             std::forward<Arg>(args)...);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }

    // Enable the interrupts for the IIC device.
    XScuGic_Enable(&drv_inst_, irpt_id);

    return XST_SUCCESS;
  }

  ScuGicInterruptController::Errc ScuGicInterruptController::disableInterrupt(uint32_t irpt_id)
  {
    XScuGic_Disable(&drv_inst_, irpt_id);
    return XST_SUCCESS;
  }

  ScuGicInterruptController::Errc ScuGicInterruptController::enableInterrupt(uint32_t irpt_id)
  {
    XScuGic_Enable(&drv_inst_, irpt_id);
    return XST_SUCCESS;
  }
} /* namespace digilent */

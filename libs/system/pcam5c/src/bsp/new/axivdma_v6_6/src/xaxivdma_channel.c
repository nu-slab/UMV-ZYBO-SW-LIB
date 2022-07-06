/******************************************************************************
*
* Copyright (C) 2012 - 2018 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xaxivdma_channel.c
* @addtogroup axivdma_v6_6
* * @{
*
* Implementation of the channel related functions. These functions are used
* internally by the driver, and are declared in xaxivdma_i.h.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a jz   08/16/10 First release
* 2.00a jz   12/10/10 Added support for direct register access mode, v3 core
* 2.01a jz   01/19/11 Added ability to re-assign BD addresses
* 	rkv  03/28/11 XAxiVdma_ChannelInit API is changed.
* 3.02a srt  08/26/11 - XAxiVdma_ChannelErrors API is changed to support for
*			Flush on Frame Sync feature.
*		      - Two flags, XST_VDMA_MISMATCH_ERROR & XAXIVDMA_MIS
*			MATCH_ERROR are added to report error status when
*			Flush on Frame Sync feature is enabled.
* 4.00a srt  11/21/11 - XAxiVdma_ChannelSetBufferAddr API is changed to
*			support 32 Frame Stores.
*		      - XAxiVdma_ChannelConfig API is changed to support
*			modified Park Offset Register bits.
*		      - Added APIs:
*			XAxiVdma_ChannelHiFrmAddrEnable()
*			XAxiVdma_ChannelHiFrmAddrDisable()
* 4.01a srt  06/13/12 - Added API:
*			XAxiVdma_ClearChannelErrors()
*			XAxiVdma_ChannelGetEnabledIntr()
*		      - XAxiVdma_ChannelErrors API is changed to remove
*			Mismatch error logic.
*		      - Removed Error checking logic in the APIs. Provided
*			User APIs to do this.
* 4.04a srt  03/03/13 - Changes for IPv5.04a:
*			Support for the GenlockRepeat Control bit (Bit 15)
*                       (CR: 691391)
*	     	- Support for *_ENABLE_DEBUG_INFO_* debug configuration
*			parameters (CR: 703738)
* 4.05a srt  04/26/13 - Added unalignment checks for Hsize and Stride
*			(CR 710279)
* 6.6   rsp  07/02/18 - Program vertical flip state for S2MM channel
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xaxivdma_i.h"
#include "xaxivdma.h"

/************************** Constant Definitions *****************************/
/* Number of polling loops to do to check for reset completion
 *
 * This number is large enough to cover the maximum transfer length
 *
 * However, if the memory operation being throttled by the system, this number
 * is not large enough
 */
#define XAXIVDMA_RESET_POLLING      1000



/** @} */

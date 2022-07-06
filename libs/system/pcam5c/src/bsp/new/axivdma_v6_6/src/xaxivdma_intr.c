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
* @file xaxivdma_intr.c
* @addtogroup axivdma_v6_6
* @{
*
* Implementation of interrupt related functions.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a jz   08/16/10 First release
* 2.00a jz   12/10/10 Added support for direct register access mode, v3 core
* 2.01a jz   01/19/11 Added ability to re-assign BD addresses
* 4.01a srt  06/13/12 Modified the logic of Error handling in interrupt
*		      handlers.
* 4.06a srt  04/09/13 Added API XAxiVdma_MaskS2MMErrIntr which will mask
*		      the S2MM interrupt for the error mask provided.
*		      (CR 734741)
* 6.3   ms   02/20/17 Fixed compilation error. CR-969129.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xaxivdma.h"
#include "xaxivdma_i.h"


/** @} */

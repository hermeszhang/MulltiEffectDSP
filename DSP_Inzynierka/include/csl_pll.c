/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


/** @file csl_pll.c
 *
 *  @brief PLL functional layer API source file
 *
 *  Path: \(CSLPATH)\src\
 */

/* ============================================================================
 * Revision History
 * ================
 * 22-Aug-2008 Created
 * 13-Aug-2010 CSL v2.10 release
 * 06-Jul-2011 CSL v2.50 release
 * 24-Jul-2012 Added C5517 configuration sequence
 * 25-Jul-2012 Modified PLL_config to match system user guide
 * 13-Sep-2012 CSL v3.00 release
 * 20-Dec-2012 CSL v3.01 release
 * ============================================================================
 */

#include "csl_pll.h"
#include "csl_pllAux.h"

/** ============================================================================
 *   @n@b PLL_init
 *   @b Description
 *   @n This is the initialization function for the pll CSL. The function
 *      must be called before calling any other API from this CSL. This
 *      will initialize the PLL object.
 *
 *   @b Arguments
 *   @verbatim
            pllObj          Pointer to PLL object.
            pllInstId       Instance number of the PLL.
    @endverbatim
 *
 *   <b> Return Value </b> CSL_Status
 *   @li                       	       CSL_SOK - Init call is successful
 *   @li                               CSL_ESYS_INVPARAMS- Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   PLL object structure is populated
 *
 *   @b Modifies
 *   @n Handle is modified
 *
 *   @b Example
 *   @verbatim
         PLL_Obj                 pllObj;
         CSL_Status              status;
         Uint32                  pllInstId;
         pllInstId = 0;
         status = PLL_init(&pllObj,pllInstId);
     @endverbatim
 *  ============================================================================
 */
CSL_Status PLL_init( PLL_Obj * pllObj,
                     Uint32 pllInstId)

{
    CSL_Status    status;

    status = CSL_ESYS_INVPARAMS;

    if(NULL != pllObj)
    {
        switch (pllInstId)
        {
            case CSL_PLL_INST_0:
                pllObj->instId = pllInstId;
                pllObj->sysAddr = CSL_SYSCTRL_REGS;
				status = CSL_SOK;
                break;
           /* Invalid instance number */
            default:
                break;
		}
     }

	return (status);
}

/** ============================================================================
 *   @n@b PLL_config
 *
 *   @b Description
 *   @n This API is used to configure the PLL
 *
 *   @b Arguments
 *   @verbatim
           hPll             Handle to the pll
           pconfigInfo      pointer to PLL_config structure.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Configuring the pll is successful
 *
 *   @li                    CSL_ESYS_BADHANDLE  - The handle passed is invalid
 *
 *   @li                    CSL_ESYS_INVPARAMS  - The pconfigInfo is NULL
 *
 *   <b> Pre Condition </b>
 *   @n  PLL_init should be successfully called.
 *
 *   <b> Post Condition </b>
 *   @n  Configures the PLL registers.
 *
 *   @b Modifies
 *   @n hPll variable
 *
 *   @b Example
 *   @verbatim
         CSL_Status              status;
         PLL_Obj                 pllObj;
         PLL_Config              configInfo;
         PLL_Handle              hPll;
         Uint32                  pllInstId;
         pllInstId = 0;
         status = PLL_init(&pllObj,pllInstId);
         hPll = &pllObj;
	     .......
         Configure the PLL for 12.288MHz on C5505/15
         configInfo.PLLCNTL1 = 0x82ed;
         configInfo.PLLINCNTL = 0x8000;
         configInfo.PLLCNTL2 = 0x0806;
         configInfo.PLLOUTCNTL = 0x0200;
         status = PLL_config(hPll, &configInfo);
     @endverbatim
 *  ============================================================================
 */

CSL_Status PLL_config(PLL_Handle hPll,
                      PLL_Config *pconfigInfo)
{
    Uint16 timeout = TIMEOUT;
	CSL_Status status = CSL_SOK;

	if(NULL == hPll)
    {
		status = CSL_ESYS_BADHANDLE;
		return status;
	}

	if(NULL == pconfigInfo)
    {
		status = CSL_ESYS_INVPARAMS;
		return status;
	}

	hPll->pllConfig = pconfigInfo;

	    /* Force to BYPASS mode */
    CSL_FINST(hPll->sysAddr->CCR2, SYS_CCR2_SYSCLKSEL, BYPASS);


	/* Set RSVD = 0 in CGCR1 */
    CSL_FINST(hPll->sysAddr->CGCR1, SYS_CGCR1_RSVD, CLEAR);
	/*Program RDRATIO, M, and RDBYPASS in CGCR1 and CGCR2*/
	hPll->sysAddr->CGCR2 = pconfigInfo->PLLINCNTL;
	CSL_FINS(hPll->sysAddr->CGCR1, SYS_CGCR1_M,(pconfigInfo->PLLCNTL1 & CSL_SYS_CGCR1_M_MASK));
	/*Program ODRATIO and OUTDIVEN in CGCR4*/
	hPll->sysAddr->CGCR4 = pconfigInfo->PLLOUTCNTL;
	/*Write 0806g to CGCR3*/
	hPll->sysAddr->CGCR3 = pconfigInfo->PLLCNTL2;
    /*Set PLL_PWRDN = 0*/
    CSL_FINS(hPll->sysAddr->CGCR1, SYS_CGCR1_PLL_PWRDN, CSL_FEXT(pconfigInfo->PLLCNTL1, SYS_CGCR1_PLL_PWRDN));
	//CSL_FINST(hPll->sysAddr->CGCR1, SYS_CGCR1_PLL_PWRDN, POWERED);
	/* Set RSVD = 1 in CGCR1 */
    CSL_FINST(hPll->sysAddr->CGCR1, SYS_CGCR1_RSVD, SET);


	/*Wait 4ms for the PLL to complete its phase-locking sequence*/
	while (timeout--) ;
	/* Select pll */
    CSL_FINST(hPll->sysAddr->CCR2, SYS_CCR2_SYSCLKSEL, LOCK);

	return (status);
}

/** ============================================================================
 *   @n@b PLL_enable
 *
 *   @b Description
 *   @n This API is used to enable the PLL
 *
 *   @b Arguments
 *   @verbatim
            hPll        Handle to the pll
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             -  Enabling the PLL is successful
 *
 *   @li                    CSL_ESYS_BADHANDLE  - The handle passed is invalid
 *
 *   <b> Pre Condition </b>
 *   @n  PLL_init and PLL_config should be called successfully.
 *
 *   <b> Post Condition </b>
 *   @n  Pll is enabled
 *
 *   @b Modifies
 *   @n hPll variable
 *
 *   @b Example
 *   @verbatim
         CSL_Status              status;
         PLL_Obj                 pllObj;
         PLL_Config              configInfo;
         PLL_Handle              hPll;
         Uint32                  pllInstId;
         pllInstId = 0;
         status = PLL_init(&pllObj,pllInstId);
         hPll = &pllObj;
	     .......
         Configure the PLL for 12.288MHz
         configInfo.PLLCNTL1 = 0x82ed;
         configInfo.PLLINCNTL = 0x8000;
         configInfo.PLLCNTL2 = 0x0806;
         configInfo.PLLOUTCNTL = 0x0200;
         status = PLL_config(hPll, &configInfo);
         status = PLL_enable(hPll);

     @endverbatim
 *  ============================================================================
 */

CSL_Status PLL_enable(PLL_Handle    hPll)
{
	CSL_Status    status;
    Uint16        timeout;

	status  = CSL_SOK;
    timeout = TIMEOUT;

	if(NULL == hPll)
    {
		status = CSL_ESYS_BADHANDLE;
		return (status);
	}

	/* Enable the PLL */
    CSL_FINST(hPll->sysAddr->CGCR1, SYS_CGCR1_PLL_PWRDN, POWERED);

    while (!PLL_getTestLockMonStatus(hPll) && timeout--) ;

    /* Select pll */
    CSL_FINST(hPll->sysAddr->CCR2, SYS_CCR2_SYSCLKSEL, LOCK);

	return (status);
}

/** ============================================================================
 *   @n@b PLL_bypass
 *
 *   @b Description
 *   @n This API is used to Bypass the PLL.
 *
 *   @b Arguments
 *   @verbatim
            hPll        Handle to the pll
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             -  Bypassing the PLL is successful
 *
 *   @li                    CSL_ESYS_BADHANDLE  - The handle passed is invalid
 *
 *   <b> Pre Condition </b>
 *   @n  PLL_init and PLL_config should be called successfully.
 *
 *   <b> Post Condition </b>
 *   @n  Pll is bypassed
 *
 *   @b Modifies
 *   @n hPll variable
 *
 *   @b Example
 *   @verbatim
         CSL_Status              status;
         PLL_Obj                 pllObj;
         PLL_Config              configInfo;
         PLL_Handle              hPll;
         Uint32                  pllInstId;
         pllInstId = 0;
         status = PLL_init(&pllObj,pllInstId);
         hPll = &pllObj;
	     .......
         Configure the PLL for 12.288MHz
         configInfo.PLLCNTL1 = 0x82ed;
         configInfo.PLLINCNTL = 0x8000;
         configInfo.PLLCNTL2 = 0x0806;
         configInfo.PLLOUTCNTL = 0x0200;
         status = PLL_config(hPll, &configInfo);
         status = PLL_bypass(hPll);
     @endverbatim
 *  ============================================================================
 */
CSL_Status PLL_bypass(PLL_Handle hPll)
{

    CSL_Status status;

    status = CSL_SOK;

	if(NULL == hPll)
    {
		status = CSL_ESYS_BADHANDLE;
		return status;
	}

    /* Bypass the PLL */
    CSL_FINST(hPll->sysAddr->CCR2, SYS_CCR2_SYSCLKSEL, BYPASS);

	CSL_FINST(hPll->sysAddr->CGCR1, SYS_CGCR1_PLL_PWRDN, POWERDWN);

	return (status);
}

/** ============================================================================
 *   @n@b PLL_reset
 *
 *   @b Description
 *   @n Resets all the PLL registers.
 *
 *   @b Arguments
 *   @verbatim
            hPll        Handle to the pll
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             -         Resetting the PLL is successful.
 *
 *   @li                    CSL_ESYS_BADHANDLE  - The handle passed is invalid
 *
 *
 *   <b> Pre Condition </b>
 *   @n  PLL_init should be called successfully.
 *
 *   <b> Post Condition </b>
 *   @n  PLL registers are resetted.
 *
 *   @b Modifies
 *   @n hPll variable
 *
 *   @b Example
 *   @verbatim
            CSL_Status              status;
 	        PLL_Obj                 pllObj;
            Uint32                  pllInstId;
            PLL_Handle              hPll;
            pllInstId = 0;
            status = PLL_init(&pllObj,pllInstId);
          	.....
            hPll = &pllObj;
            status = PLL_reset(hPll);
     @endverbatim
 *  ============================================================================
 */
CSL_Status PLL_reset(PLL_Handle hPll)
{
	CSL_Status status;

	status = CSL_SOK;

	if(NULL == hPll)
    {
		status = CSL_ESYS_BADHANDLE;
		return (status);
	}

    /* Bypass the PLL */
    CSL_FINST(hPll->sysAddr->CCR2, SYS_CCR2_SYSCLKSEL, BYPASS);

	/* Reset PLL register */
    hPll->sysAddr->CGCR1 = CSL_SYS_CGCR1_RESETVAL;
	hPll->sysAddr->CGCR2 = CSL_SYS_CGCR2_RESETVAL;
	hPll->sysAddr->CGCR4 = CSL_SYS_CGCR4_RESETVAL;
	hPll->sysAddr->CGCR3 = CSL_SYS_CGCR3_RESETVAL;

	return (status);
}

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    none
 *
 *  \return   System clock value in Hz
 */

Uint32 getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Uint32    sysClk;
	Uint16    pllM;
	Uint16    pllRD;
	Uint16    pllOD;

	pllM = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_M);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllOD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);

	sysClk = CSL_PLL_CLOCKIN;

	if (0 == pllRDBypass)
	{
		sysClk = sysClk/(pllRD + 4);
	}

	sysClk = (sysClk * (pllM + 4));

	if (1 == pllOutDiv)
	{
		sysClk = sysClk/(pllOD + 1);
	}

	/* Return the value of system clock in KHz */
	return(sysClk/1000);
}





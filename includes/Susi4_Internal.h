/*******************************************************************************
* Advantech SUSI4 Driver License
* Copyright c 2018 Advantech Co., Ltd. All rights reserved.

* This Software can only be used on Advantech Products.
* You may use and distribute this Software under the terms of Advantech Software License Agreement.
* Advantech has the right to review all the modifications of this Software.
* Redistribution and use in source forms, with or without modification, are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice and the following disclaimer. 
*Advantech Co., Ltd. may not be used to endorse or promote products derived from this Software without specific prior written permission.

*THIS SOFTWARE IS PROVIDED ¡§AS IS¡¨, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR *OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THIS SOFTWARE OR THE USE OR OTHER *DEALINGS IN THIS SOFTWARE. 

*******************************************************************************/ 

#ifdef __cplusplus
extern "C" {
#endif

/* Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// pBufLen == NULL							| SUSI_STATUS_INVALID_PARAMETER
// Length != 1, 2 or 4						| SUSI_STATUS_INVALID_PARAMETER
// Else										| SUSI_STATUS_SUCCESS
*/

/* Resume Type */
#define SUSI_ID_PWR_RESUME_TYPE_NA				0x0		//Function is off
#define SUSI_ID_PWR_RESUME_TYPE_S3				0x3		//Sleep
#define SUSI_ID_PWR_RESUME_TYPE_S4				0x4		//Hibernation
#define SUSI_ID_PWR_RESUME_TYPE_G5				0x5		//Power off
#define SUSI_ID_PWR_RESUME_TYPE_G3				0x6		//Mechanical off

SusiStatus_t SUSI_API SusiBoardSetPWRCycle(
	uint32_t Delaytime,	/* IN	Power resume alarm time */
	uint8_t Eventype	/* IN	Power resume type */
	);

SusiStatus_t SUSI_API SusiBoardGetPWRCycle(
	uint32_t *Delaytime,	/* OUT	Power resume alarm time */
	uint8_t *Eventype		/* OUT	Power resume type */
	);

SusiStatus_t SUSI_API SusiBoardReadPCI(
	uint8_t Bus,		/* IN	PCI Bus number */
	uint8_t Device,		/* IN	PCI Device number */
	uint8_t Function,	/* IN	PCI Function number */
	uint32_t Offset,	/* IN	Configuration space offset */
	uint8_t *pData,		/* OUT	Data point */
	uint32_t Length		/* IN	Data length */
	);

SusiStatus_t SUSI_API SusiBoardWritePCI(
	uint8_t Bus,		/* IN	PCI Bus number */
	uint8_t Device,		/* IN	PCI Device number */
	uint8_t Function,	/* IN	PCI Function number */
	uint32_t Offset,	/* IN	Configuration space offset */
	uint8_t *pData,		/* IN	Data point */
	uint32_t Length		/* IN	Data length */
	);

SusiStatus_t SUSI_API SusiBoardReadIO(
	uint16_t Port,		/* IN	IO port address */
	uint32_t *pValue,	/* OUT	Value point */
	uint32_t Length		/* IN	Value length, should be 1, 2 or 4 */
	);

SusiStatus_t SUSI_API SusiBoardWriteIO(
	uint16_t Port,		/* IN	IO port address */
	uint32_t Value,		/* IN	Value */
	uint32_t Length		/* IN	Value length, should be 1, 2 or 4 */
	);

SusiStatus_t SUSI_API SusiBoardReadMemory(
	uint32_t Address,	/* IN	Memory address */
	uint8_t *pData,		/* OUT	Data point */
	uint32_t Length		/* IN	Data length */
	);

SusiStatus_t SUSI_API SusiBoardWriteMemory(
	uint32_t Address,	/* IN	Memory address */
	uint8_t *pData,		/* IN	Data point */
	uint32_t Length		/* IN	Data length */
	);

SusiStatus_t SUSI_API SusiBoardReadMSR(
	uint32_t index,		/* IN	CPU MSR index */
	uint32_t *EAX_reg,	/* OUT	CPU register EAX */
	uint32_t *EDX_reg	/* OUT	CPU register EFX */
	);

SusiStatus_t SUSI_API SusiBoardWriteMSR(
	uint32_t index,	/* IN	CPU MSR index */
	uint32_t EAX_reg,	/* IN	CPU register EAX */
	uint32_t EDX_reg	/* IN	CPU register EDX */
	);

#ifdef __cplusplus
}
#endif


# NVIC functions

|     CMSIS function                  |   Description                                             |
|-------------------------------------|-----------------------------------------------------------|
| void NVIC_EnableIRQ(IRQn_Type IRQn) |   Enables an interrupt or exception.                      |
| void NVIC_DisableIRQ(IRQn_Type IRQn)	| Disables an interrupt or exception. | 
| void NVIC_SetPendingIRQ(IRQn_Type IRQn) | 	Sets the pending status of interrupt or exception to 1. | 
| void NVIC_ClearPendingIRQ(IRQn_Type IRQn) | 	Clears the pending status of interrupt or exception to 0. | 
| uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) | Reads the pending status of interrupt or exception. This function returns non- zero value if the pending status is set to 1. | 
| void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) | Sets the priority of an interrupt or exception with configurable priority level to 1. | 
| uint32_t NVIC_GetPriority(IRQn_Type IRQn) |  Reads the priority of an interrupt or exception with configurable priority level. This function return the current priority level. | 
| void 	NVIC_SystemReset (void)        | 	Reset the system. |


# Special functions 
| CMSIS function                      | Instruction         | Description          |
|-------------------------------------|---------------------|-----------------------|
| void __enable_irq(void) 	           | CPSIE I             |	 Change processor state - enable interrupts                     |
| void __disable_irq(void) 	           |  CPSID I            |	 Change processor state - disable interrupts                      |
| void __enable_fault_irq(void)	   |  CPSIE F            |	 Change processor state - enable fault interrupts                 |
| void __disable_fault_irq(void)      |  CPSID F            |  Change processor state - disable fault interrupts                |
| void __ISB(void)	                   |  ISB                |  Instruction Synchronization Barrier  |
| void __DSB(void)	                   |  DSB                |  Data Synchronization Barrier                    |
| void __DMB(void)	                   |  DMB                |  Data Memory Barrier                     |
| uint32_t __REV(uint32_t int value)	|  REV                |  Reverse Byte Order in a Word                     |
| uint32_t __REV16(uint32_t int value)	|  REV16              |  Reverse Byte Order in each Half-Word                     |
| uint32_t __REVSH(uint32_t int value)	|  REVSH              |  Reverse byte order in bottom halfword and sign extend    |
| uint32_t __RBIT(uint32_t int value)	|  RBIT               |  Reverse bits                    |
| void __SEV(void)	                    | SEV                |  Send Event                   |
| void __WFE(void)	                    | WFE                |  Wait for Event                    |
| void __WFI(void)	                    | WFI                |  Wait for Interrupt                   |


## Register access
|    CMSIS function                     |       Description           |
|---------------------------------------|-----------------------------|
| uint32_t __get_PRIMASK (void)         |    Read PRIMASK  |
| __set_PRIMASK (uint32_t value)        |    Write PRIMASK |
| uint32_t __get_FAULTMASK (void)       | Read Write |
| void __set_FAULTMASK (uint32_t value) | Write Write |
| uint32_t __get_BASEPRI (void)         | Read BASEPRI |
| void __set_BASEPRI (uint32_t value)   | Write BASEPRI |
| uint32_t __get_CONTROL (void)         | Read CONTROL |
| void __set_CONTROL (uint32_t value)   | Write CONTROL |
| uint32_t __get_MSP (void)             | Read MSP |
| void __set_MSP (uint32_t TopOfMainStack)  | Write MSP |
| uint32_t __get_PSP (void)             | Read PSP |
| void __set_PSP (uint32_t TopOfProcStack)  | Write PSP |



##NVIC

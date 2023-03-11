/*
 * SCEDULER.h
 *
 *  Created on: Mar 10, 2023
 *      Author: MOHAMMED ALSHREIF
 */

#ifndef INC_SCEDULER_H_
#define INC_SCEDULER_H_
#include "RTOS_CORTEX_M3_PORTING.h"

//========================================================
typedef enum {
	no_error,
	RTOS_INIT_ERROR
}OS_ERRORS;

//===================TASK frame============================
typedef struct { //TASK frame

	uint32_t stack_size;
	uint8_t priority;
	void (*TASK_FUNCTION)();
	uint32_t _PSP_STACK_TOP;//not entered by the user
	uint32_t _PSP_STACK_END;//not entered by the user
	uint32_t* _PSP_STACK_CURENT;//not entered by the user
	char NAME[15];
	enum {
		Suspend,
		Wating,
		Ready,
		Running
	}State; //not entered by the user

	struct{
		enum {
			enable,
			disable
		}blocking;
		uint32_t TICKS;
	}wating_time;

}TASK_FRAME_t;


//=================== API ===========================

OS_ERRORS ALSHREIF_RTOS_INIT();
void ALSHREIF_RTOS_CREAT_TASK(TASK_FRAME_t* TASK);
void ALSHREIF_RTOS_ACTIVAT_TASK(TASK_FRAME_t* TASK);
void ALSHREIF_RTOS_TERMINAT_TASK(TASK_FRAME_t* TASK);
#endif /* INC_SCEDULER_H_ */

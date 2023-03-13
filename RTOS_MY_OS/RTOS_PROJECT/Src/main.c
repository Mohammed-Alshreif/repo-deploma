#include "SCEDULER.h"
#include "GPIO_DRIVER.h"
TASK_FRAME_t TASK1,TASK2,TASK3;
uint8_t T1,T2,T3;
void TASK1_fun(){
	while(1){
		T1^=1;
		TogglePin(GPIOA, pin7);
	}
}
void TASK2_fun(){
	while(1){
		T2^=1;
		TogglePin(GPIOB, pin0);
	}
}
void TASK3_fun(){
	while(1){
		T3^=1;
		TogglePin(GPIOB, pin1);
	}
}
int main(void)
{
	HARD_WARE_INIT();
	ALSHREIF_RTOS_INIT();

	strcpy(TASK1.NAME,"TASK1");
	TASK1.TASK_FUNCTION=TASK1_fun;
	TASK1.priority=2;
	TASK1.stack_size=1024;

	strcpy(TASK2.NAME,"TASK2");
	TASK2.TASK_FUNCTION=TASK2_fun;
	TASK2.priority=2;
	TASK2.stack_size=1024;

	strcpy(TASK3.NAME,"TASK3");
	TASK3.TASK_FUNCTION=TASK3_fun;
	TASK3.priority=2;
	TASK3.stack_size=1024;

	ALSHREIF_RTOS_CREAT_TASK(&TASK1);
	ALSHREIF_RTOS_CREAT_TASK(&TASK2);
	ALSHREIF_RTOS_CREAT_TASK(&TASK3);

	ALSHREIF_RTOS_ACTIVAT_TASK(&TASK1);
	ALSHREIF_RTOS_ACTIVAT_TASK(&TASK2);
	ALSHREIF_RTOS_ACTIVAT_TASK(&TASK3);

	ALSHREIF_RTOS_START_OS();
	while(1);
}

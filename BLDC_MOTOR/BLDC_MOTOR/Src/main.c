#include "STM32_TIMERS_DRIVER.h"
#include "stm32_speed_DRIVER.h"
#include "stm32f103c6_ISR_DRIVER.h"
#include "stm32f103c6_ADC.h"
#include "stm32f103c6_USART_SRIVER.h"
#define  clk 36000000
#define NUM_OF_HALLSENSOR_PULS 30
void state();
void wate();
void NextStep();
void NextStep_REVERS();
void MOTOR_STOP();

uint8_t MOTOR_state =0;
uint8_t hall1state=1;
uint8_t hall2state=0;
uint8_t hall3state=1;

uint8_t MOTOR_DIRECTION;

uint32_t adcread;
uint32_t TIME_CALC;
uint8_t buf[7];

int encoder_pulses,MOTOR_SPEED_RPM;
uint8_t stop_flage,flag_SPEED;
void s1(){
	hall1state=READ_PIN(GPIOA, pin0);

	int X = READ_PIN(GPIOA, pin1);
	if(hall1state==1){
		if(X > 0){
			encoder_pulses++;
		}
		else{
			encoder_pulses--;
		}
	}

	if(MOTOR_DIRECTION==0){
		NextStep();
	}
	else {
		NextStep_REVERS();
	}

}
void s2(){

	if(flag_SPEED==0){
		TIME_CALCULATION(clk, TIMER_START);
		flag_SPEED=1;
	}
	else{
		TIME_CALC=TIME_CALCULATION(clk, TIMER_STOP);
		flag_SPEED=0;
	}

	hall2state=READ_PIN(GPIOA, pin1);

	if(MOTOR_DIRECTION==0){
		NextStep();
	}
	else {
		NextStep_REVERS();
	}
}
void s3(){
	for(int i=0;i<500;i++);
	hall3state=READ_PIN(GPIOA, pin2);
	if(MOTOR_DIRECTION==0){
		NextStep();
	}
	else {
		NextStep_REVERS();
	}
}

void NextStep() {
	if ((hall1state == 1) && (hall2state == 0) && (hall3state == 1)) {
		MOTOR_state = 0;
	}
	if ((hall1state == 0) && (hall2state == 0) && (hall3state == 1)) {
		MOTOR_state = 1;
	}
	if ((hall1state == 0) && (hall2state == 1) && (hall3state == 1)) {
		MOTOR_state = 2;
	}
	if ((hall1state == 0) && (hall2state == 1) && (hall3state == 0)) {
		MOTOR_state = 3;
	}
	if ((hall1state == 1) && (hall2state == 1) && (hall3state == 0)) {
		MOTOR_state = 4;
	}
	if ((hall1state == 1) && (hall2state == 0) && (hall3state == 0)) {
		MOTOR_state = 5;
	}

	TIMER_ISR(TIM3,50, U_us, clk, wate);
}
void NextStep_REVERS() {
	if ((hall2state == 1) && (hall1state == 0) && (hall3state == 1)) {
		MOTOR_state = 5;
	}
	if ((hall2state == 0) && (hall1state == 0) && (hall3state == 1)) {
		MOTOR_state = 4;
	}
	if ((hall2state == 0) && (hall1state == 1) && (hall3state == 1)) {
		MOTOR_state = 3;
	}
	if ((hall2state == 0) && (hall1state == 1) && (hall3state == 0)) {
		MOTOR_state = 2;
	}
	if ((hall2state == 1) && (hall1state == 1) && (hall3state == 0)) {
		MOTOR_state = 1;
	}
	if ((hall2state == 1) && (hall1state == 0) && (hall3state == 0)) {
		MOTOR_state = 0;
	}

	TIMER_ISR(TIM3,50, U_us, clk, wate);
}

void wate(){
	//A floating
	pinwrite(GPIOB, pin0,LOW);
	pinwrite(GPIOB, pin1,HIGH);
	//B floating
	pinwrite(GPIOB, pin10,LOW);
	pinwrite(GPIOB, pin7,HIGH);
	//C floating
	pinwrite(GPIOB, pin8,LOW);
	pinwrite(GPIOB, pin5,HIGH);
	TIMER_ISR(TIM3,500, U_us, clk, state);
}
void state(){
	switch (MOTOR_state) {
	case 0:
		//MOTOR_state++;
		//A h
		pinwrite(GPIOB, pin0,HIGH);
		pinwrite(GPIOB, pin1,HIGH);
		//B floating
		pinwrite(GPIOB, pin10,LOW);
		pinwrite(GPIOB, pin7,HIGH);
		//C LOW
		pinwrite(GPIOB, pin8,LOW);
		pinwrite(GPIOB, pin5,LOW);
		break;

	case 1:
		//MOTOR_state++;
		//A floating
		pinwrite(GPIOB, pin0,LOW);
		pinwrite(GPIOB, pin1,HIGH);
		//B h
		pinwrite(GPIOB, pin10,HIGH);
		pinwrite(GPIOB, pin7,HIGH);
		//C LOW
		pinwrite(GPIOB, pin8,LOW);
		pinwrite(GPIOB, pin5,LOW);
		break;

	case 2:
		//MOTOR_state++;
		//A l
		pinwrite(GPIOB, pin0,LOW);
		pinwrite(GPIOB, pin1,LOW);
		//B h
		pinwrite(GPIOB, pin10,HIGH);
		pinwrite(GPIOB, pin7,HIGH);
		//C floating
		pinwrite(GPIOB, pin8,LOW);
		pinwrite(GPIOB, pin5,HIGH);
		break;

	case 3:
		//MOTOR_state++;
		//A l
		pinwrite(GPIOB, pin0,LOW);
		pinwrite(GPIOB, pin1,LOW);
		//B floating
		pinwrite(GPIOB, pin10,LOW);
		pinwrite(GPIOB, pin7,HIGH);
		//C h
		pinwrite(GPIOB, pin8,HIGH);
		pinwrite(GPIOB, pin5,HIGH);
		break;

	case 4:
		//MOTOR_state++;
		//A floating
		pinwrite(GPIOB, pin0,LOW);
		pinwrite(GPIOB, pin1,HIGH);
		//B l
		pinwrite(GPIOB, pin10,LOW);
		pinwrite(GPIOB, pin7,LOW);
		//C h
		pinwrite(GPIOB, pin8,HIGH);
		pinwrite(GPIOB, pin5,HIGH);
		break;

	case 5:
		//MOTOR_state=0;
		//A h
		pinwrite(GPIOB, pin0,HIGH);
		pinwrite(GPIOB, pin1,HIGH);
		//B l
		pinwrite(GPIOB, pin10,LOW);
		pinwrite(GPIOB, pin7,LOW);
		//C floating
		pinwrite(GPIOB, pin8,LOW);
		pinwrite(GPIOB, pin5,HIGH);
		break;
	}
	TIMER_ISR(TIM3,adcread, U_us, clk, wate);

}

int main (){
	MCAL_speed_init(CLK_SORC_IN_CLK8MHz, mood_FAST_MOOD_36MHzCORE);
	pinmode(GPIOB, pin0,GPIO_MODE_OUTPUT_push_pull_Speed10);
	pinmode(GPIOB, pin1,GPIO_MODE_OUTPUT_push_pull_Speed10);
	pinmode(GPIOB, pin10,GPIO_MODE_OUTPUT_push_pull_Speed10);
	pinmode(GPIOB, pin7,GPIO_MODE_OUTPUT_push_pull_Speed10);
	pinmode(GPIOB, pin8,GPIO_MODE_OUTPUT_push_pull_Speed10);
	pinmode(GPIOB, pin5,GPIO_MODE_OUTPUT_push_pull_Speed10);
	ADC_INIT(ADC1, ADC_STATE_START);
	EXTI(GPIOA, pin0,EXTI_Trigger_RisingAndFalling, s1);
	EXTI(GPIOA, pin1,EXTI_Trigger_RisingAndFalling, s2);
	EXTI(GPIOA, pin2,EXTI_Trigger_RisingAndFalling, s3);
	USART_INIT(USART1, mode_RX_TX_ENABLE, word_length_9, stop_bits_2, baud_rate_9600, parity_even,clk);
	pinmode(GPIOA, pin8,GPIO_MODE_INPUT_PU);

	if(READ_PIN(GPIOA, pin8)==0){
		MOTOR_DIRECTION=1;
	}
	else {
		MOTOR_DIRECTION=0;
	}

	s1();
	s2();
	s3();
	//soft start
	for(int i=0;i<100;i++){
		adcread+=2;
		delay(2, U_ms, clk);
	}

	while(1){
		adcread=ADC_READ(ADC1, ADC_pin_PA5)/2;
		delay(190, U_ms, clk);
		MOTOR_SPEED_RPM=60000000/(TIME_CALC*NUM_OF_HALLSENSOR_PULS);
		TIME_CALC=0;
		sprintf (buf, "%d",MOTOR_SPEED_RPM);
		USART_SEND_STRING(USART1,buf);
		delay(3, U_ms, clk);
		USART_SEND_STRING(USART1," \n");
		if(READ_PIN(GPIOA, pin8)==0){
			MOTOR_DIRECTION=1;
		}
		else {
			MOTOR_DIRECTION=0;
		}
	}

}

void MOTOR_STOP(){
	//A h
	pinwrite(GPIOB, pin0,LOW);
	pinwrite(GPIOB, pin1,LOW);
	//B floating
	pinwrite(GPIOB, pin10,LOW);
	pinwrite(GPIOB, pin7,LOW);
	//C LOW
	pinwrite(GPIOB, pin8,LOW);
	pinwrite(GPIOB, pin5,LOW);
	stop_flage=1;
}

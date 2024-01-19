//*******************************************************
//BY/MOHAMMED ADEL ALSHREIF
//AMR MANSOURA SIEMENS BLDC MOTOR CONTROL
// 2022/10/1
//*******************************************************
#include "STM32_TIMERS_DRIVER.h"
#include "stm32_speed_DRIVER.h"
#include "stm32f103c6_ISR_DRIVER.h"
#include "stm32f103c6_ADC.h"
#include "stm32f103c6_USART_SRIVER.h"
#include "math.h"
//*******************************************************
#define clk 36000000                 //system clock
#define NUM_OF_HALLSENSOR_PULS 30    //pulses per rotation
#define MAX_PWM 2000                 //max motor PWM high
#define MOTOR_PWM_LOW 800            //motor PWM LOW
#define MAX_SPEED 2300              //MAX_SPEED

//*******************************************************
void state();                        //action of the next state
void wate();                         //make the 3 phases floating so it make the low side of pwm
void NextStep_NORMAL();				 //what is the next step
void NextStep_REVERES();			 //what is the next step reveres the motor direction
void MOTOR_STOP();                   //stop the motor
void BLDC_init();
void read_string();
//*******************************************************

uint8_t hall1state=1;                 //hall sensor
uint8_t hall2state=0;
uint8_t hall3state=1;

uint8_t MOTOR_state =0;              //motor state
uint8_t MOTOR_DIRECTION;            //motor direction

uint32_t SET_MOTOR_SPEED;
int encoder_pulses;
int MOTOR_SPEED_RPM,last_read_motor_speed;
int error,MOTOR_PWM;

uint32_t TIME_CALC;

uint8_t buf[7];
uint8_t stop_flage,flag_SPEED;
uint8_t HALL2READ;

unsigned char ch;
char string_data[10];
uint8_t serial_flag=0,index1;
//*******************************************************
//===================== -INTERRUPT- =====================
//*******************************************************
void s1(){
	hall1state=READ_PIN(GPIOA, pin0);

	//calculate the distance
	HALL2READ = READ_PIN(GPIOA, pin1);
	if(hall1state==1){
		if(HALL2READ > 0){
			encoder_pulses++;
		}
		else{
			encoder_pulses--;
		}
	}

	//motor direction
	if(MOTOR_DIRECTION==0){
		NextStep_NORMAL();
	}
	else {
		NextStep_REVERES();
	}

}
void s2(){
	//calculate the speed
	if(flag_SPEED==0){
		TIME_CALCULATION(clk, TIMER_START);
		flag_SPEED=1;
	}
	else{
		TIME_CALC=TIME_CALCULATION(clk, TIMER_STOP);
		flag_SPEED=0;
	}

	//hall read
	hall2state=READ_PIN(GPIOA, pin1);
	//motor direction
	if(MOTOR_DIRECTION==0){
		NextStep_NORMAL();
	}
	else {
		NextStep_REVERES();
	}
}
void s3(){
	//hall read
	hall3state=READ_PIN(GPIOA, pin2);
	//motor direction
	if(MOTOR_DIRECTION==0){
		NextStep_NORMAL();
	}
	else {
		NextStep_REVERES();
	}
}
//*******************************************************
//=============== -NextStep action- =====================
//*******************************************************
void NextStep_NORMAL() {
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
	//second problem % interrupt debouncing %
	//when the interrupt Happens the state change if we first put a rest(LOW PWM),
	//the motor will lose the torque and it return and it make another interrupt
	//and it happens again and again so we start with high side for 50 us to shore the motor pass the sensor
	TIMER_ISR(TIM3,50, U_us, clk, state);
	// 3 phase as a float to solve the pulse stop of motor when it run at high speed
	// when changing the step without rest the motor stuck at pulse time , make it very violent
	//A floating
	pinwrite(GPIOB, pin0,LOW);
	pinwrite(GPIOB, pin1,HIGH);
	//B floating
	pinwrite(GPIOB, pin10,LOW);
	pinwrite(GPIOB, pin7,HIGH);
	//C floating
	pinwrite(GPIOB, pin8,LOW);
	pinwrite(GPIOB, pin5,HIGH);
}
void NextStep_REVERES() {
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

	TIMER_ISR(TIM3,50, U_us, clk, state);
	// 3 phase as a float to solve the pulse stop of motor when it run at high speed
	// when changing the step without rest the motor stuck at pulse time , make it very violent
	//A floating
	pinwrite(GPIOB, pin0,LOW);
	pinwrite(GPIOB, pin1,HIGH);
	//B floating
	pinwrite(GPIOB, pin10,LOW);
	pinwrite(GPIOB, pin7,HIGH);
	//C floating
	pinwrite(GPIOB, pin8,LOW);
	pinwrite(GPIOB, pin5,HIGH);
}
//*******************************************************
//==================== -LOW PWM- ========================
//*******************************************************
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
	TIMER_ISR(TIM3,MOTOR_PWM_LOW, U_us, clk, state);
}
//*******************************************************
//==================== -HIGH PWM- =======================
//*******************************************************
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
	TIMER_ISR(TIM3,MOTOR_PWM, U_us, clk, wate);

}
//*******************************************************
//==================== -MOTOR_STOP- =====================
//*******************************************************
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
//*******************************************************
//==================== -BLDC_init- ======================
//*******************************************************
void BLDC_init(){
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
	USART_INIT(USART1, mode_RX_TX_ENABLE, word_length_8, stop_bits_1, baud_rate_9600, parity_none,clk);

	s1();
	s2();
	s3();
	//soft start
	for(int i=0;i<10;i++){
		MOTOR_PWM+=1;
		delay(2, U_ms, clk);
	}
	USART_READ_INTERRUPT_EN(USART1, read_string);
}
//*******************************************************
//==================== -READ_DATA- ======================
//*******************************************************
void read_string (){

	USART_READ(USART1,&ch);

	if(ch=='$'){
		serial_flag=1;
		index1=0;
	}
	if(ch=='e'){
		serial_flag=0;
		string_data[index1]='\n';
		string_data[index1+1]=0;

	}

	if(serial_flag==1){
		string_data[index1]=ch;
		index1++;
	}
}
//*******************************************************
//==================== -MAIN Fun- =======================
//*******************************************************
int main (){
	//******************************
	//======== -BLDC_init- ========
	//******************************
	BLDC_init();

	while(1){
		delay(270, U_ms, clk);
		//******************************
		//======== -direction- ========
		//******************************
		if(string_data[1]=='1'){
			MOTOR_DIRECTION=1;
		}
		else {
			MOTOR_DIRECTION=0;
		}
		//******************************
		//======== -speed- ============
		//******************************
		//read speed value
		//$.direction.speed.s
		SET_MOTOR_SPEED=atoi(string_data+2)*10;//reat user set point speed

		MOTOR_SPEED_RPM=600000000/(TIME_CALC*NUM_OF_HALLSENSOR_PULS);//read the actual speed
		TIME_CALC=0;

		//(filter) if actual speed is hire than 2300/10 RPM make speed 2300
		//to remove hall effect noise
		if( (abs(MOTOR_SPEED_RPM-last_read_motor_speed))>MAX_SPEED ){
			MOTOR_SPEED_RPM=last_read_motor_speed;
		}
		else {
			last_read_motor_speed=MOTOR_SPEED_RPM;
		}

		//******************************
		//============ -PID- ===========
		//******************************
		error=abs(MOTOR_SPEED_RPM-SET_MOTOR_SPEED);

		if(MOTOR_SPEED_RPM<SET_MOTOR_SPEED){
			MOTOR_PWM+=(error/8.0);
			if(MOTOR_PWM>=MAX_PWM){
				MOTOR_PWM=MAX_PWM;
			}
		}
		else if(MOTOR_SPEED_RPM>SET_MOTOR_SPEED){
			MOTOR_PWM-=(error/8.0);
			if(MOTOR_PWM<=5){
				MOTOR_PWM=5;
			}
		}
		else{//error=0
			if(SET_MOTOR_SPEED==0){
				MOTOR_PWM=2;
			}
			else{
				MOTOR_PWM=MOTOR_PWM;
			}

		}


		//display
		sprintf (buf, "%d",MOTOR_SPEED_RPM/10);
		USART_SEND_STRING(USART1,buf);
		delay(3, U_ms, clk);
		USART_SEND_STRING(USART1," RPM \n");

	}

}



#include "STM32_TIMERS_DRIVER.h"
#include "stm32_speed_DRIVER.h"
#include "stm32f103c6_ISR_DRIVER.h"
#include "stm32f103c6_ADC.h"
#include "stm32f103c6_USART_SRIVER.h"
#include "math.h"
#include <string.h>
//=======================================================
#define clk 36000000                 //system clock
#define joystickRight_X		 ADC_pin_PA1
#define joystickRight_Y 	 ADC_pin_PA2
#define joystickLEFT_X   	 ADC_pin_PA0
#define joystickLEFT_Y   	 ADC_pin_PA3


#define Potentiometer_RIGHT  ADC_pin_PA4
#define Potentiometer_LEFT   ADC_pin_PA5
#define SelectSwitchLEFTUP	   pin13 //port B
#define SelectSwitchLEFTdown   pin12
#define SelectSwitchRIGHTUP    pin15
#define SelectSwitchRIGHTdown  pin14

#define joystickLEFT_switch    pin11 //port b
#define joystickRIGHT_switch   pin15 //port c

#define redSwitch1             pin10 //port b
#define redSwitch2			   pin1 //port b

#define greenSwitch1           pin0 //port b
#define greenSwitch2           pin7 //port A

uint8_t KEYS[10];
uint16_t joystick[4];
uint16_t Potentiometer[2];
uint16_t offset=0;
uint16_t Systemcounter=0;

char result[100];

int main(void)
{
	MCAL_speed_init(CLK_SORC_IN_CLK8MHz, mood_FAST_MOOD_36MHzCORE);
	pinmode(GPIOB, SelectSwitchLEFTUP,GPIO_MODE_INPUT_PU);
	pinmode(GPIOB, SelectSwitchLEFTdown,GPIO_MODE_INPUT_PU);
	pinmode(GPIOB, SelectSwitchRIGHTUP,GPIO_MODE_INPUT_PU);
	pinmode(GPIOB, SelectSwitchRIGHTdown,GPIO_MODE_INPUT_PU);

	pinmode(GPIOB, joystickLEFT_switch,GPIO_MODE_INPUT_PU);
	pinmode(GPIOC, joystickRIGHT_switch,GPIO_MODE_INPUT_PU);

	pinmode(GPIOB, redSwitch1,GPIO_MODE_INPUT_PU);
	pinmode(GPIOB, redSwitch2,GPIO_MODE_INPUT_PU);
	pinmode(GPIOB, greenSwitch1,GPIO_MODE_INPUT_PU);
	pinmode(GPIOA, greenSwitch2,GPIO_MODE_INPUT_PU);

	pinmode(GPIOC, pin13, GPIO_MODE_OUTPUT_push_pull_Speed2);

	ADC_INIT(ADC1, ADC_STATE_START);

	USART_INIT(USART1, mode_RX_TX_ENABLE, word_length_8, stop_bits_1, baud_rate_9600, parity_none,clk);
	while(1){
		//READ
		joystick[0]=ADC_READ(ADC1, joystickRight_X)/2;
		joystick[1]=ADC_READ(ADC1, joystickRight_Y)/2;
		joystick[2]=ADC_READ(ADC1, joystickLEFT_X)/2;
		joystick[3]=ADC_READ(ADC1, joystickLEFT_Y)/2;
		Potentiometer[0]=ADC_READ(ADC1, Potentiometer_RIGHT)/4;
		Potentiometer[1]=ADC_READ(ADC1, Potentiometer_LEFT)/4;

		KEYS[0]=READ_PIN(GPIOB, SelectSwitchLEFTUP);
		KEYS[1]=READ_PIN(GPIOB, SelectSwitchLEFTdown);
		KEYS[2]=READ_PIN(GPIOB, SelectSwitchRIGHTUP);
		KEYS[3]=READ_PIN(GPIOB, SelectSwitchRIGHTdown);

		KEYS[4]=READ_PIN(GPIOB, joystickLEFT_switch);
		KEYS[5]=READ_PIN(GPIOC, joystickRIGHT_switch);
		KEYS[6]=READ_PIN(GPIOB, redSwitch1);
		KEYS[7]=READ_PIN(GPIOB, redSwitch2);
		KEYS[8]=READ_PIN(GPIOB, greenSwitch1);
		KEYS[9]=READ_PIN(GPIOA, greenSwitch2);

		//CONVERT TO STRING
		for (int i = 0; i < 4; i++) {
			offset += sprintf(result + offset, "%u,", joystick[i]);
		}
		for (int i = 0; i < 2; i++) {
			offset += sprintf(result + offset, "%u,", Potentiometer[i]);
		}
		for (int i = 0; i < 10; i++) {
			offset += sprintf(result + offset, "%u,", KEYS[i]);
		}
		offset=0;
		if(Systemcounter<20)Systemcounter++;
		else Systemcounter=0;
		pinwrite(GPIOC, pin13,HIGH);
		USART_SEND_STRING_UNinterrupted(USART1,result);
		USART_SEND_STRING_UNinterrupted(USART1,"m\n");

		delay(6, U_ms, clk);

		if(Systemcounter>10)pinwrite(GPIOC, pin13,LOW);
	}
}

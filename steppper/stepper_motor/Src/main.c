#include "STM32_TIMERS_DRIVER.h"
#include "stm32_speed_DRIVER.h"
#include "GPIO_DRIVER.h"
int main(void)
{
	MCAL_speed_init(CLK_SORC_IN_CLK8MHz, mood_FAST_MOOD_36MHzCORE);//speed of all 36MHz
	//	PWM(TIM2, CH_1,50,5000,36000000);
	//	PWM(TIM2, CH_2,40,5000,36000000);
	//	PWM(TIM2, CH_3,20,5000,36000000);
	//	PWM(TIM2, CH_4,10,5000,36000000);

#define del 200
#define frq 2200

	while(1){
		stepper_motor_steps(CH_3,GPIOB,pin10 ,400, 50, frq,direction_clk, 36000000);
		delay(del, U_ms, 36000000);
		stepper_motor_steps(CH_3,GPIOB,pin10 ,400, 50, frq,direction_unticlk, 36000000);
		delay(del, U_ms, 36000000);
		stepper_motor_steps(CH_3,GPIOB,pin10 ,200, 50, frq,direction_clk, 36000000);
		delay(del, U_ms, 36000000);
		stepper_motor_steps(CH_3,GPIOB,pin10 ,200, 50, frq,direction_unticlk, 36000000);
		delay(del, U_ms, 36000000);
		stepper_motor_steps(CH_3,GPIOB,pin10 ,100, 50, frq,direction_clk, 36000000);
		delay(del, U_ms, 36000000);
		stepper_motor_steps(CH_3,GPIOB,pin10 ,100, 50, frq,direction_unticlk, 36000000);
		delay(del, U_ms, 36000000);
		stepper_motor_steps(CH_3,GPIOB,pin10 ,50, 50, frq,direction_clk, 36000000);
		delay(del, U_ms, 36000000);
		stepper_motor_steps(CH_3,GPIOB,pin10 ,50, 50, frq,direction_unticlk, 36000000);
		delay(del, U_ms, 36000000);

	}
}

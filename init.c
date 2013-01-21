#include "init.h"

#include "includes.h"
#include "globals.h"
#include "usart.h"
#include "mypwm.h"
#include "motor.h"

char rxBuffer[RX_BUFFER_SIZE];
char txBuffer[TX_BUFFER_SIZE];

void InterruptsEnable(void)
{
	IntMasterEnable();
}

void _ClockInitialise(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
}

void _MainOledInitialise(void)
{
	RIT128x96x4Init(1000000);
}

void _UsartInitialise(void)
{
	buffer rxBufferDescriptor = {RX_BUFFER_SIZE, rxBuffer};
	buffer txBufferDescriptor = {TX_BUFFER_SIZE, txBuffer};

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UsartInitialise(&usart0, UART0_BASE, USART0_BAUDRATE, &rxBufferDescriptor, &txBufferDescriptor);
}

void _PwmInitialise(void)
{
	int i;
	
	SysCtlPWMClockSet(SYSCTL_PWMDIV_16);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_2);
  GPIOPinTypePWM(GPIO_PORTH_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
	PwmGeneratorInitialise(&pwm_gen0, PWM0_BASE, PWM_GEN_0, PWM_PERIOD);
	PwmGeneratorInitialise(&pwm_gen1, PWM0_BASE, PWM_GEN_1, PWM_PERIOD);
	PwmInitialise(&pwm0, &pwm_gen0, PWM_OUT_0, 0);
	PwmInitialise(&pwm1, &pwm_gen0, PWM_OUT_1, 0);
	PwmInitialise(&pwm2, &pwm_gen1, PWM_OUT_2, 0);
	PwmInitialise(&pwm3, &pwm_gen1, PWM_OUT_3, 0);
	
	all_pwm[0] = &pwm0; all_pwm[1] = &pwm1; all_pwm[2] = &pwm2; all_pwm[3] = &pwm3;
	all_pwm_gens[0] = &pwm_gen0; all_pwm_gens[1] = &pwm_gen1;
	
	PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT | PWM_OUT_2_BIT | PWM_OUT_3_BIT, true);
	
	for(i = 0; i < 2; i++)
		PwmGeneratorEnable(all_pwm_gens[i]);
}

void _MotorsInitialise(void)
{
	MotorInitialise(&motorA, &pwm0);
	MotorInitialise(&motorB, &pwm1);
	MotorInitialise(&motorC, &pwm2);
	MotorInitialise(&motorD, &pwm3);
}

void InitialiseSystem(void)
{
	_ClockInitialise();
	_MainOledInitialise();
	_UsartInitialise();
	InterruptsEnable();
	UsartWriteLine(&usart0, "USART Initialised");
	_PwmInitialise();
	UsartWriteLine(&usart0, "PWM Initialised");
	_MotorsInitialise();
	UsartWriteLine(&usart0, "Motors Initialised");
}

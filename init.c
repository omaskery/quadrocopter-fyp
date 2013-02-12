#include "init.h"

#include "includes.h"
#include "globals.h"
#include "usart.h"
#include "mypwm.h"
#include "motor.h"
#include "iic.h"
#include "sensor.h"
#include "motion.h"
#include "flight.h"
#include "system.h"

char rxUsartBuffer[RX_BUFFER_SIZE];
char txUsartBuffer[TX_BUFFER_SIZE];

char rxIicBuffer[I2C_BUFFER_SIZE];
char txIicBuffer[I2C_BUFFER_SIZE];

void InterruptsEnable(void)
{
	IntMasterEnable();
}

void _ClockInitialise(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
}

void _FpuInitialise(void)
{
	FPUEnable();
}

void _MainOledInitialise(void)
{
	RIT128x96x4Init(1000000);
}

void _UsartInitialise(void)
{
	buffer rxBufferDescriptor = {RX_BUFFER_SIZE, rxUsartBuffer};
	buffer txBufferDescriptor = {TX_BUFFER_SIZE, txUsartBuffer};

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
	
	all_motors[0] = &motorA;
	all_motors[1] = &motorB;
	all_motors[2] = &motorC;
	all_motors[3] = &motorD;
}

void _IicInitialise(void)
{
	buffer rxBufferDescriptor = {I2C_BUFFER_SIZE, rxIicBuffer};
	buffer txBufferDescriptor = {I2C_BUFFER_SIZE, txIicBuffer};
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);

	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD);
  GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD);

	GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_DIR_MODE_HW);
  GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_DIR_MODE_HW);

	IicInitialise(&i2c0, I2C0_MASTER_BASE, &rxBufferDescriptor, &txBufferDescriptor);

	IntEnable(INT_I2C0);
  I2CMasterIntEnableEx(I2C0_MASTER_BASE,I2C_MASTER_INT_DATA);
}

void _SensorInitialise()
{
	SensorInitialise(&mpu6050, &i2c0, MPU6050_ADDR);
}

void _SystemStateInitialise(void)
{
	SystemInitialise(&sys);
}

void _MotionInitialise(void)
{
	MotionInitialise(&orientation, &mpu6050);
}

void _FlightInitialise(void)
{
	FlightInitialise(&flight_module);
}

void _TimerInitialise(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	IntEnable(INT_TIMER0A);
	
	TimerInitialise(&sys_timer, TIMER0_BASE, SysCtlClockGet() / 400);
	TimerStart(&sys_timer);
}

void InitialiseSystem(void)
{
	_ClockInitialise();
	_FpuInitialise();
	_MainOledInitialise();
	_UsartInitialise();
	InterruptsEnable();
	UsartWriteLine(&usart0, "USART Initialised");
	_PwmInitialise();
	UsartWriteLine(&usart0, "PWM Initialised");
	_MotorsInitialise();
	UsartWriteLine(&usart0, "Motors Initialised");
	_IicInitialise();
	UsartWriteLine(&usart0, "I2C Initialised");
	_SensorInitialise();
	UsartWriteLine(&usart0, "Sensor Initialised");
	_SystemStateInitialise();
	UsartWriteLine(&usart0, "System State Initialised");
	_MotionInitialise();
	UsartWriteLine(&usart0, "Motion Module Initialised");
	_FlightInitialise();
	UsartWriteLine(&usart0, "Flight Module Initialised");
	_TimerInitialise();
	UsartWriteLine(&usart0, "Timers initialised");
	
	UsartWriteLine(&usart0, "Initialisation Complete");
}

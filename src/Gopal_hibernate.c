/*****************************************************************************
 * Gopal_hibernate.c
 *
 * Author: Gopal Vishwakarma
 * Email: gopalvishwakarma19@yahoo.com
 *
 * Details: Power consumption in hibernate mode (EV-COG-AD3029LZ: ADuCM3029 MCU)
 *****************************************************************************/

#include <sys/platform.h>
#include "adi_initialize.h"
#include "Gopal_hibernate.h"
#include <stdio.h>
#include <drivers/pwr/adi_pwr.h>
#include <drivers/gpio/adi_gpio.h>
#include "common.h"

/* Number of wakeup interrupt prior to fully exiting hibernation */
#define MAX_WAKEUPS 3

#define PB1_PORT_NUM        ADI_GPIO_PORT1
#define PB1_PIN_NUM         ADI_GPIO_PIN_0

volatile bool bHibernateExitFlag;
static void WakeupCallback (void *pCBParam, uint32_t Event, void *pArg);

int main(int argc, char *argv[])
{
	/**
	 * Initialize managed drivers and/or services that have been added to 
	 * the project.
	 * @return zero on success 
	 */
	adi_initComponents();
	
	/* Begin adding your custom code here */
	static uint8_t gpioMemory[ADI_GPIO_MEMORY_SIZE];
	ADI_GPIO_RESULT eResult;
	SystemInit();

	/* init the GPIO service */
	adi_gpio_Init(gpioMemory, ADI_GPIO_MEMORY_SIZE);

	/* call back for WAKE0 (P0.15: pushbutton) and pin interrupt setting */
	    adi_gpio_InputEnable(ADI_GPIO_PORT0, ADI_GPIO_PIN_15, true);
	/* set GPIO input */
		if(ADI_GPIO_SUCCESS != (eResult = adi_gpio_InputEnable(PB1_PORT_NUM, PB1_PIN_NUM, true)))
		{
			DEBUG_MESSAGE("adi_gpio_InputEnable failed\n");
			//break;
		}
	  //  adi_gpio_RegisterCallback(XINT_EVT0_IRQn, WakeupCallback, NULL);
	    //adi_gpio_EnableExIRQ(XINT_EVT0_IRQn, ADI_GPIO_IRQ_FALLING_EDGE);
	/* set Pin polarity as rising edge */
		if(ADI_GPIO_SUCCESS != (eResult = adi_gpio_SetGroupInterruptPolarity(PB1_PORT_NUM, PB1_PIN_NUM)))
		{
			DEBUG_MESSAGE("adi_gpio_SetGroupInterruptPolarity failed\n");
			//break;
		}
	/* Enable pin interrupt on group interrupt A */
		if(ADI_GPIO_SUCCESS != (eResult = adi_gpio_SetGroupInterruptPins(PB1_PORT_NUM, ADI_GPIO_INTA_IRQ, PB1_PIN_NUM)))
		{
			DEBUG_MESSAGE("adi_gpio_SetGroupInterruptPins failed\n");
			//break;
		}
	  /* Register the callback */
		if(ADI_GPIO_SUCCESS != (eResult = adi_gpio_RegisterCallback (ADI_GPIO_INTA_IRQ, WakeupCallback, (void*)ADI_GPIO_INTA_IRQ)))
		{
			DEBUG_MESSAGE("adi_gpio_RegisterCallback failed\n");
			//break;
		}




		/* reset volatiles */
		    bHibernateExitFlag = false;
		//bHibernateExitFlag = NULL; //sleep on exit
	/* enter full hibernate with wakeup flag with no interrupt masking */
		/* (will not return until exit flag is set in the interrupt handler) */
		adi_pwr_EnterLowPowerMode(ADI_PWR_MODE_HIBERNATE, &bHibernateExitFlag, 0);

		while(1){
			for (volatile uint32_t i = 0; i < 1000000; i++);
			for (volatile uint32_t i = 0; i < 1000000; i++);
			for (volatile uint32_t i = 0; i < 1000000; i++);
			adi_pwr_EnterLowPowerMode(ADI_PWR_MODE_HIBERNATE, &bHibernateExitFlag, 0);
		}

	return 0;
}


/* Wakeup event callback from GPIO service */
static void WakeupCallback (void *pCBParam, uint32_t Event, void *pArg)
{
   adi_pwr_ExitLowPowerMode(&bHibernateExitFlag);

    /*  if adi_pwr_ExitLowPowerMode() is not called, we shoud re-enter
        hibernation without exiting the initial (and still pending) call
        to adi_pwr_EnterLowPowerMode()
    */
}

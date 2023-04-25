#include <stdbool.h>
#include <stdint.h>
// TivaC specific includes
extern "C"
{
  #include <driverlib/sysctl.h>
  #include <driverlib/gpio.h>
  // Bibliotecas que estan dentro del compedio de Tivaware

  // Estas librerias se encuentran dentro de la carpeta inc del tivaware
  #include "inc/hw_memmap.h" //Es para el mapa de memoria
  #include "inc/hw_types.h"
  #include "inc/tm4c1294ncpdt.h" // Es para la propia placa

  // Estas librerias se encuentran dentro de la carpeta driverlib del Tivaware
  #include <driverlib/sysctl.h> // Importante es el system control que controla todo el sistema
  #include <driverlib/gpio.h> // Para entrada y salida de los pines
  #include "driverlib/pwm.h" // Libreria que es parte para el uso del PWM
}
// ROS includes
#include <ros.h>
#include <std_msgs/Int16MultiArray.h>
#include <math.h>
#include <std_msgs/String.h>

// Definimos variables de ROS 
ros::NodeHandle nh;
std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);
char hello[13] = "Hello world!";

// Declaramos las variables globales para el funcionamiento del PWM 
#define PWM_FREQUENCY 55

//#define GPIO_PF1_M0PWM1 0x00050406
//#define GPIO_PF2_M0PWM2 0x00050806

#define GPIO_PG0_M0PWM4         0x00060006 // Comment: pin_map.h is a very large file

// Variables que son para controlar el PWM

//volatile uint32_t RelojPWM; // ~= volatile uint32_t ui32PWMClock -> Variables que guarda PWMClock
//uint32_t TIVAfreq; // Lo guarda la frecuencia de la TIVA 
//volatile uint32_t Carga;
//volatile uint16_t Ajustes1; // Analizar su comportamiento 

volatile uint32_t ui32PWMClock;//variable that stores PWMclock
volatile uint32_t ui32SysClkFreq;//variable that returns MCU frequency
volatile uint32_t ui32Load;
volatile uint32_t ui32Index;
volatile uint32_t ui32Level;

// Varables para hacer la prueba de funcionamiento del PWM 
float angulo = 0;
float cargar;
float aumento = 1;

int main(void)
{
  // TivaC application specific code
  MAP_FPUEnable();
  MAP_FPULazyStackingEnable();

  // Run from the PLL at 120 MHz.
  // 1 Paso establecer el reloj de nuestro microcontrolador
  ui32SysClkFreq = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                          SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), TM4C129FREQ);
  
  //Ajustes1 = 30;

  // 2 Paso Habilitamos los perifericos que vamos a utilizar
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

  // 3 Iniciamos la configuracion del PWM
  PWMClockSet(PWM0_BASE,PWM_SYSCLK_DIV_64);
  
  GPIOPinConfigure(GPIO_PG0_M0PWM4);
  //GPIOPinConfigure(GPIO_PF1_M0PWM1);
  //GPIOPinConfigure(GPIO_PF2_M0PWM2);
  
  GPIOPinTypePWM(GPIO_PORTG_BASE,GPIO_PIN_0);
  //GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2);

  ui32PWMClock=ui32SysClkFreq/64;
  ui32Load=(ui32PWMClock/PWM_FREQUENCY)-1;
  
  PWMGenConfigure(PWM0_BASE,PWM_GEN_2,PWM_GEN_MODE_DOWN);
  //PWMGenConfigure(PWM0_BASE,PWM_GEN_0,PWM_GEN_MODE_DOWN); // Configura el generador
  //PWMGenConfigure(PWM0_BASE,PWM_GEN_1,PWM_GEN_MODE_DOWN);
  
  PWMGenPeriodSet(PWM0_BASE,PWM_GEN_2,ui32Load);
  //PWMGenPeriodSet(PWM0_BASE,PWM_GEN_0,ui32Load);
  //PWMGenPeriodSet(PWM0_BASE,PWM_GEN_1,ui32Load);

  //PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,ui32Load/2);
  //PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,ui32Load/2);
  //PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,ui32Load/2);
  
  PWMOutputState(PWM0_BASE,PWM_OUT_4_BIT,true);
  //PWMOutputState(PWM0_BASE,PWM_OUT_1_BIT,true);
  //PWMOutputState(PWM0_BASE,PWM_OUT_2_BIT,true);
  
  PWMGenEnable(PWM0_BASE,PWM_GEN_2);
  //PWMGenEnable(PWM0_BASE,PWM_GEN_0);
  //PWMGenEnable(PWM0_BASE,PWM_GEN_1);


  // ROS nodehandle initialization and topic registration
  nh.initNode();
  nh.advertise(chatter);

  while (1)
  {
    //Inicio logica de movimiento del robot

    cargar = ((((angulo/180)*9.26)+2.74)/100)*37499;

    //PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,cargar);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,37499*0.0274);

    SysCtlDelay(1800000);

    if (aumento>0){
        angulo=angulo+1;
        if (angulo>180){
            aumento = -1;
        }
    }else{
        angulo=angulo-1;
        if (angulo<0){
            aumento=1;
        }
    }

    //PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,ui32Load/2);
    //PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,ui32Load/2);

    // Publish message to be transmitted.
    str_msg.data = hello;
    chatter.publish(&str_msg);
    // Handle all communications and callbacks.
    nh.spinOnce();
    // Delay for a bit.
    nh.getHardware()->delay(100);
  }
}

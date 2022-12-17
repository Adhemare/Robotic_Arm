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
}
// ROS includes
#include <ros.h>
#include <std_msgs/String.h>

// ROS nodehandle
ros::NodeHandle nh; // Nos permite interactuar con el nodo de ROS 

// Declaramos variables globales para el funcionamiento TIVA
bool prender_apagar = true;

void message_cb (const std_msgs::Empty& toggle_msg)
{
  if (prender_apagar){
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x01);
  }else{
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);
  }
  prender_apagar = !prender_apagar;
}

// Creamos nuestro objeto subscriber  
// Carac. del subscriber: Empty: se estan publicando empty messages en el topico led
// Cada vez que se recibe un mensaje se llama a la funcion message_cb 
ros::Subscriber<std_msgs::Empty> sub("led", &message_cb);

int main(void)
{
  // TivaC application specific code
  MAP_FPUEnable();
  MAP_FPULazyStackingEnable();
  
  // Run from the PLL at 120 MHz.
  // 1 Paso establecer el reloj de nuestro microcontrolador
  MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                          SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), TM4C129FREQ);
                          
  // 2 Paso Habilitamos los perifericos que vamos a utilizar
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
  
  // 3 Paso Configuramos los perifericos 
  GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); 
  
  // ROS nodehandle initialization and topic registration
  // Inicializamos nuestro ros node 
  nh.initNode();
  // Nos subscribimos
  nh.subscribe(sub);

  while (1)
  {

    // Handle all communications and callbacks.
    nh.spinOnce(); // Hacemos que este continuamente publicandose mensajes 

    // Delay for a bit.
    nh.getHardware()->delay(1);
  }
}


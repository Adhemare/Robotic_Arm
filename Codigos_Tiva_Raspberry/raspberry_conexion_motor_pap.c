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
#include <std_msgs/Int16.h>

// ROS nodehandle
ros::NodeHandle nh; // Nos permite interactuar con el nodo de ROS
std_msgs::Int16 message;
ros::Publisher pub("retroalimentacion", &message);

// Declaramos variables globales para el funcionamiento TIVA
bool prender_apagar = true;
int retralimentacion_m = 0;
int pasos_motor = 0;
int velocidad_motor = 120000000/100;
int32_t contador_prender_apagar = 0;
int32_t contador_pasos = 0;

void mover_motor_antihorario(int pasos_motor)
{
  while (contador_pasos < pasos_motor){
      if (prender_apagar){
          GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);
          SysCtlDelay(velocidad_motor);
      }else{
          GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x02);
          SysCtlDelay(velocidad_motor);
      }
      prender_apagar = !prender_apagar;
      contador_prender_apagar = contador_prender_apagar + 1;
      if (contador_prender_apagar == 2){
          contador_pasos = contador_pasos + 1;
          contador_prender_apagar = 0;
      }
  } // Fin de while
  contador_pasos=0;
  contador_prender_apagar=0;
  GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);

}// Fin de la clase mover_motor_antihorario

void mover_motor_horario(int pasos_motor)
{
  while (contador_pasos < pasos_motor){
    if (prender_apagar){
        GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x01);
        SysCtlDelay(velocidad_motor);
    }else{
        GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x03);
        SysCtlDelay(velocidad_motor);
    }
    prender_apagar = !prender_apagar;
    contador_prender_apagar = contador_prender_apagar + 1;
    if (contador_prender_apagar == 2){
        contador_pasos = contador_pasos + 1;
        contador_prender_apagar = 0;
    }
  }// Fin de while
  contador_pasos=0;
  contador_prender_apagar=0;
  GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);

}// Fin de la clase mover_motor_horario

void arm_actuate_cb (const std_msgs::Int16& msg_degrees_motor)
{
  if (retralimentacion_m == 0){
    retralimentacion_m = static_cast<int>(msg_degrees_motor.data)-90;
    // Realizamos las conversiones de degrees a pap
    pasos_motor = (static_cast<int>(msg_degrees_motor.data) - 90)*(800/360);
    if (pasos_motor>0){
      mover_motor_antihorario(pasos_motor);
    }else{
      pasos_motor = pasos_motor * (-1);
      mover_motor_horario(pasos_motor);
    }
  }else{
    pasos_motor = ((static_cast<int>(msg_degrees_motor.data) - 90)-retralimentacion_m)*(800/360);
    if (pasos_motor>0){
      if ((static_cast<int>(msg_degrees_motor.data)-90) > retralimentacion_m){
        mover_motor_antihorario(pasos_motor);
      }else{
        mover_motor_horario(pasos_motor);
      }
    }else{
      pasos_motor = pasos_motor *(-1);
      if ((static_cast<int>(msg_degrees_motor.data)-90) > retralimentacion_m){
        mover_motor_antihorario(pasos_motor);
      }else{
        mover_motor_horario(pasos_motor);
      }
    }
    retralimentacion_m = static_cast<int>(msg_degrees_motor.data) - 90;

  }
}
// Creamos nuestro objeto subscriber
// Carac. del subscriber: Empty: se estan publicando empty messages en el topico led
// Cada vez que se recibe un mensaje se llama a la funcion message_cb
ros::Subscriber<std_msgs::Int16> sub("tiva/arm_actuate", &arm_actuate_cb);

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
  nh.advertise(pub);

  while (1)
  {
    // Publicamos la retroalimentacion
    message.data = static_cast<int16_t>(retralimentacion_m);
    pub.publish(&message);
    // Handle all communications and callbacks.
    nh.spinOnce(); // Hacemos que este continuamente publicandose mensajes

    // Delay for a bit.
    nh.getHardware()->delay(100);
  }
}

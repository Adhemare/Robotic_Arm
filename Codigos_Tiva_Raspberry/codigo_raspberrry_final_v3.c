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
#include <std_msgs/Int16MultiArray.h>
#include <math.h>

// Inicialicamos el nodo de ROS
ros::NodeHandle nh; // Nos permite interactuar con el nodo de ROS

// Declaramos variables para nuestro publisher de valores de retroalimentacion
std_msgs::Int16MultiArray msg_retroalimentacion;
ros::Publisher pub("/brazo_robotico/retroalimentacion", &msg_retroalimentacion); //Publicara los mensaje$

// Declaramos variables globales para el funcionamiento del motor
bool prender_apagar = true;
int pasos_motor = 0; // Cuantos pasos se va a mover
int velocidad_motor = 120000000/100; // Establecemos la velocidad de los motores pap
int32_t contador_prender_apagar = 0;
int32_t contador_pasos = 0; // Inicializamos contador de pasos

void mover_motor_antihorario(int pasos_motor, int numero_motor)
{
  while (contador_pasos < pasos_motor){
      if (prender_apagar){
          switch(numero_motor){
            case 0:
              // MOVIMIENTO DE LA BASE
              GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_0|GPIO_PIN_1,0x00);
              SysCtlDelay(velocidad_motor);
              break;
            case 1:
              // MOVIMIENTO DEL ESLABON 1 
              GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x02);
              SysCtlDelay(velocidad_motor);
              break;
            case 2:
              // MOVIMIENTO DEL ESLABON 2
              GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_0|GPIO_PIN_1,0x00);
              SysCtlDelay(velocidad_motor);
              break;
          }// Fin del switch
      }else{
          switch(numero_motor){
            case 0:
              // MOVIMIENTO DE LA BASE
              GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_0|GPIO_PIN_1,0x02);
              SysCtlDelay(velocidad_motor);
              break;
            case 1:
              // MOVIMIENTO DEL ESLABON 1 
              GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x06);
              SysCtlDelay(velocidad_motor);
              break;
            case 2:
              // MOVIMIENTO DEL ESLABON 2
              GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_0|GPIO_PIN_1,0x02);
              SysCtlDelay(velocidad_motor);
              break;
          }// Fin del switch
      }
      prender_apagar = !prender_apagar;
      contador_prender_apagar = contador_prender_apagar + 1;
      if (contador_prender_apagar == 2){
          contador_pasos = contador_pasos + 1;
          contador_prender_apagar = 0;
      }
  } // Fin de while

  // Reiniciamos los valores
  contador_pasos=0;
  contador_prender_apagar=0;
  // Ponemos los motores en descanso
  GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_0|GPIO_PIN_1,0x00);
  GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);
  GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_0|GPIO_PIN_1,0x00);


}// Fin de la clase mover_motor_antihorario

void mover_motor_horario(int pasos_motor, int numero_motor)
{
  while (contador_pasos < pasos_motor){
    if (prender_apagar){
        switch(numero_motor){
          case 0:
            // MOVIMIENTO DE LA BASE
            GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_0|GPIO_PIN_1,0x01);
            SysCtlDelay(velocidad_motor);
            break;
          case 1:
            // MOVIMIENTO DEL ESLABON 1 
            GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x01);
            SysCtlDelay(velocidad_motor);
            break;
          case 2:
            // MOVIMIENTO DEL ESLABON 2
            GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_0|GPIO_PIN_1,0x01);
            SysCtlDelay(velocidad_motor);
            break;
        }// Fin del switch
    }else{
        switch(numero_motor){
          case 0:
            // MOVIMIENTO DE LA BASE
            GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_0|GPIO_PIN_1,0x03);
            SysCtlDelay(velocidad_motor);
            break;
          case 1:
            // MOVIMIENTO DEL ESLABON 1 
            GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x05);
            SysCtlDelay(velocidad_motor);
            break;
          case 2:
            // MOVIMIENTO DEL ESLABON 2
            GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_0|GPIO_PIN_1,0x03);
            SysCtlDelay(velocidad_motor);
            break;
        }// Fin del switch
    }
    prender_apagar = !prender_apagar;
    contador_prender_apagar = contador_prender_apagar + 1;
    if (contador_prender_apagar == 2){
        contador_pasos = contador_pasos + 1;
        contador_prender_apagar = 0;
    }
  }// Fin de while

  // Reiniciamos los valores
  contador_pasos=0;
  contador_prender_apagar=0;
  // Ponemos los motores en descanso
  GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_0|GPIO_PIN_1,0x00);
  GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);
  GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_0|GPIO_PIN_1,0x00);

}// Fin de la clase mover_motor_horario

void arm_actuate_cb (const std_msgs::Int16MultiArray& msg_degrees_motor)
{
  for (int i=0;i<5;i++){
    if (msg_retroalimentacion.data[i] == 0){
      // Realizamos las conversiones de degrees a pap
      pasos_motor = msg_degrees_motor.data[i]*(800/360); // Es de forma directa
      switch (i){
        case 0:
          // MOVIMIENTO DE LA BASE
          if (pasos_motor>0){
            mover_motor_antihorario(pasos_motor,i);
          }else{
            pasos_motor = pasos_motor * (-1);
            mover_motor_horario(pasos_motor,i);
          }
          break;
        case 1:
          // MOVIMIENTO DEL ESLABON 1
          if (pasos_motor>0){
            pasos_motor = 3.5*pasos_motor;
            mover_motor_antihorario(pasos_motor,i);
          }else{
            pasos_motor = 3.5*pasos_motor;
            pasos_motor = pasos_motor * (-1);
            mover_motor_horario(pasos_motor,i);
          }
          break;
        case 2:
          // MOVIMIENTO DEL ESLABON 2
          if (pasos_motor>0){
            pasos_motor = 3*pasos_motor;
            mover_motor_horario(pasos_motor,i);
          }else{
            pasos_motor = 3*pasos_motor;
            pasos_motor = pasos_motor * (-1);
            mover_motor_antihorario(pasos_motor,i);
          }
          break;
        case 3:
          break;
        case 4:
          break;
      }// Fin del switch
    }else{
      pasos_motor = fabs(msg_degrees_motor.data[i] - msg_retroalimentacion.data[i])*(800/360);
      if (msg_degrees_motor.data[i] > msg_retroalimentacion.data[i]){
        switch (i){
          case 0:
            // MOVIMIENTO DE LA BASE
            mover_motor_antihorario(pasos_motor,i);
            break;
          case 1:
            // MOVIMIENTO DEL ESLABON 1
            pasos_motor = 3.5*pasos_motor;
            mover_motor_horario(pasos_motor,i);
            break;
          case 2:
            // MOVIMIENTO DEL ESLABON 2
            pasos_motor = 3*pasos_motor;
            mover_motor_horario(pasos_motor,i);
            break;
          case 3:
            break;
          case 4:
            break;
        }// Fin del switch
      }else{
        switch (i){
          case 0:
            // MOVIMIENTO DE LA BASE
            mover_motor_horario(pasos_motor,i);
            break;
          case 1:
            // MOVIMIENTO DEL ESLABON 1
            pasos_motor = 3.5*pasos_motor;
            mover_motor_antihorario(pasos_motor,i);
            break;
          case 2:
            // MOVIMIENTO DEL ESLABON 2
            pasos_motor = 3*pasos_motor;
            mover_motor_antihorario(pasos_motor,i);
            break;
          case 3:
            break;
          case 4:
            break;
        }// Fin del switch
      }
    } // Fin del If primario
    //Defnimos la retroalimentacion
    msg_retroalimentacion.data[i] = msg_degrees_motor.data[i];
  }// Fin del for
}// Fin de la funcion arm_actuate_cb
// Creamos nuestro objeto subscriber
// Carac. del subscriber: Empty: se estan publicando empty msg_retroalimentacions en el topico led
// Cada vez que se recibe un mensaje se llama a la funcion msg_retroalimentacion_cb
ros::Subscriber<std_msgs::Int16MultiArray> sub("tiva/arm_actuate", &arm_actuate_cb);

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
  // Perifericos para el motor de la base
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
  // Perifericos para el motor del eslabon 3 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);


  // 3 Paso Configuramos los perifericos
  GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  // Perifericos para el motor de la base
  GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE,GPIO_PIN_0|GPIO_PIN_1);
  // Perifericos para el motor del eslabon 3 
  GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE,GPIO_PIN_0|GPIO_PIN_1);

  // CONFIGURAIONNES ROS A PARTIR DE AQUI

  // Rellenamos nuestros mensjaes de retroalimentacion
  int16_t values[4] = {0,0,0,0};
  msg_retroalimentacion.data = values;
  msg_retroalimentacion.data_length = 4;
  // ROS nodehandle initialization and topic registration
  // Inicializamos nuestro ros node
  nh.initNode();
  // Nos subscribimos
  nh.subscribe(sub);
  nh.advertise(pub);

  while (1)
  {
    // Publicamos la retroalimentacion
    //msg_retroalimentacion.data = msg_retroalimentacion;
    pub.publish(&msg_retroalimentacion);
    // Handle all communications and callbacks.
    nh.spinOnce(); // Hacemos que este continuamente publicandose mensajes

    // Delay for a bit.
    nh.getHardware()->delay(100);
  }// Final while
}// Final int_main

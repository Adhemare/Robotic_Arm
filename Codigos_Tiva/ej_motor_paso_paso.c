//Biblioteca standars
#include<stdint.h> // Es para importar la bibliotecas standar para enteros
#include<stdbool.h> // Es para importar la bibliotecas standar para boleanos

//Bibliotecas que me traen dentro del compedio de Tivaware

//Estas librerias se encuntran dentro de la carpeta inc del Tivaware
#include "inc/hw_memmap.h" //Es para el mapa de memoria
#include "inc/hw_types.h"
#include "inc/tm4c1294ncpdt.h" // Es para la propia placa

//Estas librerias se encuntran dentro de la carpeta driverlib del Tivaware
#include "driverlib/sysctl.h" // Importante es el system control que controla todo el sistema
#include "driverlib/gpio.h" // Para entrada y salida de los pines
#include "driverlib/timer.h" // Libreria para timers
#include "driverlib/interrupt.h" // Libreria para interrupciones

// ---------------------------------------------------------------------------------------------------
// Declaramos variables globales
bool prender_apagar = true;
int nummero_pasos_M1 = 800;
uint32_t contador_prender_apagar_M1 = 0;
uint32_t contador_pasos_M1 = 0;
uint32_t periodo_timer0 = 0;

// Declaramos nuestras funciones Timers o Interrupciones
void Timer0IntHandler(void){
    char cOne, cTwo;

    //TIMER 0
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Limpia la interrupcion

    // LOGICA DEL TIMER
    if (contador_pasos_M1 < nummero_pasos_M1){
        if (prender_apagar){
                GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x01);
            }else{
                GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x03);
            }
            prender_apagar = !prender_apagar;
            contador_prender_apagar_M1 = contador_prender_apagar_M1 + 1;
            if (contador_prender_apagar_M1 == 2){
                contador_pasos_M1 = contador_pasos_M1 + 1;
                contador_prender_apagar_M1 = 0;
            }

    }else{
        GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);
    }


    IntMasterDisable();
    IntMasterEnable();
}

int main(){

    // ----------------------------------------------------------------------------------------------------
    // 1 Paso establecer el reloj de nuestro microcontrolador
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),120000000);
    // ----------------------------------------------------------------------------------------------------


    // ----------------------------------------------------------------------------------------------------
    // 2 Habilitamos los perifericos que vamos a utilizar


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Habilitamos el Timer



    // ----------------------------------------------------------------------------------------------------
    // 3 Configuramos los perifericos
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    // Iniciamos la configuracion del timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // La configuracion del timer es de forma periodica
    periodo_timer0 = 120000000/100;
    TimerLoadSet(TIMER0_BASE, TIMER_A, periodo_timer0); // El tiempo aqui es de 1 segundos
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //TimerIntRegister(TIMER0_BASE, TIMER_A,Timer0IntHandler);
    IntEnable(INT_TIMER0A);
    IntMasterEnable(); //aquel que dice al cpu parapermitir todas las interrupciones
    TimerEnable(TIMER0_BASE, TIMER_A);


    // Declaramos nuestros variable que nos van a servir para la logica de nuestro programa
    while(1){

    }



}

//Biblioteca standars
#include<stdint.h> // Es para importar la bibliotecas standar para enteros
#include<stdbool.h> // Es para importar la bibliotecas standar para boleanos

//Bibliotecas que me traen dentro del compedio de Tivaware

//Estas librerias se encuntran dentro de la carpeta inc del Tivaware
#include "inc/hw_memmap.h" //Es para el mapa de memoria
#include "inc/hw_types.h"
#include "inc/tm4c1294ncpdt.h" // Es para la propia placa

//Estas librerias se encuntran dentro de la carpeta driverlib del Tivaware
#include "driverlib/sysctl.h" // Importante
#include "driverlib/gpio.h" // Para entrada y salida de los pines

// ---------------------------------------------------------------------------------------------------
// Declaramos variables globales
void my_delay(uint32_t delay);

void my_delay(uint32_t delay)
{
    while(delay--);
}

int main(){

    // ----------------------------------------------------------------------------------------------------
    // 1 Paso establecer el reloj de nuestro microcontrolador
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),120000000);
    // ----------------------------------------------------------------------------------------------------


    // ----------------------------------------------------------------------------------------------------
    // 2 Habilitamos los perifericos que vamos a utilizar


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    // ----------------------------------------------------------------------------------------------------
    // 3 Configuramos los perifericos
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE,GPIO_PIN_0|GPIO_PIN_1);

    // Declaramos nuestros variable que nos van a servir para la logica de nuestro programa



    while(1)
    {
        // EN ESTA PARTE YA ES UN CICLO QUE VA A IR REPETIDAMENTE ES NUESTRA LOGICA

        GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_0|GPIO_PIN_1,255);
        SysCtlDelay(40000000);
        //my_delay(40000000);
        GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_0|GPIO_PIN_1,0);
        SysCtlDelay(40000000);
        //my_delay(40000000);

    }
}

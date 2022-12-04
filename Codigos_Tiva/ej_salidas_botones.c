//Biblioteca standars
#include<stdint.h> // Es para importar la bibliotecas standar para enteros
#include<stdbool.h> // Es para importar la bibliotecas standar para boleanos

//Bibliotecas que me traen dentro del compedio de Tivaware

//Estas librerias se encuntran dentro de la carpeta inc del Tivaware
#include "inc/hw_memmap.h" //Es para el mapa de memoria
#include "inc/tm4c1294ncpdt.h" // Es para la propia placa

//Estas librerias se encuntran dentro de la carpeta driverlib del Tivaware
#include "driverlib/sysctl.h" // Importante
#include "driverlib/gpio.h" // Para entrada y salida de los pines

// ---------------------------------------------------------------------------------------------------
// Declaramos variables globales
int push_value_1=0;
int push_value_2=0;

int main(){

    // ----------------------------------------------------------------------------------------------------
    // 1 Paso establecer el reloj de nuestro microcontrolador
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),120000000);
    // ----------------------------------------------------------------------------------------------------


    // ----------------------------------------------------------------------------------------------------
    // 2 Habilitamos los perifericos que vamos a utilizar


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);

    // ----------------------------------------------------------------------------------------------------
    // 3 Configuramos los perifericos
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // TOdos estos pines son de salida
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1); // TOdos estos pines son de entrada

    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    // Declaramos nuestros variable que nos van a servir para la logica de nuestro programa
    uint8_t secuencia_1[5] = {0x00,0x01,0x02,0x04,0x08};
    uint8_t secuencia_2[4] = {0x0F,0x07,0x03,0x01};
    uint8_t contador_1 = 0;
    uint8_t contador_2 = 0;


    while(1)
    {
        // EN ESTA PARTE YA ES UN CICLO QUE VA A IR REPETIDAMENTE ES NUESTRA LOGICA

        //Leemos nuestros valores

        push_value_1 = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0);
        push_value_2 = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_1);
        if(push_value_1 == 0)
        {
            GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,secuencia_1[contador_1]);
            contador_1 = contador_1 + 1;
            if (contador_1 == 5)
            {
                contador_1 = 0;
            }
            SysCtlDelay(20000000);
        }

        if(push_value_2 == 0)
                {
                    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,secuencia_2[contador_2]);
                    contador_2 = contador_2 + 1;
                    if (contador_2 == 4)
                    {
                        contador_2 = 0;
                    }
                    SysCtlDelay(40000000);
        }

        GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);


    }
}

#include <stdbool.h>
#include <stdint.h>
extern "C"
{
    #include <driverlib/sysctl.h>
    #include <driverlib/gpio.h>
    #include "driverlib/interrupt.h"
    #include "driverlib/timer.h"
    #include "driverlib/pwm.h"
    #include "driverlib/pin_map.h"
}
#include <ros.h>
#include <stdlib.h>
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Int32.h"
ros::NodeHandle ntiva;
geometry_msgs::Twist var;

ros::Publisher canal_tiva("control",&var);
bool activa1,activa2,un=true,activa1r,activa2r,activa2p,activa2pr,set1=true,set2,rut,one=true;
int recibe,temp1=0,temp2=0,velservos,limst1=153,limst2,contst=1,vr,q3,poss1,poss2,pp1,pp2,auxvr;
uint8_t st1=0,st2=0,str2=0,str1=0,stp2=0,stpr2=0,c1=0,c2=0,st1set=0,st2set=0,place=0;
volatile uint16_t Ajustes1,Ajustes2,pinza,Ajuster1,Ajuster2,Ajustep1=80,Ajustep2=152,Ajustepr1,Ajustepr2,gr1;
volatile uint32_t Carga,control=0,termine=0;
uint32_t caja,reconocido=0;
double q0,q1,q2;
#define PWM_FREQUENCY 50
#define GPIO_PF1_M0PWM1 0x00050406
#define GPIO_PF2_M0PWM2 0x00050806
#define GPIO_PG0_M0PWM4 0x00060006
void Callback(const geometry_msgs::Quaternion& msg)
{
    q0=msg.x;
    q1=msg.y;
    q2=msg.z;
    q3=int(msg.w);
}
ros::Subscriber<geometry_msgs::Quaternion> subs("TIVA", &Callback);
void GPIOL_Interrupt_Handler(void) {
    int estado =0;
    estado = GPIOIntStatus(GPIO_PORTL_BASE,true);
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0);
    c1=1;
    GPIOIntClear(GPIO_PORTL_BASE,estado);
}
void GPIOM_Interrupt_Handler(void) {
    int estado =0;
    estado = GPIOIntStatus(GPIO_PORTM_BASE,true);
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,2);
    c2=1;
    GPIOIntClear(GPIO_PORTM_BASE,estado);
}
void Timer0IntHandler(void){
    TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    if(activa1 || activa1r|| set1){
        if(GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_0))
            GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_0,0);
        else
            GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_0,1);
    }
}
void Timer1IntHandler(void){
    TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
    if(activa2 || activa2r || activa2p|| activa2pr || set2){
        if(GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_1))
            GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_1,0);
        else
            GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_1,2);
        }
}
void mov1(){
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0);
    activa1=true;
    st1+=contst;
}
void mov1r(){
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,1);
    activa1r=true;
    str1+=contst;
}
void mov2(){
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0);
    activa2=true;
    st2+=contst;
}
void mov2r(){
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,2);
    activa2r=true;
    str2+=contst;
}
void mov2p(){
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0);
    activa2p=true;
    stp2+=contst;
}
void mov2pr(){
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,2);
    activa2pr=true;
    stpr2+=contst;
}
void mov3(int s1){
    Ajustes1+=velservos;
    if(Ajustes1>s1) Ajustes1=s1;
        PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,Ajustes1*Carga/2000);
}
void mov3r(){
    Ajuster1-=velservos;
    if(Ajuster1<80) Ajuster1=80;
        PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,Ajuster1*Carga/2000);
}
void mov3p(int p1){
    Ajustep1+=velservos;
    if(Ajustep1>p1) Ajustep1=p1;
        PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,Ajustep1*Carga/2000);
}
void mov3pr(){
    Ajustepr1-=velservos;
    if(Ajustepr1<80) Ajustepr1=80;
        PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,Ajustepr1*Carga/2000);
}
void mov4(int s2){
    Ajustes2-=velservos;
    if(Ajustes2<s2) Ajustes2=s2;
        PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,Ajustes2*Carga/2000);
}
void mov4r(){
    Ajuster2+=velservos;
    if(Ajuster2>152) Ajuster2=152;
        PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,Ajuster2*Carga/2000);
}
void mov4p(int p2){
    Ajustep2-=velservos;
    if(Ajustep2<p2) Ajustep2=p2;
        PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,Ajustep2*Carga/2000);
}
void mov4pr(){
    Ajustepr2+=velservos;
    if(Ajustepr2>146) Ajustepr2=146;
        PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,Ajustepr2*Carga/2000);
}
void apinza(){
    pinza=108;
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,pinza*Carga/2000);
}
void cpinza(){
    gr1=220;
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,gr1*Carga/2000);
}
int main(void)
{
    volatile uint32_t RelojPWM;
    volatile uint32_t TIVAfreq;
    uint32_t pm1,pm2;
    Ajustes1=80;
    Ajustes2=146;
    pinza=220;
    pm1=TIVAfreq / 300;
    pm2=TIVAfreq / 100;
    MAP_FPUEnable();
    MAP_FPULazyStackingEnable();
    TIVAfreq=MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), TM4C129FREQ);
    RelojPWM=TIVAfreq/64;
    Carga=(RelojPWM/PWM_FREQUENCY)-1;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,GPIO_PIN_0|GPIO_PIN_1);//DIR
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE,GPIO_PIN_0|GPIO_PIN_1);//PULL
    GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTG_BASE,GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinConfigure(GPIO_PG0_M0PWM4);
    PWMGenConfigure(PWM0_BASE,PWM_GEN_0,PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM0_BASE,PWM_GEN_1,PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM0_BASE,PWM_GEN_2,PWM_GEN_MODE_DOWN);
    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC);
    GPIOPadConfigSet(GPIO_PORTL_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTM_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTL_BASE, GPIO_INT_PIN_2,GPIO_FALLING_EDGE);
    GPIOIntTypeSet(GPIO_PORTM_BASE, GPIO_INT_PIN_2,GPIO_FALLING_EDGE);
    TimerLoadSet(TIMER0_BASE,TIMER_A,pm1-1);
    TimerLoadSet(TIMER1_BASE,TIMER_A,pm2-1);
    PWMClockSet(PWM0_BASE,PWM_SYSCLK_DIV_64);
    PWMGenPeriodSet(PWM0_BASE,PWM_GEN_0,Carga);
    PWMGenPeriodSet(PWM0_BASE,PWM_GEN_1,Carga);
    PWMGenPeriodSet(PWM0_BASE,PWM_GEN_2,Carga);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,Ajustes1*Carga/2000);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,Ajustes2*Carga/2000);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,pinza*Carga/2000);
    GPIOIntRegister(GPIO_PORTL_BASE,GPIOL_Interrupt_Handler);
    GPIOIntRegister(GPIO_PORTM_BASE,GPIOM_Interrupt_Handler);
    TimerIntRegister(TIMER0_BASE, TIMER_A,Timer0IntHandler);
    TimerIntRegister(TIMER1_BASE, TIMER_A,Timer1IntHandler);
    GPIOIntEnable(GPIO_PORTL_BASE, GPIO_INT_PIN_2);
    GPIOIntEnable(GPIO_PORTM_BASE, GPIO_INT_PIN_2);
    TimerEnable(TIMER0_BASE, TIMER_A);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER1_BASE, TIMER_A);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    PWMGenEnable(PWM0_BASE,PWM_GEN_0);
    PWMGenEnable(PWM0_BASE,PWM_GEN_1);
    PWMGenEnable(PWM0_BASE,PWM_GEN_2);
    IntMasterEnable();
    PWMOutputState(PWM0_BASE,PWM_OUT_1_BIT,true);
    PWMOutputState(PWM0_BASE,PWM_OUT_2_BIT,true);
    PWMOutputState(PWM0_BASE,PWM_OUT_4_BIT,true);
    ntiva.initNode();
    ntiva.advertise(canal_tiva);
    ntiva.subscribe(subs);
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,1);
    GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0);
    while (1)
    {
        if(recibe<=1 && rut && (q0!=0 || q1!=0 || q2!=0)){
            pm1=TIVAfreq / 100;
            TimerLoadSet(TIMER0_BASE,TIMER_A,pm1-1);
            poss1=160;poss2=110;pp1=100;
            if(q0==1) place=84;
            if(q1==1) place=127;
            if(q2==1) place=170;
            limst2=28; vr=6;pp2=86;
            velservos=4;
            if (one){
                Ajuster1=poss1;
                Ajuster2=poss2;
                Ajustepr1=pp1;
                Ajustepr2=pp2;
                one=false;
            }
            if(q3%2==0) {//PICKING
                if(st1>=limst1) {
                    activa1=false;
                    if (st2<limst2) mov2();
                    if(Ajuster1!=80) mov3(poss1);
                    if (Ajustes2!=poss2) mov4(poss2);
                    if(st2>=limst2) activa2=false;
                    temp1++;
                    if(temp1>30 && temp1<43) apinza();
                    if(Ajustes2==poss2 && temp1>43){
                        if(str2<limst2) {
                            mov2r();
                            mov3r();
                            mov4r();
                        }
                        if (str2>=limst2 && str1<place){
                            mov1r();
                            activa2r=false;
                        }
                        if(str1>=place) {
                            activa1r=false;
                            if(stp2<limst2-vr){
                                mov4p(pp2);
                                mov3p(pp1);
                                mov2p();
                            }
                            if(stp2>=limst2-vr) {
                                activa2p=false;
                                temp2++;
                                if(temp2>9 && temp2<20) cpinza();
                                if (temp2>=20 && Ajustep1==pp1){
                                    if(stpr2<limst2-vr){
                                        mov3pr();
                                        mov4pr();
                                        mov2pr();
                                    }
                                    if(stpr2>=limst2-vr) {
                                        activa2pr=false;
                                        if(recibe==1) rut=false;
                                    }
                                }
                            }
                        }
                    }
                }else{
                mov1();
                }
            }//PICKING
            else{//PLACING
                limst2=27;vr=6;pp2=100;
                limst1=153;
                if(stp2<limst2-vr){
                    mov3p(pp1);
                    mov4p(pp2);
                    mov2p();
                }
                if(stp2>=limst2-vr) {
                    activa2p=false;
                    temp2++;
                    if(temp2>9 && temp2<20) apinza();
                    if (temp2>=20 && Ajustep1==pp1){
                        if(stpr2<limst2-vr){
                            mov3pr();
                            if(Ajustepr1<pp1) mov4pr();
                            mov2pr();
                        }
                        if(stpr2>=limst2-vr && str1<place) {
                            activa2pr=false;
                            mov1r();
                        }
                        if(str1>=place) {
                            activa1r=false;
                            if (st2<limst2) mov2();
                            if(Ajuster1!=80) mov3(poss1);
                            if (Ajustes2!=poss2) mov4(poss2);
                            if(st2>=limst2) activa2=false;
                            if(st2==20) control++;
                            temp1++;
                            if(temp1>30 && temp1<43) cpinza();
                            if(Ajustes2==poss2 && temp1>43){
                                if(str2<limst2) {
                                    mov2r();
                                    mov3r();
                                    mov4r();
                                }
                                switch(control){
                                    case 1:place=126;break;
                                    case 2:place=168; break;
                                    case 3:place=96;break;
                                }
                                if (str2>=limst2 && st1<place){
                                    mov1();
                                    activa2r=false;
                                }
                                if(st1>=place){ activa1=false;
                                if(recibe==1) rut=false;
                                if(control==6) control=0;
                                }
                            }
                        }
                    }
                }
            }//PLACING
        }else{
            if(recibe==0 and un==true){
                if(c1==1) {
                    if(st1set<=40) st1set++;
                    else set1=false;
                }
                if(!set1) set2=true;
                if(c2==1) {
                    if(st2set<=28){ 
                        st2set++;
                    }else{
                        set2=false;recibe=1;rut=true;un=false;
                    }
                }
            }else{
                st1=0,st2=0,str2=0,str1=0,stp2=0,stpr2=0,one=true;
                Ajustep1=80,Ajustep2=152;
                temp1=0,temp2=0;
                Ajustes1=80;
                Ajustes2=146;
                pinza=220;
                PWMPulseWidthSet(PWM0_BASE,PWM_OUT_1,Ajustes1*Carga/2000);
                PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,Ajustes2*Carga/2000);
                PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,pinza*Carga/2000);
                if(q0!=0 || q1!=0 || q2!=0){
                    recibe=2;limst1=place;
                }else {
                    recibe=1;
                }
                rut=true;
            }
        }
        var.linear.x=q0;
        var.linear.y=q1;
        var.linear.z=q2;
        var.angular.x=q3;
        var.angular.y=control;
        var.angular.z=recibe;
        canal_tiva.publish(&var);
        ntiva.spinOnce();
        ntiva.getHardware()->delay(100);
    }
}

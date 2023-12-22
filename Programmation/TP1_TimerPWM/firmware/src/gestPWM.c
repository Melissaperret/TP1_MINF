/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2016-2017
//
//	Auteur 		: 	C. HUBER
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.42 + Harmony 1.08
//
/*--------------------------------------------------------*/



#include "GestPWM.h"
#include "app.h"
#include "Mc32DriverAdc.h"
#include "Mc32DriverLcd.h"
#include "C:\microchip\harmony\v2_06\framework\peripheral\oc\processor\oc_p32mx795f512l.h"


S_pwmSettings PWMData;      // pour les settings



void GPWM_Initialize(S_pwmSettings *pData)
{ 
    // Init état du pont en H
    BSP_EnableHbrige();
    // lance les timers et OC
    
    // Démarrage des OCs
    DRV_OC0_Start();
    DRV_OC1_Start();

    // Démarrage des timers 1 à 4
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
    DRV_TMR3_Start();
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData)	
{
    int16_t chan0Val0a198 = 0; 
    static S_ADCResults tabValADC[10]; 
    static uint8_t i = 0;
    static uint8_t j = 0;
    float moyenneChan0; 
    float moyenneChan1; 

    // Lecture du convertisseur AD
    tabValADC[i] = BSP_ReadAllADC();
    
    moyenneChan0 = 0; 
    moyenneChan1 = 0;
    
    for(j = 0; j < 10 ; j++)
    {
        moyenneChan0+= tabValADC[j].Chan0;
        moyenneChan1 += tabValADC[j].Chan1;
    }
     
    moyenneChan0 = moyenneChan0 / 10; 
    moyenneChan1 = moyenneChan1 / 10; 
    
    chan0Val0a198 = ((moyenneChan0*198)/1023);    // 1023 * X = 198 => X = 198/1023
    pData->SpeedSetting = chan0Val0a198 - 99;
    pData->absSpeed = abs(pData->SpeedSetting);
    
    pData->absAngle = ((moyenneChan1*180)/1023);  // 1023 * X = 180 => X = 180/1023
    pData->AngleSetting = pData->absAngle - 90;
    
    //On stock les 10 dernières valeurs du potentiomètre 
    i++;
    if(i >= 10)
    {
        i=0;
    }
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{ 
    lcd_gotoxy(1,1); 
    printf_lcd("TP1 PWM 2023-2024");
    lcd_gotoxy(1,2); 
    printf_lcd("SpeedSetting %3d", pData->SpeedSetting);
    lcd_gotoxy(1,3); 
    printf_lcd("adbSpeed      %2d", pData->absSpeed);
    lcd_gotoxy(1,4); 
    printf_lcd("Angle        %3d", pData->AngleSetting);
    
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    
    if(pData->SpeedSetting > 0)
    {
        AIN1_HBRIDGE_W = 1;
        AIN2_HBRIDGE_W = 0;
    }
    else if (pData->SpeedSetting < 0)
    {
        AIN1_HBRIDGE_W = 0;
        AIN2_HBRIDGE_W = 1;
    }
    else 
    {
        AIN1_HBRIDGE_W = 0;
        AIN2_HBRIDGE_W = 0;
    }
    
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, (pData->absSpeed*1999)/100);
    
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, (((pData->absAngle*4500)/180)+1500));
    
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    
    if(compteur < pData->absSpeed)
    {
        LED2_W = 1;
    }
    else
    {
        LED2_W = 0;
    }
}



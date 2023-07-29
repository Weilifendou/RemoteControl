#include "iwdg.h"

void Iwdg_Init(void)
{
    //1�� ȡ���Ĵ���д������
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    //2�����ö������Ź���Ԥ��Ƶϵ����ȷ��ʱ��:125HZ
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    //3�����ÿ��Ź���װ��ֵ��ȷ�����ʱ��:2s
    IWDG_SetReload(250);
    //4��ʹ�ܿ��Ź�
    IWDG_Enable();
    IWDG_ReloadCounter();
}








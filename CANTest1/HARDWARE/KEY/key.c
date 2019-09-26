/*******************************************************************************
* @File     key.c
* @Brief    KEY�����ļ�
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#include "key.h"
#include "delay.h"

/*******************************************************************************
* @Brief    ������ʼ������
* @Param    None
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();               //����GPIOHʱ��

    GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3; //PH2,3
    GPIO_Initure.Mode = GPIO_MODE_INPUT;        //����
    GPIO_Initure.Pull = GPIO_PULLUP;            //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //����
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
}

/*******************************************************************************
* @Brief    ����������
* @Param    mode:0,��֧��������;1,֧��������
* @Return   0--4:��Ӧ��������״̬
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
*           �˺�������Ӧ���ȼ�,KEY0>KEY1
* @Author   Jove
*******************************************************************************/
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;       //�����ɿ���־
    if (mode == 1)key_up = 1;   //֧������
    if (key_up && (KEY0 == 0 || KEY1 == 0))
    {
        delay_ms(10);
        key_up = 0;
        if (KEY0 == 0)       return KEY0_PRES;
        else if (KEY1 == 0)  return KEY1_PRES;
    }
    else if (KEY0 == 1 && KEY1 == 1)key_up = 1;
    return NO_KEY;   //�ް�������
}

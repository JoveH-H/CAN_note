/*******************************************************************************
* @File     key.c
* @Brief    KEY驱动文件
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#include "key.h"
#include "delay.h"

/*******************************************************************************
* @Brief    按键初始化函数
* @Param    None
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();               //开启GPIOH时钟

    GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3; //PH2,3
    GPIO_Initure.Mode = GPIO_MODE_INPUT;        //输入
    GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
}

/*******************************************************************************
* @Brief    按键处理函数
* @Param    mode:0,不支持连续按;1,支持连续按
* @Return   0--4:对应按键按下状态
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
*           此函数有响应优先级,KEY0>KEY1
* @Author   Jove
*******************************************************************************/
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;       //按键松开标志
    if (mode == 1)key_up = 1;   //支持连按
    if (key_up && (KEY0 == 0 || KEY1 == 0))
    {
        delay_ms(10);
        key_up = 0;
        if (KEY0 == 0)       return KEY0_PRES;
        else if (KEY1 == 0)  return KEY1_PRES;
    }
    else if (KEY0 == 1 && KEY1 == 1)key_up = 1;
    return NO_KEY;   //无按键按下
}

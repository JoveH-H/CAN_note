/*******************************************************************************
* @File     led.c
* @Brief    LED驱动文件
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#include "led.h"

/*******************************************************************************
* @Brief    LED初始化函数
* @Param    None
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();               //开启GPIOB时钟

    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1; //PB1,0
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;    //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //PB0置1，默认初始化后灯灭
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //PB1置1，默认初始化后灯灭
}

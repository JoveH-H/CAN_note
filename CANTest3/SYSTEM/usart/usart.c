/*******************************************************************************
* @File     usart.c
* @Brief    串口通讯驱动文件
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#include "usart.h"
#include "delay.h"	 

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //FreeRTOS使用	 
#endif

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数 
int fputc(int ch, FILE* f)
{
    while ((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕   
    USART1->DR = (u8)ch;
    return ch;
}

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0;               //接收状态标记	

u8 aRxBuffer[RXBUFFERSIZE];         //HAL库使用的串口接收缓冲
UART_HandleTypeDef USART1_Handler;  //UART句柄

/*******************************************************************************
* @Brief    串口1初始化函数
* @Param    bound:波特率
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void uart_init(u32 bound)
{
    USART1_Handler.Instance = USART1;                       //USART1
    USART1_Handler.Init.BaudRate = bound;                   //波特率
    USART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;    //字长为8位数据格式
    USART1_Handler.Init.StopBits = UART_STOPBITS_1;         //一个停止位
    USART1_Handler.Init.Parity = UART_PARITY_NONE;          //无奇偶校验位
    USART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;    //无硬件流控
    USART1_Handler.Init.Mode = UART_MODE_TX_RX;             //收发模式
    HAL_UART_Init(&USART1_Handler);                         //HAL_UART_Init()会使能UART1
}

/*******************************************************************************
* @Brief    串口底层初始化函数
* @Param    huart:串口句柄
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* 			此函数会被HAL_UART_Init()调用
* @Author   Jove
*******************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_Initure;                          //GPIO端口设置

    if (huart->Instance == USART1)                          //如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();                       //使能GPIOA时钟
        __HAL_RCC_USART1_CLK_ENABLE();                      //使能USART1时钟

        GPIO_Initure.Pin = GPIO_PIN_9;                      //PA9
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;                //复用推挽输出
        GPIO_Initure.Pull = GPIO_PULLUP;                    //上拉
        GPIO_Initure.Speed = GPIO_SPEED_FAST;               //高速
        GPIO_Initure.Alternate = GPIO_AF7_USART1;           //复用为USART1
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);                //初始化PA9

        GPIO_Initure.Pin = GPIO_PIN_10;                     //PA10
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);                //初始化PA10
        __HAL_UART_DISABLE_IT(huart, UART_IT_TC);
#if EN_USART1_RX
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);          //开启接收中断
        HAL_NVIC_EnableIRQ(USART1_IRQn);                    //使能USART1中断通道
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);            //抢占优先级3，子优先级3
#endif	
    }

}

/*******************************************************************************
* @Brief    串口1中断服务函数
* @Param    None
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void USART1_IRQHandler(void)
{
    u8 Res;
    if ((__HAL_UART_GET_FLAG(&USART1_Handler, UART_FLAG_RXNE) != RESET))    //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        HAL_UART_Receive(&USART1_Handler, &Res, 1, 1000);
        if ((USART_RX_STA & 0x8000) == 0)           //接收未完成
        {
            if (USART_RX_STA & 0x4000)              //接收到了0x0d
            {
                if (Res != 0x0a)USART_RX_STA = 0;   //接收错误,重新开始
                else USART_RX_STA |= 0x8000;        //接收完成了 
            }
            else //还没收到0X0D
            {
                if (Res == 0x0d)USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))USART_RX_STA = 0;//接收数据错误,重新开始接收	  
                }
            }
        }
    }
    HAL_UART_IRQHandler(&USART1_Handler);
}
#endif	

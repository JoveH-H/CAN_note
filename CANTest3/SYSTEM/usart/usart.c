/*******************************************************************************
* @File     usart.c
* @Brief    ����ͨѶ�����ļ�
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#include "usart.h"
#include "delay.h"	 

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //FreeRTOSʹ��	 
#endif

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE
{
    int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc���� 
int fputc(int ch, FILE* f)
{
    while ((USART1->SR & 0X40) == 0);//ѭ������,ֱ���������   
    USART1->DR = (u8)ch;
    return ch;
}

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0;               //����״̬���	

u8 aRxBuffer[RXBUFFERSIZE];         //HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef USART1_Handler;  //UART���

/*******************************************************************************
* @Brief    ����1��ʼ������
* @Param    bound:������
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void uart_init(u32 bound)
{
    USART1_Handler.Instance = USART1;                       //USART1
    USART1_Handler.Init.BaudRate = bound;                   //������
    USART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;    //�ֳ�Ϊ8λ���ݸ�ʽ
    USART1_Handler.Init.StopBits = UART_STOPBITS_1;         //һ��ֹͣλ
    USART1_Handler.Init.Parity = UART_PARITY_NONE;          //����żУ��λ
    USART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;    //��Ӳ������
    USART1_Handler.Init.Mode = UART_MODE_TX_RX;             //�շ�ģʽ
    HAL_UART_Init(&USART1_Handler);                         //HAL_UART_Init()��ʹ��UART1
}

/*******************************************************************************
* @Brief    ���ڵײ��ʼ������
* @Param    huart:���ھ��
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* 			�˺����ᱻHAL_UART_Init()����
* @Author   Jove
*******************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_Initure;                          //GPIO�˿�����

    if (huart->Instance == USART1)                          //����Ǵ���1�����д���1 MSP��ʼ��
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();                       //ʹ��GPIOAʱ��
        __HAL_RCC_USART1_CLK_ENABLE();                      //ʹ��USART1ʱ��

        GPIO_Initure.Pin = GPIO_PIN_9;                      //PA9
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;                //�����������
        GPIO_Initure.Pull = GPIO_PULLUP;                    //����
        GPIO_Initure.Speed = GPIO_SPEED_FAST;               //����
        GPIO_Initure.Alternate = GPIO_AF7_USART1;           //����ΪUSART1
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);                //��ʼ��PA9

        GPIO_Initure.Pin = GPIO_PIN_10;                     //PA10
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);                //��ʼ��PA10
        __HAL_UART_DISABLE_IT(huart, UART_IT_TC);
#if EN_USART1_RX
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);          //���������ж�
        HAL_NVIC_EnableIRQ(USART1_IRQn);                    //ʹ��USART1�ж�ͨ��
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);            //��ռ���ȼ�3�������ȼ�3
#endif	
    }

}

/*******************************************************************************
* @Brief    ����1�жϷ�����
* @Param    None
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void USART1_IRQHandler(void)
{
    u8 Res;
    if ((__HAL_UART_GET_FLAG(&USART1_Handler, UART_FLAG_RXNE) != RESET))    //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        HAL_UART_Receive(&USART1_Handler, &Res, 1, 1000);
        if ((USART_RX_STA & 0x8000) == 0)           //����δ���
        {
            if (USART_RX_STA & 0x4000)              //���յ���0x0d
            {
                if (Res != 0x0a)USART_RX_STA = 0;   //���մ���,���¿�ʼ
                else USART_RX_STA |= 0x8000;        //��������� 
            }
            else //��û�յ�0X0D
            {
                if (Res == 0x0d)USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))USART_RX_STA = 0;//�������ݴ���,���¿�ʼ����	  
                }
            }
        }
    }
    HAL_UART_IRQHandler(&USART1_Handler);
}
#endif	

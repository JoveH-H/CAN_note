/*******************************************************************************
* @File     main.c
* @Brief    �������ļ�
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
            CAN1ͨѶ����
* @Author   Jove
*******************************************************************************/

#include "stdlib.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "can.h"

/*******************************************************************************
* @Brief    CAN������ʾ����
* @Param    *buf:������
*           y   :��ʾ��ʼ���y����
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void LCD_Show_CAN_Msg(u8* buf, u16 y)
{
    u8 i = 0;                   //��������ֵ

    for (i = 0; i < 8; i++)
    {
        if (i < 4)              //��ʾ��һ������
        {
            LCD_ShowxNum(30 + i * 32, y, buf[i], 3, 16, 0X80);
        }
        else                    //��ʾ�ڶ�������
        {
            LCD_ShowxNum(30 + (i - 4) * 32, y + 20, buf[i], 3, 16, 0X80);
        }
    }
}

/*******************************************************************************
* @Brief    ������
* @Param    None
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
int main(void)
{
    u8 key, res, rcv;                   //����״̬��������������Ϣ��������Ϣ
    u8 i = 0, t = 0;                    //��������ֵ
    u8 canbuf[8];                       //������
    u8 mode = 0;                        //CANģʽ

    HAL_Init();                         //��ʼ��HAL��   
    Stm32_Clock_Init(360, 25, 2, 8);    //����ʱ��,180Mhz
    uart_init(115200);                  //��ʼ��USART
    delay_init(180);                    //��ʼ����ʱ����
    LED_Init();                         //��ʼ��LED 
    KEY_Init();                         //��ʼ������
    LCD_Init();                         //��ʼ��LCD

    //CAN1��ʼ��,Ĭ�ϲ�����500Kbps�ػ�ģʽ  
    CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 6, CAN_MODE_LOOPBACK);

    POINT_COLOR = RED;                                                  //��������Ϊ��ɫ
    LCD_ShowString(30, 35, 200, 16, 16, (u8*)"CAN TEST");               //��ʾ���Ա���
    LCD_ShowString(30, 60, 200, 16, 16, (u8*)"KEY0:Send KEY1:Mode");    //��ʾ����������Ϣ
    LCD_ShowString(30, 90, 200, 16, 16, (u8*)"LoopBack Mode");          //��ʾģʽ��Ϣ
    POINT_COLOR = BLUE;                                                 //��������Ϊ��ɫ         
    LCD_ShowString(30, 120, 200, 16, 16, (u8*)"Send Data:");            //��ʾ���͵�����    
    LCD_ShowString(30, 190, 200, 16, 16, (u8*)"Receive Data:");         //��ʾ���յ�������        
    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY0_PRES)    //KEY0����,����һ������
        {
            //��䷢�ͻ�����
            for (i = 0; i < 8; i++)
            {
                canbuf[i] = rand() % 256;
            }

            //��ʾCAN���͵�����
            LCD_Show_CAN_Msg(canbuf, 140);

            //��������
            res = CAN1_Send_Msg(canbuf, 8);

            //��ʾ����״̬
            if (res)
            {
                POINT_COLOR = RED;                                           //��������Ϊ��ɫ
                LCD_ShowString(30 + 80, 120, 200, 16, 16, (u8*)"Failed");    //��ʾ����ʧ��
                POINT_COLOR = BLUE;                                          //��������Ϊ��ɫ    
            }
            else
            {
                LCD_ShowString(30 + 80, 120, 200, 16, 16, (u8*)"OK    ");    //��ʾ���ͳɹ�
            }
        }
        else if (key == KEY1_PRES)    //KEY1���£��ı�CAN�Ĺ���ģʽ
        {
            //�л�ģʽ״̬��־λ
            mode = !mode;

            //�л�ģʽ:��ͨģʽ,�ػ�ģʽ
            CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 6, mode ? CAN_MODE_NORMAL : CAN_MODE_LOOPBACK);

            //������ʾģʽ
            POINT_COLOR = RED;
            if (mode)
            {
                LCD_ShowString(30, 90, 200, 16, 16, (u8*)"Nnormal Mode ");
            }
            else
            {
                LCD_ShowString(30, 90, 200, 16, 16, (u8*)"LoopBack Mode");
            }
            POINT_COLOR = BLUE;
        }

        //��������
        rcv = CAN1_Receive_Msg(canbuf);
        if (rcv)    //���յ�������
        {
            LCD_Fill(30, 210, 160, 250, WHITE);   //���֮ǰ����ʾ
            LCD_Show_CAN_Msg(canbuf, 210);        //��ʾCAN���ܵ�����
        }

        //ʱ������ۼ�
        t++;
        delay_ms(10);
        if (t == 50)
        {
            LED0 = !LED0;    //��ʾϵͳ��������    
            t = 0;
        }
    }
}

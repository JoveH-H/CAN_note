/*******************************************************************************
* @File     main.c
* @Brief    主函数文件
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
            CAN1通讯测试
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
* @Brief    CAN数据显示函数
* @Param    *buf:数据组
*           y   :显示起始点的y坐标
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void LCD_Show_CAN_Msg(u8* buf, u16 y)
{
    u8 i = 0;                   //计数辅助值

    for (i = 0; i < 8; i++)
    {
        if (i < 4)              //显示第一行数据
        {
            LCD_ShowxNum(30 + i * 32, y, buf[i], 3, 16, 0X80);
        }
        else                    //显示第二行数据
        {
            LCD_ShowxNum(30 + (i - 4) * 32, y + 20, buf[i], 3, 16, 0X80);
        }
    }
}

/*******************************************************************************
* @Brief    主函数
* @Param    None
* @Return   None
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
int main(void)
{
    u8 key, res, rcv;                   //返回状态：按键、发送消息、接受消息
    u8 i = 0, t = 0;                    //计数辅助值
    u8 canbuf[8];                       //数据组
    u8 mode = 0;                        //CAN模式

    HAL_Init();                         //初始化HAL库   
    Stm32_Clock_Init(360, 25, 2, 8);    //设置时钟,180Mhz
    uart_init(115200);                  //初始化USART
    delay_init(180);                    //初始化延时函数
    LED_Init();                         //初始化LED 
    KEY_Init();                         //初始化按键
    LCD_Init();                         //初始化LCD

    //CAN1初始化,默认波特率500Kbps回环模式  
    CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 6, CAN_MODE_LOOPBACK);

    POINT_COLOR = RED;                                                  //设置字体为红色
    LCD_ShowString(30, 35, 200, 16, 16, (u8*)"CAN TEST");               //显示测试标题
    LCD_ShowString(30, 60, 200, 16, 16, (u8*)"KEY0:Send KEY1:Mode");    //显示按键功能信息
    LCD_ShowString(30, 90, 200, 16, 16, (u8*)"LoopBack Mode");          //显示模式信息
    POINT_COLOR = BLUE;                                                 //设置字体为蓝色         
    LCD_ShowString(30, 120, 200, 16, 16, (u8*)"Send Data:");            //提示发送的数据    
    LCD_ShowString(30, 190, 200, 16, 16, (u8*)"Receive Data:");         //提示接收到的数据        
    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY0_PRES)    //KEY0按下,发送一次数据
        {
            //填充发送缓冲区
            for (i = 0; i < 8; i++)
            {
                canbuf[i] = rand() % 256;
            }

            //显示CAN发送的数据
            LCD_Show_CAN_Msg(canbuf, 140);

            //发送数据
            res = CAN1_Send_Msg(canbuf, 8);

            //显示发送状态
            if (res)
            {
                POINT_COLOR = RED;                                           //设置字体为红色
                LCD_ShowString(30 + 80, 120, 200, 16, 16, (u8*)"Failed");    //提示发送失败
                POINT_COLOR = BLUE;                                          //设置字体为蓝色    
            }
            else
            {
                LCD_ShowString(30 + 80, 120, 200, 16, 16, (u8*)"OK    ");    //提示发送成功
            }
        }
        else if (key == KEY1_PRES)    //KEY1按下，改变CAN的工作模式
        {
            //切换模式状态标志位
            mode = !mode;

            //切换模式:普通模式,回环模式
            CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 6, mode ? CAN_MODE_NORMAL : CAN_MODE_LOOPBACK);

            //更新显示模式
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

        //接收数据
        rcv = CAN1_Receive_Msg(canbuf);
        if (rcv)    //接收到有数据
        {
            LCD_Fill(30, 210, 160, 250, WHITE);   //清除之前的显示
            LCD_Show_CAN_Msg(canbuf, 210);        //显示CAN接受的数据
        }

        //时间计数累加
        t++;
        delay_ms(10);
        if (t == 50)
        {
            LED0 = !LED0;    //提示系统正在运行    
            t = 0;
        }
    }
}

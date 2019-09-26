/*******************************************************************************
* @File     main.c
* @Brief    主函数文件
* @Date     2019-09-17
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
*           CAN1通讯测试
* @Author   Jove
*******************************************************************************/

#include "stdlib.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "can.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO     1           //任务优先级
#define START_STK_SIZE      128         //任务堆栈大小    
TaskHandle_t StartTask_Handler;         //任务句柄
void start_task(void* pvParameters);    //任务函数

#define TASK1_TASK_PRIO     2           //任务优先级
#define TASK1_STK_SIZE      128         //任务堆栈大小    
TaskHandle_t Task1Task_Handler;         //任务句柄
void task1_task(void* pvParameters);    //任务函数

#define TASK2_TASK_PRIO     3           //任务优先级
#define TASK2_STK_SIZE      128         //任务堆栈大小    
TaskHandle_t Task2Task_Handler;         //任务句柄
void task2_task(void* pvParameters);    //任务函数


/*******************************************************************************
* @Brief    主函数
* @Param    None
* @Return   None
* @Date     2019-09-17
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
int main(void)
{
    HAL_Init();                         //初始化HAL库   
    Stm32_Clock_Init(360, 25, 2, 8);    //设置时钟,180Mhz
    uart_init(115200);                  //初始化USART
    delay_init(180);                    //初始化延时函数
    LED_Init();                         //初始化LED 
    KEY_Init();                         //初始化按键

    //CAN1初始化,默认波特率500Kbps回环模式  
    CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 6, CAN_MODE_LOOPBACK);
    printf("欢迎使用 HH CAN 驱动测试！\r\n");     //显示测试标题
    printf("KEY0:发送 KEY1:模式 \r\n");          //显示按键功能信息
    printf("回环模式 \r\n");                     //显示模式信息

    //创建开始任务
    xTaskCreate((TaskFunction_t)start_task,     //任务函数
        (const char*)"start_task",              //任务名称
        (uint16_t)START_STK_SIZE,               //任务堆栈大小
        (void*)NULL,                            //传递给任务函数的参数
        (UBaseType_t)START_TASK_PRIO,           //任务优先级
        (TaskHandle_t*)&StartTask_Handler);     //任务句柄                
    vTaskStartScheduler();                      //开启任务调度    
}

/*******************************************************************************
* @Brief    开始任务任务函数
* @Param    *pvParameters:任务传递形参
* @Return   None
* @Date     2019-09-13
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void start_task(void* pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t)task1_task,
        (const char*)"task1_task",
        (uint16_t)TASK1_STK_SIZE,
        (void*)NULL,
        (UBaseType_t)TASK1_TASK_PRIO,
        (TaskHandle_t*)&Task1Task_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t)task2_task,
        (const char*)"task2_task",
        (uint16_t)TASK2_STK_SIZE,
        (void*)NULL,
        (UBaseType_t)TASK2_TASK_PRIO,
        (TaskHandle_t*)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

/*******************************************************************************
* @Brief    task1任务函数
* @Param    *pvParameters:任务传递形参
* @Return   None
* @Date     2019-09-17
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void task1_task(void* pvParameters)
{
    u8 key, res;    //返回状态：按键、发送消息
    u8 i = 0;       //计数辅助值
    u8 canbuf[8];   //数据组
    u8 mode = 0;    //CAN模式

    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY0_PRES)   //KEY0按下,发送一次数据
        {
            //填充发送缓冲区
            for (i = 0; i < 8; i++)
            {
                canbuf[i] = rand() % 256;
            }

            //显示CAN发送的数据
            printf("\r\n发送的数据：");
            for (i = 0; i < 8; i++)
            {
                printf("%3d ", canbuf[i]);
            }

            //发送数据
            res = CAN1_Send_Msg(canbuf, 8);

            //显示发送状态
            if (res)
            {
                printf(" 发送失败 \r\n");   //提示发送失败
            }
            else
            {
                printf(" 发送成功 \r\n");   //提示发送成功
            }
        }
        else if (key == KEY1_PRES)  //KEY1按下，改变CAN的工作模式
        {
            //切换模式状态标志位
            mode = !mode;

            //切换模式:普通模式,回环模式
            CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 6, mode ? CAN_MODE_NORMAL : CAN_MODE_LOOPBACK);

            //更新显示模式
            if (mode)
            {
                printf("\r\n普通模式 \r\n");
            }
            else
            {
                printf("\r\n回环模式 \r\n");
            }
        }

        vTaskDelay(50);     //延迟50ms
    }
}

/*******************************************************************************
* @Brief    task2任务函数
* @Param    *pvParameters:任务传递形参
* @Return   None
* @Date     2019-09-17
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void task2_task(void* pvParameters)
{
    u8 t = 0;   //计数辅助值

    while (1)
    {
        //时间计数累加
        t++;
        if (t == 50)
        {
            LED0 = !LED0;   //提示系统正在运行    
            t = 0;
        }

        vTaskDelay(10);     //延迟10ms
    }
}

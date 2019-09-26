/*******************************************************************************
* @File     main.c
* @Brief    �������ļ�
* @Date     2019-09-19
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
*           CAN1ͨѶ����
* @Author   Jove
*******************************************************************************/

#include "stdlib.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "bsp_can.h"
#include "bsp_timer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "TestMaster.h"
#include "TestSlave.h"

#define START_TASK_PRIO        1           //�������ȼ�
#define START_STK_SIZE         128         //�����ջ��С    
TaskHandle_t StartTask_Handler;            //������
void start_task(void* pvParameters);       //������

#define TASK1_TASK_PRIO        2           //�������ȼ�
#define TASK1_STK_SIZE         128         //�����ջ��С    
TaskHandle_t Task1Task_Handler;            //������
void task1_task(void* pvParameters);       //������

#define TASK2_TASK_PRIO        3           //�������ȼ�
#define TASK2_STK_SIZE         128         //�����ջ��С    
TaskHandle_t Task2Task_Handler;            //������
void task2_task(void* pvParameters);       //������

/*******************************************************************************
* @Brief    ������
* @Param    None
* @Return   None
* @Date     2019-09-19
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
int main(void)
{
    HAL_Init();                            //��ʼ��HAL��   
    Stm32_Clock_Init(360, 25, 2, 8);       //����ʱ��,180Mhz
    uart_init(115200);                     //��ʼ��USART
    delay_init(180);                       //��ʼ����ʱ����
    LED_Init();                            //��ʼ��LED 
    KEY_Init();                            //��ʼ������

    //CAN1��ʼ��,Ĭ�ϲ�����500Kbps��ͨģʽ  
    CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 6, CAN_MODE_NORMAL);

    //��ʾ���Ա���
    printf("��ӭʹ�� HH CAN �������ԣ�\r\n");

    //��ʼ�����ýڵ�
    setNodeId(&CanOpen_Data, CanOpenNodeID);
    setState(&CanOpen_Data, Initialisation);
    setState(&CanOpen_Data, Operational);

    TIM5_Init(100 - 1, 900 - 1);        //��ʱ��3��ʼ������ʱ��ʱ��Ϊ90M����Ƶϵ��Ϊ900-1��
                                        //���Զ�ʱ��3��Ƶ��Ϊ90M/900=100K���Զ���װ��Ϊ100-1����ô��ʱ�����ھ���1ms
    //������ʼ����
    xTaskCreate((TaskFunction_t)start_task,  //������
        (const char*)"start_task",           //��������
        (uint16_t)START_STK_SIZE,            //�����ջ��С
        (void*)NULL,                         //���ݸ��������Ĳ���
        (UBaseType_t)START_TASK_PRIO,        //�������ȼ�
        (TaskHandle_t*)&StartTask_Handler);  //������                
    vTaskStartScheduler();                   //�����������  
}

/*******************************************************************************
* @Brief    ��ʼ����������
* @Param    *pvParameters:���񴫵��β�
* @Return   None
* @Date     2019-09-13
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void start_task(void* pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����TASK1����
    xTaskCreate((TaskFunction_t)task1_task,
        (const char*)"task1_task",
        (uint16_t)TASK1_STK_SIZE,
        (void*)NULL,
        (UBaseType_t)TASK1_TASK_PRIO,
        (TaskHandle_t*)&Task1Task_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t)task2_task,
        (const char*)"task2_task",
        (uint16_t)TASK2_STK_SIZE,
        (void*)NULL,
        (UBaseType_t)TASK2_TASK_PRIO,
        (TaskHandle_t*)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

/*******************************************************************************
* @Brief    task1������
* @Param    *pvParameters:���񴫵��β�
* @Return   None
* @Date     2019-09-19
* @Version  V2.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void task1_task(void* pvParameters)
{
    u8 key, res;    //����״̬��������������Ϣ
    u8 i = 0;       //��������ֵ
    u8 canbuf[8];   //������
    u8 mode = 1;    //CANģʽ

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
            printf("\r\n���͵����ݣ�");
            for (i = 0; i < 8; i++)
            {
                printf("%3d ", canbuf[i]);
            }

            //��������
            res = CAN1_Send_Msg(canbuf, 8);

            //��ʾ����״̬
            if (res)
            {
                printf("\r\n����ʧ��\r\n");    //��ʾ����ʧ��
            }
            else
            {
                printf("\r\n���ͳɹ�\r\n");    //��ʾ���ͳɹ�
            }
        }
        else if (key == KEY1_PRES)    //KEY1���£��ı�CAN�Ĺ���ģʽ
        {
            //�л�ģʽ״̬��־λ
            mode = !mode;

            //�л�ģʽ:��ͨģʽ,�ػ�ģʽ
            CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 6, mode ? CAN_MODE_NORMAL : CAN_MODE_LOOPBACK);

            //������ʾģʽ
            if (mode)
            {
                printf("\r\n��ͨģʽ \r\n");
            }
            else
            {
                printf("\r\n�ػ�ģʽ \r\n");
            }
        }

        vTaskDelay(50);     //�ӳ�50ms
    }
}

/*******************************************************************************
* @Brief    task2������
* @Param    *pvParameters:���񴫵��β�
* @Return   None
* @Date     2019-09-17
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void task2_task(void* pvParameters)
{
    u8 t = 0;       //��������ֵ

    while (1)
    {
        //ʱ������ۼ�
        t++;
        if (t == 50)
        {
            LED0 = !LED0;    //��ʾϵͳ��������    
            t = 0;
        }

        vTaskDelay(10);     //�ӳ�10ms
    }
}

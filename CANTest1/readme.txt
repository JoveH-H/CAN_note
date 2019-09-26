STM32F429IGT6简易CAN通讯测试

基于HAL库

硬件接口：
1. LED0 --- PB1
2. LED1 --- PB0
3. KEY0 --- PH3
4. KEY1 --- PH2
5. USART1_TX --- PA9
6. USART1_RX --- PA10
7. CAN_RX --- PA11
8. CAN_TX --- PA12

注意：
1. LCD接口与正点原子阿波罗开发板接口相同
2. 在LCD_Init函数里面(在lcd.c),用到了printf,如果不初始化串口1,将导致液晶无法显示
3. 串口波特率115200
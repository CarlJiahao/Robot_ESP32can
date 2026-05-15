/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  * 文件名             : freertos.c
  * 描述               : FreeRTOS 应用程序代码
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  * 本软件遵循根目录中 LICENSE 文件的条款进行许可。
  * 如果未附带 LICENSE 文件，则按"原样"提供。
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
// 包含头文件
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// 用户自定义包含头文件
#include "PCA9685.h"    // PCA9685 PWM 驱动
#include "ssd1306.h"    // SSD1306 OLED 显示驱动
#include "globals.h"    // 全局变量声明
#include "SR04.h"       // HC-SR04 超声波传感器驱动
#include "mpu6050.h"    // MPU6050 IMU 驱动

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
// 私有类型定义
/* USER CODE BEGIN PTD */
char distance_buffer[20];  // 用来存储格式化后的距离字符串
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
// 私有宏定义
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
// 私有宏
/* USER CODE BEGIN PM */


/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
// 私有变量
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Definitions for defaultTask */
// defaultTask 任务定义
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",              // 任务名称：默认任务
  .stack_size = 128 * 4,              // 栈大小：128字 × 4字节 = 512字节
  .priority = (osPriority_t) osPriorityNormal,  // 优先级：普通优先级（24）
};

/* Definitions for active */
// active_task 任务定义
osThreadId_t activeHandle;
const osThreadAttr_t active_attributes = {
  .name = "active",                   // 任务名称：活动任务
  .stack_size = 128 * 4,              // 栈大小：128字 × 4字节 = 512字节
  .priority = (osPriority_t) osPriorityLow,   // 优先级：低优先级（23）
};

/* Definitions for activeEvent */
// activeEvent 事件标志定义
osEventFlagsId_t activeEventHandle;
const osEventFlagsAttr_t activeEvent_attributes = {
  .name = "activeEvent"               // 事件标志名称：活动事件
};

/* Private function prototypes -----------------------------------------------*/
// 私有函数原型声明
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);   // 默认任务函数声明
void active_task(void *argument);        // 活动任务函数声明

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
// FreeRTOS 初始化函数声明（符合 MISRA C 2004 规则 8.1）

/**
  * @brief  FreeRTOS initialization
  * @brief  FreeRTOS 初始化
  * @param  None
  * @param  无
  * @retval None
  * @retval 无
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  // 用户自定义初始化代码

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  // 添加互斥量...
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  // 添加信号量...
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  // 启动定时器，添加新定时器...
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  // 添加队列...
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  // 创建任务线程
  /* creation of defaultTask */
  // 创建 defaultTask 任务
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of active */
  // 创建 active_task 任务
  activeHandle = osThreadNew(active_task, NULL, &active_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  // 添加其他任务...
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  // 创建事件对象
  /* creation of activeEvent */
  // 创建 activeEvent 事件标志
  activeEventHandle = osEventFlagsNew(&activeEvent_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  // 添加其他事件...
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @brief  实现 defaultTask 任务的函数
  * @param  argument: Not used
  * @param  argument: 未使用
  * @retval None
  * @retval 无
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  // 无限循环
  for(;;) {
    // GyroData gyroDataStruct;
    // 陀螺仪数据结构体变量（已注释）
    display_oled();           // 更新 OLED 显示
    osDelay(10);              // 延时 10ms，让出 CPU 给其他任务
    // advance_bot_init();
    // 机器人前进初始化（已注释）
    // MPU6050_GetAccelData(&gyroDataStruct);
    // 读取 MPU6050 加速度数据（已注释）
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_active_task */
/**
* @brief Function implementing the active thread.
* @brief 实现 active_task 任务的函数
* @param argument: Not used
* @param argument: 未使用
* @retval None
* @retval 无
*/
/* USER CODE END Header_active_task */
void active_task(void *argument)
{
  /* USER CODE BEGIN active_task */
  /* Infinite loop */
  // 无限循环
  for(;;) {
    // 创建结构体变量用于存储陀螺仪数据
    // GyroData gyroDataStruct;
    
    // 等待事件标志（阻塞状态，不占用 CPU）
    // activeEventHandle: 事件标志句柄
    // 0x01: 等待第 0 位标志
    // osFlagsWaitAny: 任意一位匹配即唤醒
    // osWaitForever: 永久等待
    osEventFlagsWait(activeEventHandle, 0x01, osFlagsWaitAny, osWaitForever);
    
    // 收到事件后进入运动控制循环
    while(1) {
      switch(activeTask) {  // 根据 activeTask 全局变量执行不同动作
        case 1:  // 前进模式
          ssd1306_clear_area(60,20,128,40);     // 清除 OLED 指定区域
          ssd1306_set_cursor(60, 35);            // 设置光标位置
          ssd1306_write_string(font6x8, "advance  ");  // 显示 "advance"
          
          if(SR04_GetData() <= 40) {  // 如果超声波检测距离 <= 40cm
            HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);  // 翻转 LED 指示灯
            advance_bot_init();  // 障碍物检测，停止并初始化姿态
          } else {
            advance();  // 无障碍物，执行前进步伐
          }
          break;
          
        case 2:  // 左转模式
          ssd1306_clear_area(60,20,250,40);     // 清除 OLED 指定区域
          ssd1306_set_cursor(60, 35);            // 设置光标位置
          ssd1306_write_string(font6x8, "turn left");  // 显示 "turn left"
          turn_left();  // 执行左转动作
          break;
          
        case 3:  // 右转模式
          ssd1306_clear_area(60,20,250,40);     // 清除 OLED 指定区域
          ssd1306_set_cursor(60, 35);            // 设置光标位置
          ssd1306_write_string(font6x8, "turn right");  // 显示 "turn right"
          turn_right();  // 执行右转动作
          break;
          
        case 4:  // 后退模式
          ssd1306_clear_area(60,20,250,40);     // 清除 OLED 指定区域
          ssd1306_set_cursor(60, 35);            // 设置光标位置
          ssd1306_write_string(font6x8, "back      ");  // 显示 "back"
          back();  // 执行后退动作
          break;
          
        case 5:  // 停止模式
          ssd1306_clear_area(60,20,250,40);     // 清除 OLED 指定区域
          ssd1306_set_cursor(60, 35);            // 设置光标位置
          ssd1306_write_string(font6x8, "stop      ");  // 显示 "stop"
          advance_bot_init();  // 执行停止并初始化姿态
          break;
          
        default:  // 默认情况
          break;
      }
      osDelay(100);  // 每步延时 100ms，控制运动节奏
    }
  }
  /* USER CODE END active_task */
}

/* Private application code --------------------------------------------------*/
// 私有应用代码
/* USER CODE BEGIN Application */


/* USER CODE END Application */


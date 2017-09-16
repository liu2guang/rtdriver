#ifndef __DRV_STEPMOTOR_H_
#define __DRV_STEPMOTOR_H_

#include <rtthread.h>

/* 支持f1系列 */
#if defined(STM32F103xE)
#include "stm32f1xx.h"
#endif

/* 类型定义 */
typedef struct{
    __IO uint8_t  run_state ;  // 电机旋转状态
    __IO uint8_t  dir ;        // 电机旋转方向
    __IO int32_t  step_delay;  // 下个脉冲周期（时间间隔），启动时为加速度
    __IO uint32_t decel_start; // 启动减速位置
    __IO int32_t  decel_val;   // 减速阶段步数
    __IO int32_t  min_delay;   // 最小脉冲周期(最大速度，即匀速段速度)
    __IO int32_t  accel_count; // 加减速阶段计数值
}speed_ramp_data;

/* 转动方向 */
#define RT_STEPMOTOR_CLOCKWISE                     (0)      /* 顺时针转动 */
#define RT_STEPMOTOR_COUNTERCLOCKWISE              (1)      /* 逆时针转动 */

/* 双电机拖动转动方式, 向上时方向 */
#define RT_STEPMOTOR_DRAG_UP_CLOCKWISE  RT_STEPMOTOR_CLOCKWISE

/* 电机运行状态 */
#define RT_STEPMOTOR_MODE_STOP                     (0)      /* 电机停止转动模式 */
#define RT_STEPMOTOR_MODE_ACCEL                    (1)      /* 电机加速转动模式 */
#define RT_STEPMOTOR_MODE_CONSTANT                 (2)      /* 电机匀速转动模式 */
#define RT_STEPMOTOR_MODE_DECEL                    (3)      /* 电机减速转动模式 */

/* 步进电机1相关 */
#ifdef RT_USING_STEPMOTOR1
#define RT_STEPMOTOR1_TIMx                    TIM1                /* 定时器 */
#define RT_STEPMOTOR1_TIMx_IRQn               TIM1_CC_IRQn        /* 输出比较中断号 */
#define RT_STEPMOTOR1_TIMx_IRQHandler         TIM1_CC_IRQHandler  /* 输出比较中断函数 */

/* 该值在输出比较里面没有实际用途, 但是配置的时候必须配置, 请保留 */
#define RT_STEPMOTOR1_TIMx_PERIOD             (0xFFFF)

#define RT_STEPMOTOR1_TIMx_RCC_CLK_ENABLE()   __HAL_RCC_TIM1_CLK_ENABLE()
#define RT_STEPMOTOR1_TIMx_RCC_CLK_DISABLE()  __HAL_RCC_TIM1_CLK_DISABLE()

#define RT_STEPMOTOR1_TIMx_CHANNEL            TIM_CHANNEL_1       /* 步进电机#1 方波信号输出通道 */
#define RT_STEPMOTOR1_TIMx_IT_CCx             TIM_IT_CC1          /*  */
#define RT_STEPMOTOR1_TIMx_FLAG_CCx           TIM_FLAG_CC1        /*  */

#define RT_STEPMOTOR1_PUL_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define RT_STEPMOTOR1_PUL_PORT                GPIOA
#define RT_STEPMOTOR1_PUL_PINx                GPIO_PIN_8

#define RT_STEPMOTOR1_DIR_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define RT_STEPMOTOR1_DIR_PORT                GPIOA
#define RT_STEPMOTOR1_DIR_PINx                GPIO_PIN_10

#define RT_STEPMOTOR1_ENA_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define RT_STEPMOTOR1_ENA_PORT                GPIOA
#define RT_STEPMOTOR1_ENA_PINx                GPIO_PIN_9

/* TB6600升级版驱动器, DIR方向信号引脚悬空为正向旋转 */
#define RT_STEPMOTOR1_DIR_FORWARD()           HAL_GPIO_WritePin(RT_STEPMOTOR1_PUL_PORT, \
                                                RT_STEPMOTOR1_DIR_PINx, GPIO_PIN_RESET)
#define RT_STEPMOTOR1_DIR_REVERSE()           HAL_GPIO_WritePin(RT_STEPMOTOR1_PUL_PORT, \
                                                RT_STEPMOTOR1_DIR_PINx, GPIO_PIN_SET)

/* TB6600升级版驱动器, ENA使能信号引脚悬空为电机使能 */
#define RT_STEPMOTOR1_ENA_ENABLE()            HAL_GPIO_WritePin(RT_STEPMOTOR1_ENA_PORT, \
                                                RT_STEPMOTOR1_ENA_PINx, GPIO_PIN_RESET)
#define RT_STEPMOTOR1_ENA_DISABLE()           HAL_GPIO_WritePin(RT_STEPMOTOR1_ENA_PORT, \
                                                RT_STEPMOTOR1_ENA_PINx, GPIO_PIN_SET)

/* TB6600升级版驱动器细分数与定时器频率 */
#define RT_STEPMOTOR1_SUBDIVIDE                   ( 32)

#define RT_STEPMOTOR1_TIMx_PRESCALER              (  5)    /* 18MHz:    32细分 */
// #define RT_STEPMOTOR1_TIMx_PRESCALER           ( 11)    /* 9MHz:     16细分 */
// #define RT_STEPMOTOR1_TIMx_PRESCALER           ( 23)    /* 4.5MHz:   08细分 */
// #define RT_STEPMOTOR1_TIMx_PRESCALER           ( 47)    /* 2.25MHz:  04细分 */
// #define RT_STEPMOTOR1_TIMx_PRESCALER           ( 95)    /* 1.125MHz: 02细分 */
// #define RT_STEPMOTOR1_TIMx_PRESCALER           (191)    /* 0.5625MHz:01细分 */
#endif

/* 步进电机2相关 */
#ifdef RT_USING_STEPMOTOR2
#define RT_STEPMOTOR2_TIMx                    TIM8                /* 定时器 */
#define RT_STEPMOTOR2_TIMx_IRQn               TIM8_CC_IRQn        /* 输出比较中断号 */
#define RT_STEPMOTOR2_TIMx_IRQHandler         TIM8_CC_IRQHandler  /* 输出比较中断函数 */

/* 该值在输出比较里面没有实际用途, 但是配置的时候必须配置, 请保留 */
#define RT_STEPMOTOR2_TIMx_PERIOD             (0xFFFF)

#define RT_STEPMOTOR2_TIMx_RCC_CLK_ENABLE()   __HAL_RCC_TIM8_CLK_ENABLE()
#define RT_STEPMOTOR2_TIMx_RCC_CLK_DISABLE()  __HAL_RCC_TIM8_CLK_DISABLE()

#define RT_STEPMOTOR2_TIMx_CHANNEL            TIM_CHANNEL_1       /* 步进电机#2 方波信号输出通道 */
#define RT_STEPMOTOR2_TIMx_IT_CCx             TIM_IT_CC1          /*  */
#define RT_STEPMOTOR2_TIMx_FLAG_CCx           TIM_FLAG_CC1        /*  */

#define RT_STEPMOTOR2_PUL_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define RT_STEPMOTOR2_PUL_PORT                GPIOC
#define RT_STEPMOTOR2_PUL_PINx                GPIO_PIN_6

#define RT_STEPMOTOR2_DIR_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define RT_STEPMOTOR2_DIR_PORT                GPIOB
#define RT_STEPMOTOR2_DIR_PINx                GPIO_PIN_14

#define RT_STEPMOTOR2_ENA_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define RT_STEPMOTOR2_ENA_PORT                GPIOB
#define RT_STEPMOTOR2_ENA_PINx                GPIO_PIN_15

/* TB6600升级版驱动器, DIR方向信号引脚悬空为正向旋转 */
#define RT_STEPMOTOR2_DIR_FORWARD()           HAL_GPIO_WritePin(RT_STEPMOTOR2_PUL_PORT, \
                                                RT_STEPMOTOR2_DIR_PINx, GPIO_PIN_RESET)
#define RT_STEPMOTOR2_DIR_REVERSE()           HAL_GPIO_WritePin(RT_STEPMOTOR2_PUL_PORT, \
                                                RT_STEPMOTOR2_DIR_PINx, GPIO_PIN_SET)

/* TB6600升级版驱动器, ENA使能信号引脚悬空为电机使能 */
#define RT_STEPMOTOR2_ENA_ENABLE()            HAL_GPIO_WritePin(RT_STEPMOTOR2_ENA_PORT, \
                                                RT_STEPMOTOR2_ENA_PINx, GPIO_PIN_RESET)
#define RT_STEPMOTOR2_ENA_DISABLE()           HAL_GPIO_WritePin(RT_STEPMOTOR2_ENA_PORT, \
                                                RT_STEPMOTOR2_ENA_PINx, GPIO_PIN_SET)

/* TB6600升级版驱动器细分数与定时器频率 */
#define RT_STEPMOTOR2_SUBDIVIDE                   ( 32)

#define RT_STEPMOTOR2_TIMx_PRESCALER              (  5)    /* 18MHz:    32细分 */
// #define RT_STEPMOTOR2_TIMx_PRESCALER           ( 11)    /* 9MHz:     16细分 */
// #define RT_STEPMOTOR2_TIMx_PRESCALER           ( 23)    /* 4.5MHz:   08细分 */
// #define RT_STEPMOTOR2_TIMx_PRESCALER           ( 47)    /* 2.25MHz:  04细分 */
// #define RT_STEPMOTOR2_TIMx_PRESCALER           ( 95)    /* 1.125MHz: 02细分 */
// #define RT_STEPMOTOR2_TIMx_PRESCALER           (191)    /* 0.5625MHz:01细分 */
#endif

/* 算法数学常数, 目前暂时设计为步进电机1 2使用的细分是一样的 */
#define PI                                         (3.14159)
#define CIRCLE_STEP_NUMBER                         (200)                                                  /* 57步进电机:每圈200步数 */
#define SUBDIVIDE_NUMBER                           RT_STEPMOTOR2_SUBDIVIDE                                /* TB6600加强版步进电机驱动器细分数: 32细分 */
#define T1_FREQ                                    (SystemCoreClock/(RT_STEPMOTOR2_TIMx_PRESCALER+1))     /* Ft */
#define SPR                                        (CIRCLE_STEP_NUMBER*SUBDIVIDE_NUMBER)                  /* 57步进电机:每圈6400脉冲数 */

#define ALPHA                                      ((float) (2*PI/SPR) )                                   /* α= 2*pi/spr */
#define A_T_x10                                    ((float) (10*ALPHA*T1_FREQ) )                           /* α*Ft */
#define T1_FREQ_148                                ((float) ((T1_FREQ*0.676)/10) )                         /* 0.676为误差修正值 */
#define A_SQ                                       ((float) (2*100000*ALPHA) )                             /*  */
#define A_x200                                     ((float) (200*ALPHA) )                                  /* 200倍α */

/* 导出变量 */

/* 导出API */

/* 步进电机初始化 */
int rt_hw_stepmotor_init(void);

/* 步进电机转动相对距离, step转动步数(正数顺时针), accel加速度, decel减速度, speed期望速度 */
rt_err_t rt_stepmotor1_move_relative(rt_int32_t step, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed);
rt_err_t rt_stepmotor2_move_relative(rt_int32_t step, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed);

/* 步进电机永久转动, 需要改变速度先停止电机, 再重新调用该函数 */
rt_err_t rt_stepmotor1_move_forever(rt_uint8_t dir, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed);
rt_err_t rt_stepmotor2_move_forever(rt_uint8_t dir, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed);

/* 0:停止, 1:运动 */
rt_uint8_t rt_stepmotor1_run_status(void);
rt_uint8_t rt_stepmotor2_run_status(void);

/* 获取当前电机位置 */
rt_int32_t rt_stepmotor1_current_pos(void);
rt_int32_t rt_stepmotor2_current_pos(void);

/* 复位当前电机位置 */
rt_int32_t rt_stepmotor1_reset_pos_var(void);
rt_int32_t rt_stepmotor2_reset_pos_var(void);

/* 停止电机转动 */
rt_err_t rt_stepmotor1_stop(void);
rt_err_t rt_stepmotor2_stop(void);

/* 双电机拖动方式向上和下, 要求向上时双电机上下面的电机是逆时针 */
rt_err_t rt_stepmotor_drag_up(rt_int32_t step, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed);
rt_err_t rt_stepmotor_drag_down(rt_int32_t step, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed);

/* 双电机向上下拖动, 永久 */
rt_err_t rt_stepmotor_drag_up_forever(rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed);
rt_err_t rt_stepmotor_drag_down_forever(rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed);

#endif

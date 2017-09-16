#include "drv_stepmotor.h"

/* finsh and msh组件 */
#ifdef RT_USING_FINSH
#include "finsh.h"
#endif

/* 操作系统硬件相关 */
#include "rthw.h"

/* 数学库 */
#include <math.h>

/* 电机1相关参数 */
#ifdef RT_USING_STEPMOTOR1
static TIM_HandleTypeDef htimx_stepmotor1;
static speed_ramp_data   srd1 = {RT_STEPMOTOR_MODE_STOP, RT_STEPMOTOR_CLOCKWISE, 0, 0, 0, 0, 0};
static __IO rt_uint8_t   stepmotor1_motion_status = 0; /* 0:停止, 1:运动 */
static __IO rt_int32_t   stepmotor1_position      = 0; /* 当前位置 */
#endif

/* 电机2相关参数 */
#ifdef RT_USING_STEPMOTOR2
static TIM_HandleTypeDef htimx_stepmotor2;
static speed_ramp_data   srd2 = {RT_STEPMOTOR_MODE_STOP, RT_STEPMOTOR_CLOCKWISE, 0, 0, 0, 0, 0};
static __IO rt_uint8_t   stepmotor2_motion_status = 0; /* 0:停止, 1:运动 */
static __IO rt_int32_t   stepmotor2_position      = 0; /* 当前位置 */
#endif

#ifdef RT_USING_STEPMOTOR1
static void rt_stepmotor1_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 开启PUL DIR ENA引脚时钟 */
    RT_STEPMOTOR1_PUL_CLK_ENABLE();
    RT_STEPMOTOR1_DIR_CLK_ENABLE();
    RT_STEPMOTOR1_ENA_CLK_ENABLE();

    /* PUL信号引脚 */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin       = RT_STEPMOTOR1_PUL_PINx;
    HAL_GPIO_Init(RT_STEPMOTOR1_PUL_PORT, &GPIO_InitStruct);

    /* DIR信号引脚 */
    //RT_STEPMOTOR1_DIR_FORWARD();   /* 引脚初始化之前先配置为正向旋转 */

    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin       = RT_STEPMOTOR1_DIR_PINx;
    HAL_GPIO_Init(RT_STEPMOTOR1_DIR_PORT, &GPIO_InitStruct);

    /* ENA信号引脚 */
    //RT_STEPMOTOR1_ENA_DISABLE();   /* 引脚初始化之前先失能步进电机 */

    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin       = RT_STEPMOTOR1_ENA_PINx;
    HAL_GPIO_Init(RT_STEPMOTOR1_ENA_PORT, &GPIO_InitStruct);
}
#endif

#ifdef RT_USING_STEPMOTOR2
static void rt_stepmotor2_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 开启PUL DIR ENA引脚时钟 */
    RT_STEPMOTOR2_PUL_CLK_ENABLE();
    RT_STEPMOTOR2_DIR_CLK_ENABLE();
    RT_STEPMOTOR2_ENA_CLK_ENABLE();

    /* PUL信号引脚 */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin       = RT_STEPMOTOR2_PUL_PINx;
    HAL_GPIO_Init(RT_STEPMOTOR2_PUL_PORT, &GPIO_InitStruct);

    /* DIR信号引脚 */
    //RT_STEPMOTOR2_DIR_FORWARD();   /* 引脚初始化之前先配置为正向旋转 */

    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin       = RT_STEPMOTOR2_DIR_PINx;
    HAL_GPIO_Init(RT_STEPMOTOR2_DIR_PORT, &GPIO_InitStruct);

    /* ENA信号引脚 */
    //RT_STEPMOTOR2_ENA_DISABLE();   /* 引脚初始化之前先失能步进电机 */

    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin       = RT_STEPMOTOR2_ENA_PINx;
    HAL_GPIO_Init(RT_STEPMOTOR2_ENA_PORT, &GPIO_InitStruct);
}
#endif

#ifdef RT_USING_STEPMOTOR1
static void rt_stepmotor1_timx_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_OC_InitTypeDef sConfigOC;

    /* 使能定时器时钟 */
    RT_STEPMOTOR1_TIMx_RCC_CLK_ENABLE();

    /* 配置定时器计数器 */
    htimx_stepmotor1.Instance = RT_STEPMOTOR1_TIMx;

    htimx_stepmotor1.Init.Prescaler     = RT_STEPMOTOR1_TIMx_PRESCALER;        /* 定时器预分频 */
    htimx_stepmotor1.Init.CounterMode   = TIM_COUNTERMODE_UP;                  /* 向上计数模式 */
    htimx_stepmotor1.Init.Period        = RT_STEPMOTOR1_TIMx_PERIOD;           /* 定时器周期 */
    htimx_stepmotor1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;              /* 时钟分频 */
    HAL_TIM_Base_Init(&htimx_stepmotor1);

    /* 配置定时器时钟源 */
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;      /* 内部时钟源 */
    HAL_TIM_ConfigClockSource(&htimx_stepmotor1, &sClockSourceConfig);

    /* 配置比较输出 */
    sConfigOC.OCMode       = TIM_OCMODE_TOGGLE;                     /* 比较输出模式:匹配翻转电平 */
    sConfigOC.Pulse        = 0xFFFF;                                /* 脉冲数 */
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_LOW;                    /* 输出极性 */
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_LOW;                   /* 互补通道输出极性 */
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;                    /* 快速模式 */
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;                 /* 空闲电平 */
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;                /* 互补通道空闲电平 */
    HAL_TIM_OC_ConfigChannel(&htimx_stepmotor1, &sConfigOC, RT_STEPMOTOR1_TIMx_CHANNEL);

    /* 失能比较输出通道 */
    TIM_CCxChannelCmd(RT_STEPMOTOR1_TIMx, RT_STEPMOTOR1_TIMx_CHANNEL, TIM_CCx_DISABLE);

    /* 配置定时器中断优先级并使能 */
    HAL_NVIC_SetPriority(RT_STEPMOTOR1_TIMx_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RT_STEPMOTOR1_TIMx_IRQn);

    __HAL_TIM_CLEAR_FLAG(&htimx_stepmotor1, RT_STEPMOTOR1_TIMx_FLAG_CCx);

    /* 使能定时器比较输出 */
    __HAL_TIM_ENABLE_IT(&htimx_stepmotor1, RT_STEPMOTOR1_TIMx_IT_CCx);
    
    /* Enable the main output */
    __HAL_TIM_MOE_ENABLE(&htimx_stepmotor1);

    HAL_TIM_Base_Start(&htimx_stepmotor1);
}
#endif

#ifdef RT_USING_STEPMOTOR2
static void rt_stepmotor2_timx_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_OC_InitTypeDef sConfigOC;

    /* 使能定时器时钟 */
    RT_STEPMOTOR2_TIMx_RCC_CLK_ENABLE();

    /* 配置定时器计数器 */
    htimx_stepmotor2.Instance = RT_STEPMOTOR2_TIMx;

    htimx_stepmotor2.Init.Prescaler     = RT_STEPMOTOR2_TIMx_PRESCALER;        /* 定时器预分频 */
    htimx_stepmotor2.Init.CounterMode   = TIM_COUNTERMODE_UP;                  /* 向上计数模式 */
    htimx_stepmotor2.Init.Period        = RT_STEPMOTOR2_TIMx_PERIOD;           /* 定时器周期 */
    htimx_stepmotor2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;              /* 时钟分频 */
    HAL_TIM_Base_Init(&htimx_stepmotor2);

    /* 配置定时器时钟源 */
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;      /* 内部时钟源 */
    HAL_TIM_ConfigClockSource(&htimx_stepmotor2, &sClockSourceConfig);

    /* 配置比较输出 */
    sConfigOC.OCMode       = TIM_OCMODE_TOGGLE;                     /* 比较输出模式:匹配翻转电平 */
    sConfigOC.Pulse        = 0xFFFF;                                /* 脉冲数 */
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_LOW;                    /* 输出极性 */
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_LOW;                   /* 互补通道输出极性 */
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;                    /* 快速模式 */
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;                 /* 空闲电平 */
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;                /* 互补通道空闲电平 */
    HAL_TIM_OC_ConfigChannel(&htimx_stepmotor2, &sConfigOC, RT_STEPMOTOR2_TIMx_CHANNEL);

    /* 失能比较输出通道 */
    TIM_CCxChannelCmd(RT_STEPMOTOR2_TIMx, RT_STEPMOTOR2_TIMx_CHANNEL, TIM_CCx_DISABLE);

    /* 配置定时器中断优先级并使能 */
    HAL_NVIC_SetPriority(RT_STEPMOTOR2_TIMx_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(RT_STEPMOTOR2_TIMx_IRQn);

    __HAL_TIM_CLEAR_FLAG(&htimx_stepmotor2, RT_STEPMOTOR2_TIMx_FLAG_CCx);

    /* 使能定时器比较输出 */
    __HAL_TIM_ENABLE_IT(&htimx_stepmotor2, RT_STEPMOTOR2_TIMx_IT_CCx);
    
    /* Enable the main output */
    __HAL_TIM_MOE_ENABLE(&htimx_stepmotor2);

    HAL_TIM_Base_Start(&htimx_stepmotor2);
}
#endif

/* 步进电机1转动相对距离, step转动步数(正数顺时针), accel加速度, decel减速度, speed期望速度 */
#ifdef RT_USING_STEPMOTOR1
rt_err_t rt_stepmotor1_move_relative(
    __IO rt_int32_t step, 
    __IO rt_uint32_t accel,
    __IO rt_uint32_t decel,
    __IO rt_uint32_t speed)
{
    __IO rt_uint16_t tim_count;

    __IO rt_uint32_t max_s_lim;     /* 达到期望速度的步数 */
    __IO rt_uint32_t accel_lim;     /* 必须开始减速的步数 */
    
    /* 步进电机转动方向处理 */
    if(step < 0)                    /* 逆时针转动 */
    {
        /* 记录当前逆时针转动 */
        srd1.dir = RT_STEPMOTOR_COUNTERCLOCKWISE;
        RT_STEPMOTOR1_DIR_REVERSE();

        /* 取步数绝对值 */
        step = (-step); 
    }
    else if(step > 0)               /* 顺时针转动 */
    {
        /* 记录当前顺时针转动 */
        srd1.dir = RT_STEPMOTOR_CLOCKWISE;
        RT_STEPMOTOR1_DIR_FORWARD();
    }

    /* 步进电机转动步数处理 */

    /* 步数为0时 */
    if(step == 0)
    {
        return RT_EOK;
    }

    /* 步数为1时 */
    else if(step == 1)
    {
        srd1.accel_count = (-1);                     /* 只移动一步 */
        srd1.run_state   = RT_STEPMOTOR_MODE_DECEL;  /* 减速状态 */
        srd1.step_delay  = (1000);                   /* 短延时 */
    }

    /* 步数大于1 */
    else
    {
        /* 根据期望速度计算出定时器计数器匹配值: = (alpha / Tt) / w = (alpha * Ft) / w */
        srd1.min_delay = (rt_int32_t)(A_T_x10/speed);

        /* C0第0个脉冲定时器计数值:
           = 1/Tt * sqrt(2*alpha/accel) = Ft * sqrt(2*alpha/accel) 
           = ( Ft * 0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 */
        srd1.step_delay = (rt_int32_t)(T1_FREQ_148 * sqrt(A_SQ/accel)/10);

        /* 计算多少步后, 达到期望速度 = speed^2 / (2*alpha*accel) */
        max_s_lim = (rt_uint32_t)(speed*speed/(A_x200*accel/10));

        /* 如果速度小于0.5步后, 由于上面的格式会四舍五入为0, 但是实际又必须移动1步才能达到期望速度 */
        if(max_s_lim == 0)
        {
            max_s_lim = 1;
        }

        /* 计算多少步后, 步进电机必须减速 = (n1+n2)decel / (accel + decel) */
        accel_lim = (rt_uint32_t)(step*decel/(accel+decel));

        /* 至少加速1布才能减速 */
        if(accel_lim == 0)
        {
            accel_lim = 1;
        }

        /* 计算减速阶段需要的步数, 步数为负数值 */
        
        /* 达到步进电机加速达到期望速度的情况 */
        if(accel_lim > max_s_lim)
        {
            srd1.decel_val = (-(max_s_lim*accel/decel));
        }

        /* 达到步进电机加速未达到期望速度的情况 */
        else
        {
            srd1.decel_val = accel_lim - step;
        }

        /*  */
        if(srd1.decel_val == 0)
        {
            srd1.decel_val = (-1);
        }

        /* 计算开始减速的步数, srd1.decel_val为负数, 所以这里是step + srd1.decel_val */
        srd1.decel_start = step + srd1.decel_val;

        /* 当期望速度很低时, 就不启动梯形加减速算法 */
        if(srd1.step_delay <= srd1.min_delay)
        {
            srd1.step_delay = srd1.min_delay;
            srd1.run_state  = RT_STEPMOTOR_MODE_CONSTANT;
        }
        else
        {
            srd1.run_state  = RT_STEPMOTOR_MODE_ACCEL;
        }
    }

    /* 电机为运动状态 */
    stepmotor1_motion_status = 1; 

    tim_count = __HAL_TIM_GET_COUNTER(&htimx_stepmotor1);

    /* 设置定时器比较值 */
    __HAL_TIM_SET_COMPARE(&htimx_stepmotor1, RT_STEPMOTOR1_TIMx_CHANNEL, tim_count+srd1.step_delay);

    /* 使能定时器通道 */
    TIM_CCxChannelCmd(RT_STEPMOTOR1_TIMx, RT_STEPMOTOR1_TIMx_CHANNEL, TIM_CCx_ENABLE);

    /* TB6600驱动器工作与正常模式 */
    RT_STEPMOTOR1_ENA_ENABLE();

    return RT_EOK;
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_stepmotor1_move_relative, sm1, stepmotor1 axismove relative);
#endif

/* 步进电机2转动相对距离, step转动步数(正数顺时针), accel加速度, decel减速度, speed期望速度 */
#ifdef RT_USING_STEPMOTOR2
rt_err_t rt_stepmotor2_move_relative(
    __IO rt_int32_t step, 
    __IO rt_uint32_t accel,
    __IO rt_uint32_t decel,
    __IO rt_uint32_t speed)
{
    __IO rt_uint16_t tim_count;

    __IO rt_uint32_t max_s_lim;     /* 达到期望速度的步数 */
    __IO rt_uint32_t accel_lim;     /* 必须开始减速的步数 */
    
    /* 步进电机转动方向处理 */
    if(step < 0)                    /* 逆时针转动 */
    {
        /* 记录当前逆时针转动 */
        srd2.dir = RT_STEPMOTOR_COUNTERCLOCKWISE;
        RT_STEPMOTOR2_DIR_REVERSE();

        /* 取步数绝对值 */
        step = (-step); 
    }
    else if(step > 0)               /* 顺时针转动 */
    {
        /* 记录当前顺时针转动 */
        srd2.dir = RT_STEPMOTOR_CLOCKWISE;
        RT_STEPMOTOR2_DIR_FORWARD();
    }

    /* 步进电机转动步数处理 */

    /* 步数为0时 */
    if(step == 0)
    {
        return RT_EOK;
    }

    /* 步数为1时 */
    else if(step == 1)
    {
        srd2.accel_count = (-1);                     /* 只移动一步 */
        srd2.run_state   = RT_STEPMOTOR_MODE_DECEL;  /* 减速状态 */
        srd2.step_delay  = (1000);                   /* 短延时 */
    }

    /* 步数大于1 */
    else
    {
        /* 根据期望速度计算出定时器计数器匹配值: = (alpha / Tt) / w = (alpha * Ft) / w */
        srd2.min_delay = (rt_int32_t)(A_T_x10/speed);

        /* C0第0个脉冲定时器计数值:
           = 1/Tt * sqrt(2*alpha/accel) = Ft * sqrt(2*alpha/accel) 
           = ( Ft * 0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 */
        srd2.step_delay = (rt_int32_t)(T1_FREQ_148 * sqrt(A_SQ/accel)/10);

        /* 计算多少步后, 达到期望速度 = speed^2 / (2*alpha*accel) */
        max_s_lim = (rt_uint32_t)(speed*speed/(A_x200*accel/10));

        /* 如果速度小于0.5步后, 由于上面的格式会四舍五入为0, 但是实际又必须移动1步才能达到期望速度 */
        if(max_s_lim == 0)
        {
            max_s_lim = 1;
        }

        /* 计算多少步后, 步进电机必须减速 = (n1+n2)decel / (accel + decel) */
        accel_lim = (rt_uint32_t)(step*decel/(accel+decel));

        /* 至少加速1布才能减速 */
        if(accel_lim == 0)
        {
            accel_lim = 1;
        }

        /* 计算减速阶段需要的步数, 步数为负数值 */
        
        /* 达到步进电机加速达到期望速度的情况 */
        if(accel_lim > max_s_lim)
        {
            srd2.decel_val = (-(max_s_lim*accel/decel));
        }

        /* 达到步进电机加速未达到期望速度的情况 */
        else
        {
            srd2.decel_val = accel_lim - step;
        }

        /*  */
        if(srd2.decel_val == 0)
        {
            srd2.decel_val = (-1);
        }

        /* 计算开始减速的步数, srd1.decel_val为负数, 所以这里是step + srd1.decel_val */
        srd2.decel_start = step + srd2.decel_val;

        /* 当期望速度很低时, 就不启动梯形加减速算法 */
        if(srd2.step_delay <= srd2.min_delay)
        {
            srd2.step_delay = srd2.min_delay;
            srd2.run_state  = RT_STEPMOTOR_MODE_CONSTANT;
        }
        else
        {
            srd2.run_state  = RT_STEPMOTOR_MODE_ACCEL;
        }
    }

    /* 电机为运动状态 */
    stepmotor2_motion_status = 1; 

    tim_count = __HAL_TIM_GET_COUNTER(&htimx_stepmotor2);

    /* 设置定时器比较值 */
    __HAL_TIM_SET_COMPARE(&htimx_stepmotor2, RT_STEPMOTOR2_TIMx_CHANNEL, tim_count+srd2.step_delay);

    /* 使能定时器通道 */
    TIM_CCxChannelCmd(RT_STEPMOTOR2_TIMx, RT_STEPMOTOR2_TIMx_CHANNEL, TIM_CCx_ENABLE);

    /* TB6600驱动器工作与正常模式 */
    RT_STEPMOTOR2_ENA_ENABLE();

    return RT_EOK;
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_stepmotor2_move_relative, sm2, stepmotor2 axismove relative);
#endif

/* 步进电机永久转动, 需要改变速度先停止电机, 再重新调用该函数 */
rt_err_t rt_stepmotor1_move_forever(rt_uint8_t dir, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed)
{
    if(dir == RT_STEPMOTOR_CLOCKWISE)
    {
        return rt_stepmotor1_move_relative(-2147483648L, accel, decel, speed);
    }
    else
    {
        return rt_stepmotor1_move_relative(2147483647L, accel, decel, speed);
    }

    return RT_EOK;
}

/* 步进电机永久转动, 需要变速度先停止电机, 再重新调用该函数 */
rt_err_t rt_stepmotor2_move_forever(rt_uint8_t dir, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed)
{
    if(dir == RT_STEPMOTOR_CLOCKWISE)
    {
        return rt_stepmotor2_move_relative(-2147483648L, accel, decel, speed);
    }
    else
    {
        return rt_stepmotor2_move_relative(2147483647L, accel, decel, speed);
    }

    return RT_EOK;
}

/* 0:停止, 1:运动 */
rt_uint8_t rt_stepmotor1_run_status(void)
{
    return stepmotor1_motion_status;
}

/* 0:停止, 1:运动 */
rt_uint8_t rt_stepmotor2_run_status(void)
{
    return stepmotor2_motion_status;
}

/* 获取当前电机1位置 */
rt_int32_t rt_stepmotor1_current_pos(void)
{
    return stepmotor1_position;
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_stepmotor1_current_pos, cpos1, print current stepmotor1 pos);

/* 获取当前电机2位置 */
rt_int32_t rt_stepmotor2_current_pos(void)
{
    return stepmotor2_position;
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_stepmotor2_current_pos, cpos2, print current stepmotor2 pos);

/* 复位当前电机1位置变量 */
rt_int32_t rt_stepmotor1_reset_pos_var(void)
{
    return stepmotor1_position;
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_stepmotor1_reset_pos_var, rpv1, reset current stepmotor1 pos var);

/* 复位当前电机2位置 */
rt_int32_t rt_stepmotor2_reset_pos_var(void)
{
    return stepmotor2_position;
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_stepmotor2_reset_pos_var, rpv2, reset current stepmotor2 pos var);

/* 双电机拖动方式向上, 要求向上时双电机上下面的电机是逆时针 */
rt_err_t rt_stepmotor_drag_up(rt_int32_t step, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed)
{
    /* 将步数转变方向 */
#if (RT_STEPMOTOR_DRAG_UP_CLOCKWISE == RT_STEPMOTOR_CLOCKWISE)
    if(step > 0)
#elif
    if(step < 0)
#endif
    {
        step = -step;
    }

    /* 步进电机1相对位置移动 */
    rt_stepmotor1_move_relative(step, accel, decel, speed);

    /* 步进电机2脱机, 这里采用的是上面电机转动, 下面电机脱机, 靠上面电机带动下面电机转动 */
    RT_STEPMOTOR2_ENA_DISABLE();

    /* 等待转动完成, 完成后需要将下面的电机上机, 保证广告系统不会受外力而导致广告布转动 */
    while(rt_stepmotor1_run_status() == 1)
    {
        rt_thread_delay(10);
    }

    /* 步进电机2上机 */
    RT_STEPMOTOR2_ENA_ENABLE();
    
    return RT_EOK;
}

/* 双电机拖动方式向下, 要求向上时双电机上下面的电机是逆时针 */
rt_err_t rt_stepmotor_drag_down(rt_int32_t step, rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed)
{
    /* 将步数转变方向 */
#if (RT_STEPMOTOR_DRAG_UP_CLOCKWISE == RT_STEPMOTOR_CLOCKWISE)
    if(step > 0)
#elif
    if(step < 0)
#endif
    {
        step = -step;
    }

    /* 步进电机2相对位置移动 */
    rt_stepmotor2_move_relative(step, accel, decel, speed);

    /* 步进电机1脱机, 这里采用的是上面电机转动, 下面电机脱机, 靠上面电机带动下面电机转动 */
    RT_STEPMOTOR1_ENA_DISABLE();

    /* 等待转动完成, 完成后需要将下面的电机上机, 保证广告系统不会受外力而导致广告布转动 */
    while(rt_stepmotor2_run_status() == 1)
    {
        rt_thread_delay(10);
    }

    /* 步进电机2上机 */
    RT_STEPMOTOR1_ENA_ENABLE();
    
    return RT_EOK;
}

/* 双电机向上拖动, 永久 */
rt_err_t rt_stepmotor_drag_up_forever(rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed)
{
    rt_stepmotor1_move_forever(RT_STEPMOTOR_DRAG_UP_CLOCKWISE, accel, decel, speed);

    /* 步进电机2脱机 */
    RT_STEPMOTOR2_ENA_DISABLE();
    
    return RT_EOK;
}

/* 双电机向下拖动, 永久 */
rt_err_t rt_stepmotor_drag_down_forever(rt_uint32_t accel, rt_uint32_t decel, rt_uint32_t speed)
{
    rt_stepmotor2_move_forever(RT_STEPMOTOR_DRAG_UP_CLOCKWISE, accel, decel, speed);

    /* 步进电机2脱机 */
    RT_STEPMOTOR1_ENA_DISABLE();
    
    return RT_EOK;
}

/* 停止电机1转动, 这里电机停止转动和脱机不一样 */
rt_err_t rt_stepmotor1_stop(void)
{
    /* 修改控制句柄 */
    srd1.run_state   = RT_STEPMOTOR_MODE_STOP;
    srd1.dir         = RT_STEPMOTOR_CLOCKWISE;
    srd1.step_delay  = 0;
    srd1.decel_start = 0;
    srd1.decel_val   = 0;
    srd1.min_delay   = 0;
    srd1.accel_count = 0;

    stepmotor1_motion_status = 0;

    return RT_EOK;
}

/* 停止电机2转动, 这里电机停止转动和脱机不一样 */
rt_err_t rt_stepmotor2_stop(void)
{
    /* 修改控制句柄 */
    srd2.run_state   = RT_STEPMOTOR_MODE_STOP;
    srd2.dir         = RT_STEPMOTOR_CLOCKWISE;
    srd2.step_delay  = 0;
    srd2.decel_start = 0;
    srd2.decel_val   = 0;
    srd2.min_delay   = 0;
    srd2.accel_count = 0;

    stepmotor2_motion_status = 0;

    return RT_EOK;
}

/* 比较输出中断 */
#ifdef RT_USING_STEPMOTOR1
void RT_STEPMOTOR1_TIMx_IRQHandler(void)
{
    /* 关闭中断 */
    rt_base_t interrupt = rt_hw_interrupt_disable();

    HAL_TIM_IRQHandler(&htimx_stepmotor1);

    /* 开启中断 */
    rt_hw_interrupt_enable(interrupt);
}
#endif

/* 比较输出中断 */
#ifdef RT_USING_STEPMOTOR2
void RT_STEPMOTOR2_TIMx_IRQHandler(void)
{
    /* 关闭中断 */
    rt_base_t interrupt = rt_hw_interrupt_disable();

    HAL_TIM_IRQHandler(&htimx_stepmotor2);

    /* 开启中断 */
    rt_hw_interrupt_enable(interrupt);
}
#endif

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
#ifdef RT_USING_STEPMOTOR1
    if(htim->Instance == RT_STEPMOTOR1_TIMx)
    {
        __IO rt_uint16_t tim_count = 0;
        rt_uint16_t new_step_delay = 0;                 /* 保存下一个延时周期 */

        __IO static rt_uint16_t last_accel_delay = 0;   /* 加速过程中最后一次延时(脉冲周期) */
        __IO static rt_uint32_t step_count       = 0;   /* 总移动步数计数器 */
        __IO static rt_int32_t rest              = 0;   /* 记录new_step_delay中的余数，提高下一步计算的精度 */
        __IO static rt_uint8_t i                 = 0;   /* 定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲 */

        /* 设置比较值 */
        tim_count = __HAL_TIM_GET_COUNTER(&htimx_stepmotor1);
        __HAL_TIM_SET_COMPARE(&htimx_stepmotor1, RT_STEPMOTOR1_TIMx_CHANNEL, tim_count+srd1.step_delay);

        /* 2次为一个完整方波信号(脉冲) */
        if((++i) == 2)
        {
            i = 0;

            switch(srd1.run_state)
            {
                /* 停止阶段  */
                case RT_STEPMOTOR_MODE_STOP:
                {
                    step_count = 0;
                    rest       = 0;

                    /* 关闭通道 */
                    TIM_CCxChannelCmd(RT_STEPMOTOR1_TIMx, RT_STEPMOTOR1_TIMx_CHANNEL, TIM_CCx_DISABLE);
                    __HAL_TIM_CLEAR_FLAG(&htimx_stepmotor1, RT_STEPMOTOR1_TIMx_FLAG_CCx);

                    //RT_STEPMOTOR1_ENA_DISABLE();

                    /* 电机为运动状态 */
                    stepmotor1_motion_status = 0; 
                }
                break;

                /* 加速阶段 */
                case RT_STEPMOTOR_MODE_ACCEL:
                {
                    step_count++;

                    if(srd1.dir == RT_STEPMOTOR_CLOCKWISE)
                    {
                        stepmotor1_position++;
                    }
                    else
                    {
                        stepmotor1_position--;
                    }

                    srd1.accel_count++;

                    /* 计算下一步脉冲周期(时间间隔) */
                    new_step_delay = srd1.step_delay - (((2 *srd1.step_delay) + rest)/(4 * srd1.accel_count + 1));

                    /* 计算余数，下次计算补上余数，减少误差 */
                    rest = ((2 * srd1.step_delay)+rest)%(4 * srd1.accel_count + 1);

                    /* 检查是够应该开始减速 */
                    if(step_count >= srd1.decel_start)
                    {
                        srd1.accel_count = srd1.decel_val;
                        srd1.run_state   = RT_STEPMOTOR_MODE_DECEL;
                    }

                    /* 检查是否到达期望的最大速度 */
                    else if(new_step_delay <= srd1.min_delay)
                    {
                        last_accel_delay = new_step_delay;
                        new_step_delay   = srd1.min_delay;
                        rest             = 0;
                        srd1.run_state    = RT_STEPMOTOR_MODE_CONSTANT;
                    }
                }
                break;

                /* 匀速阶段 */
                case RT_STEPMOTOR_MODE_CONSTANT:
                {
                    step_count++;

                    if(srd1.dir == RT_STEPMOTOR_CLOCKWISE)
                    {
                        stepmotor1_position++;
                    }
                    else
                    {
                        stepmotor1_position--;
                    }

                    new_step_delay = srd1.min_delay;

                    /* 需要开始减速 */
                    if(step_count >= srd1.decel_start)
                    {
                        srd1.accel_count = srd1.decel_val;            /* 减速步数做为加速计数值 */
                        new_step_delay  = last_accel_delay;         /* 加阶段最后的延时做为减速阶段的起始延时(脉冲周期) */
                        srd1.run_state   = RT_STEPMOTOR_MODE_DECEL;  /* 状态改变为减速 */
                    }
                }
                break;

                /* 减速阶段 */
                case RT_STEPMOTOR_MODE_DECEL:
                {
                    step_count++;

                    if(srd1.dir == RT_STEPMOTOR_CLOCKWISE)
                    {
                        stepmotor1_position++;
                    }
                    else
                    {
                        stepmotor1_position--;
                    }

                    srd1.accel_count++;

                    /* 计算下一步脉冲周期(时间间隔) */
                    new_step_delay = srd1.step_delay - (((2 * srd1.step_delay) + rest)/(4 * srd1.accel_count + 1));

                    /* 计算余数，下次计算补上余数，减少误差 */
                    rest = ((2 * srd1.step_delay)+rest)%(4 * srd1.accel_count + 1);

                    /* 检查是否为最后一步 */
                    if(srd1.accel_count >= 0)
                    {
                        srd1.run_state = RT_STEPMOTOR_MODE_STOP;
                    }
                }
                break;
            }

            /* 为下个延时(脉冲周期)赋值 */
            srd1.step_delay = new_step_delay;
        }
    }
#endif

#ifdef RT_USING_STEPMOTOR2
    if(htim->Instance == RT_STEPMOTOR2_TIMx)
    {
        __IO rt_uint16_t tim_count = 0;
        rt_uint16_t new_step_delay = 0;                 /* 保存下一个延时周期 */

        __IO static rt_uint16_t last_accel_delay = 0;   /* 加速过程中最后一次延时(脉冲周期) */
        __IO static rt_uint32_t step_count       = 0;   /* 总移动步数计数器 */
        __IO static rt_int32_t rest              = 0;   /* 记录new_step_delay中的余数，提高下一步计算的精度 */
        __IO static rt_uint8_t i                 = 0;   /* 定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲 */

        /* 设置比较值 */
        tim_count = __HAL_TIM_GET_COUNTER(&htimx_stepmotor2);
        __HAL_TIM_SET_COMPARE(&htimx_stepmotor2, RT_STEPMOTOR2_TIMx_CHANNEL, tim_count+srd2.step_delay);

        /* 2次为一个完整方波信号(脉冲) */
        if((++i) == 2)
        {
            i = 0;

            switch(srd2.run_state)
            {
                /* 停止阶段  */
                case RT_STEPMOTOR_MODE_STOP:
                {
                    step_count = 0;
                    rest       = 0;

                    /* 关闭通道 */
                    TIM_CCxChannelCmd(RT_STEPMOTOR2_TIMx, RT_STEPMOTOR2_TIMx_CHANNEL, TIM_CCx_DISABLE);
                    __HAL_TIM_CLEAR_FLAG(&htimx_stepmotor2, RT_STEPMOTOR2_TIMx_FLAG_CCx);

                    //RT_STEPMOTOR2_ENA_DISABLE();

                    /* 电机为运动状态 */
                    stepmotor2_motion_status = 0; 
                }
                break;

                /* 加速阶段 */
                case RT_STEPMOTOR_MODE_ACCEL:
                {
                    step_count++;

                    if(srd2.dir == RT_STEPMOTOR_CLOCKWISE)
                    {
                        stepmotor2_position++;
                    }
                    else
                    {
                        stepmotor2_position--;
                    }

                    srd2.accel_count++;

                    /* 计算下一步脉冲周期(时间间隔) */
                    new_step_delay = srd2.step_delay - (((2 *srd2.step_delay) + rest)/(4 * srd2.accel_count + 1));

                    /* 计算余数，下次计算补上余数，减少误差 */
                    rest = ((2 * srd2.step_delay)+rest)%(4 * srd2.accel_count + 1);

                    /* 检查是够应该开始减速 */
                    if(step_count >= srd2.decel_start)
                    {
                        srd2.accel_count = srd2.decel_val;
                        srd2.run_state   = RT_STEPMOTOR_MODE_DECEL;
                    }

                    /* 检查是否到达期望的最大速度 */
                    else if(new_step_delay <= srd2.min_delay)
                    {
                        last_accel_delay = new_step_delay;
                        new_step_delay   = srd2.min_delay;
                        rest             = 0;
                        srd2.run_state    = RT_STEPMOTOR_MODE_CONSTANT;
                    }
                }
                break;

                /* 匀速阶段 */
                case RT_STEPMOTOR_MODE_CONSTANT:
                {
                    step_count++;

                    if(srd2.dir == RT_STEPMOTOR_CLOCKWISE)
                    {
                        stepmotor2_position++;
                    }
                    else
                    {
                        stepmotor2_position--;
                    }

                    new_step_delay = srd2.min_delay;

                    /* 需要开始减速 */
                    if(step_count >= srd2.decel_start)
                    {
                        srd2.accel_count = srd2.decel_val;           /* 减速步数做为加速计数值 */
                        new_step_delay   = last_accel_delay;         /* 加阶段最后的延时做为减速阶段的起始延时(脉冲周期) */
                        srd2.run_state   = RT_STEPMOTOR_MODE_DECEL;  /* 状态改变为减速 */
                    }
                }
                break;

                /* 减速阶段 */
                case RT_STEPMOTOR_MODE_DECEL:
                {
                    step_count++;

                    if(srd2.dir == RT_STEPMOTOR_CLOCKWISE)
                    {
                        stepmotor2_position++;
                    }
                    else
                    {
                        stepmotor2_position--;
                    }

                    srd2.accel_count++;

                    /* 计算下一步脉冲周期(时间间隔) */
                    new_step_delay = srd2.step_delay - (((2 * srd2.step_delay) + rest)/(4 * srd2.accel_count + 1));

                    /* 计算余数，下次计算补上余数，减少误差 */
                    rest = ((2 * srd2.step_delay)+rest)%(4 * srd2.accel_count + 1);

                    /* 检查是否为最后一步 */
                    if(srd2.accel_count >= 0)
                    {
                        srd2.run_state = RT_STEPMOTOR_MODE_STOP;
                    }
                }
                break;
            }

            /* 为下个延时(脉冲周期)赋值 */
            srd2.step_delay = new_step_delay;
        }
    }
#endif
}

/* 步进电机初始化 */
int rt_hw_stepmotor_init(void)
{
    /* 电机1初始化 */
#ifdef RT_USING_STEPMOTOR1
    rt_stepmotor1_gpio_init();
    rt_stepmotor1_timx_init();
#endif

    /* 电机2初始化 */
#ifdef RT_USING_STEPMOTOR2
    rt_stepmotor2_gpio_init();
    rt_stepmotor2_timx_init();
#endif

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_stepmotor_init);

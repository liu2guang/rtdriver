/*
 * @File:   drv_sdram.c 
 * @Author: liuguang 
 * @Date:   2017-10-02 03:20:05 
 * 
 * @LICENSE: 
 * The code owner is liuguang. Other personnel may 
 * not be used or modified.
 * 
 * Change Logs: 
 * Date           Author       Notes 
 * 2017-10-02     liuguang     The first version. 
 * 2018-01-15     liuguang     Update f469 MICRON MT48LC4M32B2B5-6A. 
 */ 
 
#include "drv_sdram.h"
#include "stm32f4xx.h"

static SDRAM_HandleTypeDef hsdram;
static FMC_SDRAM_CommandTypeDef command;

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
    GPIO_InitTypeDef GPIO_Init_Structure;
    
    /* gpio clock enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    
    /* fmc clock enable */
    __HAL_RCC_FMC_CLK_ENABLE();
    
    /* common gpio configuration */
    GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull      = GPIO_NOPULL;
    GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
    GPIO_Init_Structure.Alternate = GPIO_AF12_FMC;
    
    /* addrline configuration A0 ~ A9 */
    GPIO_Init_Structure.Pin = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                              GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_12 | GPIO_PIN_13 | 
                              GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);
    
    /* addrline configuration A10 ~ A11 */
    GPIO_Init_Structure.Pin = GPIO_PIN_0  | GPIO_PIN_1;
    HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
    
    /* dataline configuration D0 ~ D3, D13 ~ D15 */
    GPIO_Init_Structure.Pin = GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0  | GPIO_PIN_1  |
                              GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);
    
    /* dataline configuration D4 ~ D12 */
    GPIO_Init_Structure.Pin = GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 |
                              GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                              GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);
    
    /* dataline configuration D16 ~ D23 */
    GPIO_Init_Structure.Pin = GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 |
                              GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH, &GPIO_Init_Structure);
    
    /* dataline configuration D24 ~ D31 */
    GPIO_Init_Structure.Pin = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                              GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_9  | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);
    
    /* ctr line configuration */
    GPIO_Init_Structure.Pin = GPIO_PIN_3;           /* CS: PH3 */
    HAL_GPIO_Init(GPIOH, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_4;           /* BA0: PG4 */
    HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_5;           /* BA1: PG5 */
    HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_0;           /* WE: PC0 */ 
    HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_11;          /* RAS: PF11 */
    HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_15;          /* CAS: PG15 */
    HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_8;           /* CLK: PG8 */
    HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_2;           /* CKE: PH2 */
    HAL_GPIO_Init(GPIOH, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_1;           /* UDQM(NBL1): PE1 */
    HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_0;           /* LDQM(NBL0): PE0 */
    HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_4;           /* NBL2: PI4 */
    HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);
    
    GPIO_Init_Structure.Pin = GPIO_PIN_5;           /* NBL3: PI5 */
    HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);
}

static void sdram_init(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
    __IO uint32_t tmpmrd = 0;
    
    /* configure a clock configuration enable command */
    Command->CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
    Command->CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber      = 1;
    Command->ModeRegisterDefinition = 0;
    
    /* send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
    
    /* insert 100 ms delay, interrupt is not enable, just to delay some time */
    for(tmpmrd = 0; tmpmrd < 0xfffff; tmpmrd ++);
    
    /* configure a PALL (precharge all) command */
    Command->CommandMode            = FMC_SDRAM_CMD_PALL;
    Command->CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber      = 1;
    Command->ModeRegisterDefinition = 0;
    
    /* send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
    
    /* configure a Auto-Refresh command */
    Command->CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command->CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber      = 8;
    Command->ModeRegisterDefinition = 0;
    
    /* send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
    for(tmpmrd = 0; tmpmrd < 0xfffff; tmpmrd ++);
    
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
    for(tmpmrd = 0; tmpmrd < 0xfffff; tmpmrd ++);
    
    /* program the external memory mode register */
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
                       SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                       SDRAM_MODEREG_CAS_LATENCY_3           |
                       SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                       SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
                       
    Command->CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
    Command->CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber      = 1;
    Command->ModeRegisterDefinition = tmpmrd;
    
    /* send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
    
    /* set the refresh rate counter, (15.62 us x Freq) - 20 */
    HAL_SDRAM_ProgramRefreshRate(hsdram, SDRAM_REFRESH_COUNT);
}

int rt_hw_sdram_init(void)
{
    FMC_SDRAM_TimingTypeDef SDRAM_Timing;
    
    hsdram.Instance = FMC_SDRAM_DEVICE;
    
    SDRAM_Timing.LoadToActiveDelay    = 2;
    SDRAM_Timing.ExitSelfRefreshDelay = 7;
    SDRAM_Timing.SelfRefreshTime      = 4;
    SDRAM_Timing.RowCycleDelay        = 7;
    SDRAM_Timing.WriteRecoveryTime    = 2;
    SDRAM_Timing.RPDelay              = 2;
    SDRAM_Timing.RCDDelay             = 2;
    
    hsdram.Init.SDBank             = FMC_SDRAM_BANK1;
    hsdram.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
    hsdram.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
    hsdram.Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32;
    hsdram.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hsdram.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;
    hsdram.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram.Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;
    hsdram.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
    hsdram.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;
    
    if(HAL_SDRAM_Init(&hsdram, &SDRAM_Timing) != HAL_OK)
    {
        while(1);
    }
    
    sdram_init(&hsdram, &command);
    
    return RT_EOK;
}
INIT_BOARD_EXPORT(rt_hw_sdram_init);

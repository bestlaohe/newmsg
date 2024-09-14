#include "SX1278.h"

//频率数据表
const u8 sx1276_7_8FreqTbl[][3] = 
{ 
  {0x6B, 0x00, 0x00},  //428MHz  
  {0x6B, 0x40, 0x00},  //429MHz
  {0x6B, 0x80, 0x00},  //430MHz
  {0x6B, 0xC0, 0x00},  //431MHz
  {0x6C, 0x00, 0x00},  //432MHz
  {0x6C, 0x40, 0x00},  //433MHz
  {0x6C, 0x80, 0x00},  //434MHz
  {0x6C, 0xC0, 0x00},  //435MHz
  {0x6D, 0x00, 0x00},  //436MHz
  {0x6D, 0x40, 0x00},  //437MHz
  {0x6D, 0x80, 0x00},  //438MHz
  {0x6D, 0xC0, 0x00}   //439MHz
};
//

#define SX1278_MAX_BUFSIZE  64 //定义接收最大数据长度

//参数配置部分，其中只有载波频率是可以通过程序更改，其参数设置后为固定值
u8 Lora_Freq          = LORAFREQ_434MHZ;          //  默认频率设置
u8 Lora_Power         = LORAPOWER_20DBM;          //  输出功率设置
u8 Lora_SpreadFactor  = 7;                        //  设置扩频因子在7~12
u8 Lora_BandWide      = LORABW_125KHz;            //  带宽设置
u8 Lora_ErrorCoding   = ERROR_CODING_4_5;         //  前向纠错4/5 4/6 4/7 4/8  
/********************************************************************/


/****************以下是移植需要实现的对应接口部分***********************************/
#define SX1278_DelayMs(t)     Delay_Ms(t)  //毫秒延时函数的实现

#define SX1278_GPIO             GPIOC        //GPIO选择
#define SX1278_NSEL             GPIO_Pin_0   //引脚定义
#define SX1278_MOSI             GPIO_Pin_6   //引脚定义
#define SX1278_MISO             GPIO_Pin_7   //引脚定义
#define SX1278_SCK              GPIO_Pin_5   //引脚定义
#define SX1278_SDNN             GPIO_Pin_1   //引脚定义
#define SX1278_NIRQ             GPIO_Pin_6   //引脚定义

#define SET_SX1278_NSEL() 		GPIO_SetBits(SX1278_GPIO,SX1278_NSEL)
#define CLR_SX1278_NSEL() 		GPIO_ResetBits(SX1278_GPIO,SX1278_NSEL)//使能
#define READ_SX1278_MISO()		GPIO_ReadInputDataBit(SX1278_GPIO,SX1278_MISO)
#define SET_SX1278_SCK()		GPIO_SetBits(SX1278_GPIO,SX1278_SCK)
#define CLR_SX1278_SCK()		GPIO_ResetBits(SX1278_GPIO,SX1278_SCK)
#define SET_SX1278_MOSI()		GPIO_SetBits(SX1278_GPIO,SX1278_MOSI)
#define CLR_SX1278_MOSI()		GPIO_ResetBits(SX1278_GPIO,SX1278_MOSI)
#define SET_SX1278_SDN()		GPIO_SetBits(SX1278_GPIO,SX1278_SDNN)
#define CLR_SX1278_SDN()		GPIO_ResetBits(SX1278_GPIO,SX1278_SDNN)//用于将 SX1278 模块置于关机模式
#define READ_SX1278_NIRQ()		GPIO_ReadInputDataBit(GPIOD,SX1278_NIRQ)//引脚是 SX1278 模块的中断请求引脚




//控制口配置初始化，中断口配置在完成中断初始化中
void SX1278_test() //SPI初始化
{   u8 lorabuf[21]; // lora的buf
    u8 res;         // 操作的返回
    u8 len;
        if (SX1278_LoRaTxPacket(lorabuf, 10))
        {
            printf("TX fail \r\n");
        }

        res = SX1278_LoRaRxPacket(lorabuf, &len, 3000);
        if (res == 0)
        {
            printf("RX sucess \r\n");
            Delay_Ms(500);
        }
        else if (res == 1)
        {
            printf("Time out!\r\n");
        }
        else if (res == 2)
        {
            printf("CRC eeror!\r\n");
        }
}

//控制口配置初始化，中断口配置在完成中断初始化中
void SX1278_GPIO_Init() //SPI初始化
{

    SET_SX1278_NSEL();
    SET_SX1278_SDN();
}
//SPI初始化模拟的方式实现
void SX1278_SPI_Init()
{
    SET_SX1278_MOSI();
    SET_SX1278_SCK();
}

//上升有效，高位在前，SCK平时为低电平
u8 SX1278_SPI_RW(u8 byte)
{
    return(SPI_I2S_ReceiveData(SPI1));          // 返回读出的一字节
}


/*********************************************************************
 * @fn      EXTI0_INT_INIT
 *
 * @brief   Initializes EXTI0 collection.
 *
 * @return  none
 */
void EXTI6_INT_INIT(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);

 

    /* GPIOA ----> EXTI_Line0 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource3);
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*****************以上是移植需要实现的部分**************************************************************/


/*****************以下与MCU无关，移植无需更改*******************************************************/
/**********************************************************
**Name:     SX1278_Read_Reg
**Function: SPI Read CMD
**Input:    adr -> address for read
**Output:   None
**********************************************************/
u8 SX1278_Read_Reg(u8 adr)
{
  u8 tmp;
  CLR_SX1278_NSEL();
  SX1278_SPI_RW(adr); 
  tmp = SX1278_SPI_RW(adr);  
  SET_SX1278_NSEL();
  return(tmp);
}

/**********************************************************
**Name:     SX1278_Write_Reg
**Function: SPI Write CMD
**Input:    u8 address & u8 data
**Output:   None
**********************************************************/
void SX1278_Write_Reg(u8 adr, u8 WrPara)  
{
	
	CLR_SX1278_NSEL();						

    SX1278_SPI_RW(adr|0x80);
	SX1278_SPI_RW(WrPara);//写入数据
    
    SET_SX1278_NSEL();
}
/**********************************************************
**Name:     SX1278_Burst_Read
**Function: SPI burst read mode
**Input:    adr-----address for read
**          ptr-----data buffer point for read
**          length--how many bytes for read
**Output:   None
**********************************************************/
void SX1278_Burst_Read(u8 adr, u8 *ptr, u8 length)
{
  u8 i;
  if(length<=1)                                            //length must more than one
    return;
  else
  {
    CLR_SX1278_NSEL();
    SX1278_SPI_RW(adr); 
    for(i=0;i<length;i++)
    ptr[i] = SX1278_SPI_RW(0);
    SET_SX1278_NSEL();  
  }
}

/**********************************************************
**Name:     SPISX1278_Burst_Write
**Function: SPI burst write mode
**Input:    adr-----address for write
**          ptr-----data buffer point for write
**          length--how many bytes for write
**Output:   none
**********************************************************/
void SX1278_Burst_Write(u8 adr, u8 *ptr, u8 length)
{ 
  u8 i;
  if(length > 0)                                            //length must more than one
  {   
    CLR_SX1278_NSEL();        
    SX1278_SPI_RW(adr|0x80);
    for(i=0;i<length;i++)
        SX1278_SPI_RW(ptr[i]);
    SET_SX1278_NSEL();  
  }
}
/*************************************************************************
 功能：进入准备状态
 参数：无
 返回：无
***************************************************************************/
void SX1278_Standby(void)
{
  SX1278_Write_Reg(LR_RegOpMode,0x09);                              		//Standby//Low Frequency Mode
	//SX1278_Write_Reg(LR_RegOpMode,0x01);                              	 //Standby//High Frequency Mode
}

/*************************************************************************
 功能：进入睡眠状态
 参数：无
 返回：无
***************************************************************************/
void SX1278_Sleep(void)
{
  SX1278_Write_Reg(LR_RegOpMode,0x08);                              		//Sleep//Low Frequency Mode
	//SX1278_Write_Reg(LR_RegOpMode,0x00);                            		 //Sleep//High Frequency Mode
}

/*********************************************************/
//LoRa mode
/*********************************************************/
/*************************************************************************
 功能：进入LORA模式
 参数：无
 返回：无
***************************************************************************/
void SX1278_EntryLoRa(void)
{
  SX1278_Write_Reg(LR_RegOpMode,0x88);//Low Frequency Mode
	//SX1278_Write_Reg(LR_RegOpMode,0x80);//High Frequency Mode
}

/*************************************************************************
 功能：清中断标志位
 参数：无
 返回：无
***************************************************************************/
void SX1278_LoRaClearIrq(void)
{
  SX1278_Write_Reg(LR_RegIrqFlags,0xFF);
}


/*************************************************************************
 功能：基本寄存器配置
 参数：无
 返回：无
***************************************************************************/
void SX1278_Config(void)
{

//睡眠模式由初始化和接收发送函数完成
    SX1278_Sleep();                                      //Change modem mode Must in Sleep mode 
	SX1278_DelayMs(10);

	SX1278_EntryLoRa();  
	//SX1278_Write_Reg(0x5904);   //Change digital regulator form 1.6V to 1.47V: see errata note
    
	SX1278_Burst_Write(LR_RegFrMsb,(u8 *)sx1276_7_8FreqTbl[Lora_Freq],3);  //setting frequency parameter
 
	//setting base parameter 输出功率
	SX1278_Write_Reg(LR_RegPaConfig,Lora_Power);             //Setting output power parameter  
    
	SX1278_Write_Reg(LR_RegOcp,0x0B);                              //RegOcp,Close Ocp 电流限制 100mA
	SX1278_Write_Reg(LR_RegLna,0x23);                              //RegLNA,High & LNA Enable，高灵敏度
    
//	if(Lora_SpreadFactor==6)           //SFactor=6//不使用扩频6档，
//	{
//		u8 tmp;
//		SX1278_Write_Reg(LR_RegModemConfig1,((Lora_BandWide<<4)+(Lora_ErrorCoding<<1)+0x01));//Implicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
//		SX1278_Write_Reg(LR_RegModemConfig2,((Lora_SpreadFactor<<4)+0x04+0x03));
//      
//		tmp = SX1278_Read_Reg(0x31);
//		tmp &= 0xF8;
//		tmp |= 0x05;
//		SX1278_Write_Reg(0x31,tmp);
//		SX1278_Write_Reg(0x37,0x0C);
//        Debug_Printf("SFactor=6\r\n");
//	} 
//	else
//	{                                                                                
		SX1278_Write_Reg(LR_RegModemConfig1,((Lora_BandWide<<4)+(Lora_ErrorCoding<<1)+0x00));//带宽配置 & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)，显示报头模式
		SX1278_Write_Reg(LR_RegModemConfig2,((Lora_SpreadFactor<<4)+0x04+0x03));  //SFactor & CRC LNA gain set by the internal AGC loop 
//	}
//	SX1278_Write_Reg(LR_RegSymbTimeoutLsb,0xFF);                   //RegSymbTimeoutLsb Timeout = 0x3FF(Max) 
    
	SX1278_Write_Reg(LR_RegPreambleMsb,0x00);                       //RegPreambleMsb 
	SX1278_Write_Reg(LR_RegPreambleLsb,12);                      //RegPreambleLsb 8+4=12byte Preamble
    
	SX1278_Write_Reg(REG_LR_DIOMAPPING2,0x01);                     //RegDioMapping2 DIO5=00, DIO4=01
	
    SX1278_Standby();                                         //Entry standby mode

}

/*************************************************************************
 功能：进入接收模式
 参数：无
 返回：无
***************************************************************************/
u8 SX1278_LoRaEntryRx(void)
{
  u8 addr = 0; 
  u8 t = 5;
  SX1278_Standby();             //进入待机模式从待机模式进入其他模式
  SX1278_Write_Reg(REG_LR_PADAC,0x84);                              //zhen and Rx
  SX1278_Write_Reg(LR_RegHopPeriod,0xFF);                          //接收模式跳频周期写0XFF不跳频
  SX1278_Write_Reg(REG_LR_DIOMAPPING1,0x01);                       //DIO0=00, DIO1=00, DIO2=00, DIO3=01
      
  SX1278_Write_Reg(LR_RegIrqFlagsMask,0x9F);                       //Open RxDone interrupt & CRC
  SX1278_LoRaClearIrq();   
  
//  SX1278_Write_Reg(LR_RegPayloadLength,21);         //RegPayloadLength  21byte(this register must difine when the data long of one byte in SF is 6)
                                                      //使用定长数据包时需要配置，不使用定长无需配置
    
  addr = SX1278_Read_Reg(LR_RegFifoRxBaseAddr);           				//Read RxBaseAddr
  SX1278_Write_Reg(LR_RegFifoAddrPtr,addr);                        //RxBaseAddr -> FiFoAddrPtr　 
  SX1278_Write_Reg(LR_RegOpMode,0x8d);                        		//Continuous Rx Mode//Low Frequency Mode
  //SX1278_Write_Reg(LR_RegOpMode,0x05);                          //Continuous Rx Mode//High Frequency Mode
	while(t--)
	{		
        if((SX1278_Read_Reg(LR_RegModemStat)&0x04)==0x04)   //Rx-on going RegModemStat
			break;
	}
    return 1;
}
/*************************************************************************
 功能：获取Lora信号强度
 参数：无
 返回：信号强度 127最大
***************************************************************************/
u8 SX1278_LoRaReadRSSI(void)
{
  u16 temp=10;
  temp=SX1278_Read_Reg(LR_RegRssiValue);                  //Read RegRssiValue，Rssi value
  temp=temp+127-137;                                       //127:Max RSSI, 137:RSSI offset
  return (u8)temp;
}
/*************************************************************************
 功能：获取接收包，阻塞函数 不是通过信号量阻塞，而是用延时轮询阻塞
 参数：valid_data[out]     外部接收数组首地址 
      packet_length[out]  接收数据的长度
            timeout[in]   接收超时，为0时永久等待 单位时间ms
返回：	 0:接收成功 1：接收失败超时 2:CRC校验错误
***************************************************************************/
u8 SX1278_LoRaRxPacket(u8 *valid_data, u8* packet_length,u16 timeout)
{
    u8 addr,irq_flag;
    u8 packet_size;
    SX1278_LoRaEntryRx(); //进入接收模式
    timeout = timeout/5;
    while(timeout != 1)  //timeout 减到1就退出 如果是0 就永远不能退出
    { 
        SX1278_DelayMs(5); 
        if(timeout > 1) timeout --;
        if(READ_SX1278_NIRQ())
        {
//            Debug_Printf("SX1278_RX_NIRQ()\r\n");
            irq_flag=SX1278_Read_Reg(LR_RegIrqFlags);
            if((irq_flag & RFLR_IRQFLAGS_PAYLOADCRCERROR) == RFLR_IRQFLAGS_PAYLOADCRCERROR) //如果是CRC校验错误中断
            {
                SX1278_LoRaClearIrq(); 
                SX1278_Sleep();  //进入睡眠模式，为config做准备
                return (2);
            }
            addr = SX1278_Read_Reg(LR_RegFifoRxCurrentaddr);      //last packet addr
            SX1278_Write_Reg(LR_RegFifoAddrPtr,addr);                      //RxBaseAddr -> FiFoAddrPtr    
            packet_size = SX1278_Read_Reg(LR_RegRxNbBytes);     //Number for received bytes   
            SX1278_Burst_Read(0x00, valid_data, packet_size);
            *packet_length = packet_size;
            SX1278_LoRaClearIrq();  
            SX1278_Sleep();  //进入睡眠模式，
            return(0);
        }
    }
    SX1278_Sleep(); //进入睡眠模式，
    return(1);
}

/*************************************************************************
 功能：进入发送模式
 参数：无
 返回：无
***************************************************************************/
u8 SX1278_LoRaEntryTx(u8 packet_length)
{
    u8 addr = 0,temp;
    u8 t = 3;     
    SX1278_Standby();                           //进入待机模式从待机模式进入其他模式
    SX1278_Write_Reg(REG_LR_PADAC,0x87);                                   //Tx for 20dBm发送时需要设置的值
    SX1278_Write_Reg(LR_RegHopPeriod,0x00);                               //发送模式跳频周期写0X00不跳频
    SX1278_Write_Reg(REG_LR_DIOMAPPING1,0x41);                       //DIO0=01, DIO1=00, DIO2=00, DIO3=01

    
    SX1278_Write_Reg(LR_RegIrqFlagsMask,0xF7);                       //Open TxDone interrupt
    SX1278_LoRaClearIrq();
    
    SX1278_Write_Reg(LR_RegPayloadLength,packet_length);             

    addr = SX1278_Read_Reg(LR_RegFifoTxBaseAddr);           //RegFiFoTxBaseAddr
    SX1278_Write_Reg(LR_RegFifoAddrPtr,addr);               //RegFifoAddrPtr
    while(t --)
    {
        temp=SX1278_Read_Reg(LR_RegPayloadLength);
        if(temp==packet_length) break; 
    }
    return 0;
}
/*************************************************************************
 功能：发送数据包,阻塞函数 不是通过信号量阻塞，而是用延时轮询阻塞，
      阻塞最大时间500ms
 参数：tx_buf[in]         外部发送数数据首地址
       packet_length[in] 发送数据长度
 返回：0:发送成功 1：发送失败
***************************************************************************/
u8 SX1278_LoRaTxPacket(u8 *valid_data, u8 packet_length)
{ 
    u8 timeout = 100;
    SX1278_LoRaEntryTx(packet_length);
	SX1278_Burst_Write(0x00, valid_data, packet_length);
	SX1278_Write_Reg(LR_RegOpMode,0x8b);  //Tx Mode           
	while(-- timeout)
	{
        SX1278_DelayMs(5);
		if(READ_SX1278_NIRQ())                      //Packet send over
		{    
//            Debug_Printf("SX1278_TX_NIRQ\r\n");
			SX1278_Read_Reg(LR_RegIrqFlags);
			SX1278_LoRaClearIrq();                                //Clear irq			
//			SX1278_Standby();                                     //Entry Standby mode 
     SX1278_Sleep();                                      //进入睡眠模式
			return 0;
		}
	} 
    SX1278_Sleep();                                      //进入睡眠模式
    return 1;
}

/*************************************************************************
 功能：获取信号强度
 参数：无
 返回：信号强度 127最大
***************************************************************************/
u8 SX1278_ReadRSSI(void)
{
  u8 temp=0xff;
	
  temp=SX1278_Read_Reg(0x11);
  temp>>=1;
  temp=127-temp;                                           //127:Max RSSI
  return temp;
}
/*************************************************************************
 功能：模块初始化
 参数：freq[in]: 载波频率设置 428 ~ 439 单位MHz
 返回：无
***************************************************************************/
void SX1278_Init(u16 freq)
{
    SX1278_GPIO_Init();
    SX1278_SPI_Init(); //SPI初始化
    if(freq>=428 && freq <= 439)  //在设置范围内，不然就按照默认频率设置
    {
        Lora_Freq = freq%428; //设置载波频率
    }
    CLR_SX1278_SDN();
    SX1278_DelayMs(2);
    SET_SX1278_SDN();
    SX1278_DelayMs(6);
    SX1278_Config(); 
}

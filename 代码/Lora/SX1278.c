#include "SX1278.h"
#include <string.h> // 包含标准C库中的字符串和内存操作函数
// 频率数据表
const u8 sx1276_7_8FreqTbl[][3] =
    {
        {0x6B, 0x00, 0x00}, // 428MHz
        {0x6B, 0x40, 0x00}, // 429MHz
        {0x6B, 0x80, 0x00}, // 430MHz
        {0x6B, 0xC0, 0x00}, // 431MHz
        {0x6C, 0x00, 0x00}, // 432MHz
        {0x6C, 0x40, 0x00}, // 433MHz
        {0x6C, 0x80, 0x00}, // 434MHz
        {0x6C, 0xC0, 0x00}, // 435MHz
        {0x6D, 0x00, 0x00}, // 436MHz
        {0x6D, 0x40, 0x00}, // 437MHz
        {0x6D, 0x80, 0x00}, // 438MHz
        {0x6D, 0xC0, 0x00}  // 439MHz
};
//

#define SX1278_MAX_BUFSIZE 64 // 定义接收最大数据长度

// 参数配置部分，其中只有载波频率是可以通过程序更改，其参数设置后为固定值
u8 Lora_Freq = LORAFREQ_434MHZ;         //  默认频率设置428-439MHz
u8 Lora_Power = LORAPOWER_20DBM;        //  输出功率设置11，14，17，20
u8 Lora_SpreadFactor = 7;               //  设置扩频因子在7~12
u8 Lora_BandWide = LORABW_125KHz;       //  带宽设置
u8 Lora_ErrorCoding = ERROR_CODING_4_5; //  前向纠错4/5 4/6 4/7 4/8
/********************************************************************/

/****************以下是移植需要实现的对应接口部分***********************************/
#define SX1278_DelayMs(t) Delay_Ms(t) // 毫秒延时函数的实现
char lora_receive_buf[145] = {0};
volatile u8 lora_receive_len = 1;
volatile u8 lora_receive_flag = 0; // 0是初始状态，1是接收到特殊回应了，2是等待接收回应，3是接收到了数据

extern volatile u8 loraComplete; // lora中断操作完成的标志
// 控制口配置初始化，中断口配置在完成中断初始化中
void SX1278_Receive()
{

  //  DEBUG_PRINT("lora ID  0x%X\r\n", SX1278_Read_Reg(REG_LR_VERSION)); // 0x12
  //  DEBUG_PRINT("lora mode  0x%X\r\n", SX1278_Read_Reg(LR_RegOpMode)); // 0x89  0100 0101
  //
  //  DEBUG_PRINT("SX1278_LoRaReadRSSI= %d \r\n", SX1278_LoRaReadRSSI());
  //  DEBUG_PRINT("SX1278_ReadRSSI= %d \r\n", SX1278_ReadRSSI());

  u8 res; // 操作的返回

  res = SX1278_LoRaRxPacket(lora_receive_buf, 5000);

  if (res == 0)
  {

    DEBUG_PRINT("lora_receive_len=  %d\r\n", lora_receive_len);

    u8 var;
    for (var = 0; var < lora_receive_len; ++var)
    {
      DEBUG_PRINT("lora_receive_buf= %d\r\n", lora_receive_buf[var]);
    }
    // 实际的数据要去掉尾部的*=42
    if (lora_receive_len < 1)
    {
      DEBUG_PRINT("SX1278_Receive erro\r\n");
      return;
    }
    lora_receive_buf[lora_receive_len - 1] = 0; // 要清空不然以为有东西会出现
    lora_receive_len = lora_receive_len - 1;
  }
  else if (res == 2)
  {
    DEBUG_PRINT("lora CRC eeror!\r\n");
  }
  else if (res == 3)
  {
    DEBUG_PRINT("Received response: **\r\n");
  }
  else if (res == 4)
  {
    DEBUG_PRINT("illegal msg\r\n");
  }
}

// void SX1278_GPIO_Init()
// {
//   SX1278_NSS_DISABLE;
//   SET_SX1278_RST();
// }
// void SX1278_SPI_Init()
// {

//   SET_SX1278_MOSI();
//   SET_SX1278_SCK();
// }

// 上升有效，高位在前，SCK平时为低电平
u8 SX1278_SPI_RW(u8 byte)
{
  u8 retry = 0;

  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
  {
    retry++;
    if (retry > 200)
    {
      DEBUG_PRINT("send spi outtime!\r\n");
      return 0;
    }

  } // 还没发完
  SPI_I2S_SendData(SPI1, (uint16_t)byte);

  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
  {
    retry++;
    if (retry > 200)
    {
      DEBUG_PRINT("rec spi outtime!\r\n");
      return 0;
    }
  } // 还没收完

  return (uint8_t)SPI_I2S_ReceiveData(SPI1);
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

  while (!READ_LCD_CS)
  {
    DEBUG_PRINT("wait lcd cs 1!\r\n");
  }
  u8 tmp;
  SX1278_NSS_ENABLE;
  SX1278_SPI_RW(adr);
  tmp = SX1278_SPI_RW(adr);
  SX1278_NSS_DISABLE;
  return (tmp);
}

/**********************************************************
**Name:     SX1278_Write_Reg
**Function: SPI Write CMD
**Input:    u8 address & u8 data
**Output:   None
**********************************************************/
void SX1278_Write_Reg(u8 adr, u8 WrPara)
{
  while (!READ_LCD_CS)
  {

    DEBUG_PRINT("wait lcd cs 1!\r\n");
  }
  SX1278_NSS_ENABLE;
  SX1278_SPI_RW(adr | 0x80);
  SX1278_SPI_RW(WrPara); // 写入数据

  SX1278_NSS_DISABLE;
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
  if (length <= 1) // length must more than one
    return;
  else
  {
    while (!READ_LCD_CS)
    {

      DEBUG_PRINT("wait lcd cs 1!\r\n");
    }
    SX1278_NSS_ENABLE;
    SX1278_SPI_RW(adr);
    for (i = 0; i < length; i++)
      ptr[i] = SX1278_SPI_RW(0);
    SX1278_NSS_DISABLE;
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
  if (length > 0) // length must more than one
  {
    while (!READ_LCD_CS)
    {

      DEBUG_PRINT("wait lcd cs 1!\r\n");
    }
    SX1278_NSS_ENABLE;
    SX1278_SPI_RW(adr | 0x80);
    for (i = 0; i < length; i++)
    {

      //  DEBUG_PRINT(" ptr[i]: %d\r\n", ptr[i]);
      SX1278_SPI_RW(ptr[i]);
    }
    SX1278_NSS_DISABLE;
  }
}

// 功能: SX1278 单个寄存器读操作
unsigned char SX1276ReadReg(unsigned char addr)
{
  unsigned char dat;
  while (!READ_LCD_CS)
  {

    DEBUG_PRINT("wait lcd cs 1!\r\n");
  }
  SX1278_NSS_ENABLE;
  SX1278_SPI_RW(addr & 0x7F);
  dat = SX1278_SPI_RW(0);
  SX1278_NSS_DISABLE;
  return dat;
}

/*************************************************************************
 功能：进入准备状态
 参数：无
 返回：无
***************************************************************************/
void SX1278_Standby(void)
{
  SX1278_Write_Reg(LR_RegOpMode, 0x09); // Standby//Low Frequency Mode
  // SX1278_Write_Reg(LR_RegOpMode,0x01);                              	 //Standby//High Frequency Mode
}

/*************************************************************************
 功能：进入睡眠状态
 参数：无
 返回：无
***************************************************************************/
void SX1278_Sleep(void)
{
  SX1278_Write_Reg(LR_RegOpMode, 0x08); // Sleep//Low Frequency Mode
  // SX1278_Write_Reg(LR_RegOpMode,0x00);                            		 //Sleep//High Frequency Mode
}

/*********************************************************/
// LoRa mode
/*********************************************************/
/*************************************************************************
 功能：进入LORA模式
 参数：无
 返回：无
***************************************************************************/
void SX1278_EntryLoRa(void)
{
  SX1278_Write_Reg(LR_RegOpMode, 0x88); // Low Frequency Mode
  // SX1278_Write_Reg(LR_RegOpMode,0x80);//High Frequency Mode
}

/*************************************************************************
 功能：清中断标志位
 参数：无
 返回：无
***************************************************************************/
void SX1278_LoRaClearIrq(void)
{
  SX1278_Write_Reg(LR_RegIrqFlags, 0xFF);
}

/*************************************************************************
 功能：基本寄存器配置
 参数：无
 返回：无
***************************************************************************/
void SX1278_Config(void)
{

  // 睡眠模式由初始化和接收发送函数完成
  SX1278_Sleep(); // 改变模式需要在睡眠模式
  SX1278_DelayMs(10);

  SX1278_EntryLoRa();
  // SX1278_Write_Reg(0x5904);   //Change digital regulator form 1.6V to 1.47V: see errata note

  SX1278_Burst_Write(LR_RegFrMsb, (u8 *)sx1276_7_8FreqTbl[Lora_Freq], 3); // setting frequency parameter

  // setting base parameter 输出功率
  SX1278_Write_Reg(LR_RegPaConfig, Lora_Power); // Setting output power parameter

  SX1278_Write_Reg(LR_RegOcp, 0x0B); // RegOcp,Close Ocp 电流限制 100mA
  SX1278_Write_Reg(LR_RegLna, 0x23); // RegLNA,High & LNA Enable，高灵敏度

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
  //        DEBUG_PRINT("SFactor=6\r\n");
  //	}
  //	else
  //	{
  SX1278_Write_Reg(LR_RegModemConfig1, ((Lora_BandWide << 4) + (Lora_ErrorCoding << 1) + 0x00)); // 带宽配置 & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)，显示报头模式
  SX1278_Write_Reg(LR_RegModemConfig2, ((Lora_SpreadFactor << 4) + 0x04 + 0x03));                // SFactor & CRC LNA gain set by the internal AGC loop
  //	}
  //	SX1278_Write_Reg(LR_RegSymbTimeoutLsb,0xFF);                   //RegSymbTimeoutLsb Timeout = 0x3FF(Max)

  SX1278_Write_Reg(LR_RegPreambleMsb, 0x00); // RegPreambleMsb
  SX1278_Write_Reg(LR_RegPreambleLsb, 12);   // RegPreambleLsb 8+4=12byte Preamble

  SX1278_Write_Reg(REG_LR_DIOMAPPING2, 0x01); // RegDioMapping2 DIO5=00, DIO4=01

  SX1278_Standby(); // Entry standby mode
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
  SX1278_Standby();                     // 进入待机模式从待机模式进入其他模式
  SX1278_Write_Reg(REG_LR_PADAC, 0x84); // zhen and Rx

  SX1278_Write_Reg(LR_RegHopPeriod, 0xFF);    // 接收模式跳频周期写0XFF不跳频
  SX1278_Write_Reg(REG_LR_DIOMAPPING1, 0x01); // DIO0=00, DIO1=00, DIO2=00, DIO3=01

  SX1278_Write_Reg(LR_RegIrqFlagsMask, 0x9F); // Open RxDone interrupt & CRC  8001 1111

  //  SX1278_Write_Reg(REG_LR_DIOMAPPING1, 0x41); // DIO0=01, DIO1=00, DIO2=00, DIO3=01
  //  SX1278_Write_Reg(LR_RegIrqFlagsMask, 0x0F); // Open RxDone interrupt & CRC  8001

  SX1278_LoRaClearIrq();

  //  SX1278_Write_Reg(LR_RegPayloadLength,21);         //RegPayloadLength  21byte(this register must difine when the data long of one byte in SF is 6)
  // 使用定长数据包时需要配置，不使用定长无需配置

  addr = SX1278_Read_Reg(LR_RegFifoRxBaseAddr); // Read RxBaseAddr
  SX1278_Write_Reg(LR_RegFifoAddrPtr, addr);    // RxBaseAddr -> FiFoAddrPtr　
  SX1278_Write_Reg(LR_RegOpMode, 0x8d);         // Continuous Rx Mode//Low Frequency Mode 1000 1101
  // SX1278_Write_Reg(LR_RegOpMode,0x05);                          //Continuous Rx Mode//High Frequency Mode

  //  while (SX1278_Read_Reg(LR_RegOpMode)!=0x88)//1000 1000 tx完成
  while (t--)
  {

    // DEBUG_PRINT("Rx Mode=0x%X \r\n", SX1278_Read_Reg(LR_RegOpMode));//0x8d 1000 1010
    if ((SX1278_Read_Reg(LR_RegModemStat) & 0x04) == 0x04) // Rx-on going RegModemStat
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
  u16 temp = 10;
  temp = SX1278_Read_Reg(LR_RegRssiValue); // Read RegRssiValue，Rssi value
  temp = temp + 127 - 137;                 // 127:Max RSSI, 137:RSSI offset
  return (u8)temp;
}
/*************************************************************************
 功能：获取接收包，阻塞函数 不是通过信号量阻塞，而是用延时轮询阻塞
 参数：valid_data[out]     外部接收数组首地址
      packet_length[out]  接收数据的长度
            timeout[in]   接收超时，为0时永久等待 单位时间ms
返回：	 0:接收成功 1：接收失败超时 2:CRC校验错误
***************************************************************************/
u8 SX1278_LoRaRxPacket(u8 *valid_data, u16 timeout)
{
  u8 addr, irq_flag, mode;
  u8 packet_size;
  u8 temp_data[200];

  mode = SX1278_Read_Reg(LR_RegOpMode);

  if ((mode & 0x05) != 0x05)
  {
    // DEBUG_PRINT("enter rxmode  \r\n");
    SX1278_LoRaEntryRx(); // 进入接收模式
  }

  if (loraComplete) // 有中断进来
  {
    loraComplete = 0;
    irq_flag = SX1278_Read_Reg(LR_RegIrqFlags);
    //      DEBUG_PRINT("0-irq_flag=0x%X\r\n", irq_flag);
    //    DEBUG_PRINT("1-irq_flag=0x%X\r\n", SX1278_Read_Reg(LR_RegIrqFlags));

    if ((irq_flag & RFLR_IRQFLAGS_RXDONE) == RFLR_IRQFLAGS_RXDONE)
    {
      if ((irq_flag & RFLR_IRQFLAGS_PAYLOADCRCERROR) == RFLR_IRQFLAGS_PAYLOADCRCERROR) // 如果是CRC校验错误中断
      {
        SX1278_LoRaClearIrq();
        // SX1278_Sleep();
        return (2);
      }

      addr = SX1278_Read_Reg(LR_RegFifoRxCurrentaddr); // last packet addr
      DEBUG_PRINT("addr  %d\r\n", addr);
      SX1278_Write_Reg(LR_RegFifoAddrPtr, addr);      // RxBaseAddr -> FiFoAddrPtr
      packet_size = SX1278_Read_Reg(LR_RegRxNbBytes); // Number for received bytes



      // 记录当前有效数据的长度
      u8 current_length = lora_receive_len;

      if ((current_length + packet_size) >= sizeof(lora_receive_buf)) // 超过200重头计数
      {
        lora_receive_len = 1;
        current_length = 0;
      }

      // 从FIFO中读取数据到valid_data数组中放尾巴一直累加
      // SX1278_Burst_Read(0x00, valid_data + current_length, packet_size);

      SX1278_Burst_Read(0x00, temp_data, packet_size); // 将数据读取放这里面

      SX1278_LoRaClearIrq();
      // SX1278_Sleep(); // 进入睡眠模式

      // 检查收到的数据是否为 "**"
      //      DEBUG_PRINT(" packet_size: %d\r\n", packet_size);
      //      DEBUG_PRINT("  valid_data[current_length]: %d\r\n", valid_data[current_length]);
      if (packet_size == 2 && temp_data[packet_size - 1] == '*' && temp_data[packet_size - 2] == '*')
      {

        lora_receive_flag = 1;
        return 3; // 表示收到对方回应，不回复
      }
      else // 如果不是 "**"，则发送 "**" 表示收到
      {

        if (temp_data[packet_size - 1] != '*')
        {
          DEBUG_PRINT(" packet_size: %d\r\n", packet_size);
          DEBUG_PRINT("illegal msg=%c\r\n", temp_data[packet_size - 1]);
          DEBUG_PRINT("illegal msg=%c\r\n", temp_data[packet_size - 2]);
          DEBUG_PRINT("illegal msg=%c\r\n", temp_data[0]);
          lora_receive_len = 1;
          lora_receive_flag = 3;
          memset(valid_data, '\0', sizeof(lora_receive_buf));
          return 4;
        }
        else // 合法消息回应一下
        {

          if (!SX1278_LoRaTxPacket("*", 1))
          {

            DEBUG_PRINT("mode=0x%X \r\n", SX1278_Read_Reg(LR_RegOpMode)); // 0x88 1000 1000
            DEBUG_PRINT("Lora send ok\r\n");
            SX1278_LoRaEntryRx(); // 进入接收模式
          }
          else
          {

            DEBUG_PRINT("mode=0x%X \r\n", SX1278_Read_Reg(LR_RegOpMode)); // 0x8b 1000 1010

            DEBUG_PRINT("Lora send fail\r\n");
            SX1278_LoRaEntryRx(); // 进入接收模式
          }
          // 拷贝完整的数据
          memcpy(valid_data, temp_data, packet_size);
          lora_receive_len = packet_size;
          DEBUG_PRINT("Received my msg\r\n");
          lora_receive_flag = 3;
        }
      }

      return 0;
    }
    else
    {
      if ((irq_flag & RFLR_IRQFLAGS_TXDONE) == RFLR_IRQFLAGS_TXDONE)
      {
        DEBUG_PRINT("send ok irq_flag\r\n");
      }
      else
      {
        DEBUG_PRINT("2-irq_flag=0x%X\r\n", SX1278_Read_Reg(LR_RegIrqFlags));

      }
    }
  }

  // SX1278_Sleep(); // 进入睡眠模式
  return (1);
}

/*************************************************************************
 功能：进入发送模式
 参数：无
 返回：无
***************************************************************************/
u8 SX1278_LoRaEntryTx(u8 packet_length)
{
  u8 addr = 0, temp;
  u8 t = 3;
  SX1278_Standby();                           // 进入待机模式从待机模式进入其他模式
  SX1278_Write_Reg(REG_LR_PADAC, 0x87);       // Tx for 20dBm发送时需要设置的值
  SX1278_Write_Reg(LR_RegHopPeriod, 0x00);    // 发送模式跳频周期写0X00不跳频
  SX1278_Write_Reg(REG_LR_DIOMAPPING1, 0x41); // DIO0=01, DIO1=00, DIO2=00, DIO3=01

  SX1278_Write_Reg(LR_RegIrqFlagsMask, 0xF7); // Open TxDone interrupt
  SX1278_LoRaClearIrq();

  SX1278_Write_Reg(LR_RegPayloadLength, packet_length);

  addr = SX1278_Read_Reg(LR_RegFifoTxBaseAddr); // RegFiFoTxBaseAddr
  SX1278_Write_Reg(LR_RegFifoAddrPtr, addr);    // RegFifoAddrPtr
  while (t--)
  {
    temp = SX1278_Read_Reg(LR_RegPayloadLength);
    if (temp == packet_length)
    {
      // DEBUG_PRINT("SX1278_LoRaEntryTx=OK\r\n");
      break;
    }
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
  //  u8 timeout = 255;
  u8 temp_data[54];
  u8 temp_packet_length = packet_length;
  // 将原始数据复制到临时缓冲区
  memcpy(temp_data, valid_data, packet_length);

  // 在临时缓冲区末尾添加 '*' 字符
  temp_data[packet_length] = '*';
  temp_packet_length++; // 增加临时数据包长度以包含添加的 '*' 字符
  int t;
  for (t = 0; t < temp_packet_length; t++)
  {
    DEBUG_PRINT("sss=%c ", temp_data[t]);
    // DEBUG_PRINT("%d=%c ", t, temp_data[t]);
  }
  DEBUG_PRINT("\r\n");
  SX1278_LoRaEntryTx(temp_packet_length); // 进入待机模式
  SX1278_Burst_Write(0x00, temp_data, temp_packet_length);
  SX1278_Write_Reg(LR_RegOpMode, 0x8b); // Tx Mode 1000 1011  lora模式，tx模式

  while (SX1278_Read_Reg(LR_RegOpMode) != 0x88) // 1000 1000 tx完成
  {

    if (READ_SX1278_NIRQ()) // Packet send over
    {
      // DEBUG_PRINT("1SX1278_TX_NIRQ%d\r\n",SX1278_Read_Reg(LR_RegIrqFlags));
      SX1278_Read_Reg(LR_RegIrqFlags);
      SX1278_LoRaClearIrq(); // Clear irq
                             // DEBUG_PRINT("2SX1278_TX_NIRQ%d\r\n",SX1278_Read_Reg(LR_RegIrqFlags));
      //			SX1278_Standby();                                     //Entry Standby mode
      SX1278_Sleep(); // 进入睡眠模式
      return 0;
    }
  }

  DEBUG_PRINT("sb\r\n");
  SX1278_Sleep(); // 进入睡眠模式
  return 1;
}

/*************************************************************************
 功能：获取信号强度
 参数：无
 返回：信号强度 127最大
***************************************************************************/
u8 SX1278_ReadRSSI(void)
{
  u8 temp = 0xff;

  temp = SX1278_Read_Reg(0x11);
  temp >>= 1;
  temp = 127 - temp; // 127:Max RSSI
  return temp;
}
/*************************************************************************
 功能：模块初始化
 参数：freq[in]: 载波频率设置 428 ~ 439 单位MHz
 返回：无
***************************************************************************/
void SX1278_Init()
{
  //  SX1278_GPIO_Init();
  //  SX1278_SPI_Init(); // SPI初始化
  //       SX1278_NSS_DISABLE;
  //       SET_SX1278_RST();
  //       SET_SX1278_MOSI();
  //       SET_SX1278_SCK();
  // if (freq >= 428 && freq <= 439) // 在设置范围内，不然就按照默认频率设置
  // {
  //   Lora_Freq = freq % 428; // 设置载波频率
  // }
  CLR_SX1278_RST();
  SX1278_DelayMs(100);
  SET_SX1278_RST();
  SX1278_DelayMs(10); // 复位操作

  SX1278_Config();

  while (SX1278_Read_Reg(REG_LR_VERSION) != 0x12)
  {
    DEBUG_PRINT("wait sx1278 init \r\n");
    Delay_Ms(10);
  }
  DEBUG_PRINT("lora init ok=0x%X \r\n", SX1278_Read_Reg(REG_LR_VERSION)); // 0x89  0100 0101
}

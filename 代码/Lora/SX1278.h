/*****************************************************************************************************
 		  申明： 此处使用的是模拟SPI方式驱动的，模拟的就好，使用硬件SPI毫无意义
      完成日期： 2017-06-29
  最后修改日期：	 
 		  作者：  肖川 
          功能：	 用于汇睿微通SX1278模块驱动，可变长度数据发送(少许障碍，测试距离1.5KM可正常接收)

 	 修改说明：	 
                 
*****************************************************************************************************/
/************************************************
//  RF module:           sx1276_7_8
//  LoRa:
//  Carry Frequency:     434MHz
//  Spreading Factor:    7/8/9/10/11/12
//  Tx Power Output:     20dbm/17dbm/14dbm/11dbm
//  Receive Bandwidth:   7.8KHz/10.4KHz/15.6KHz/20.8KHz/31.2KHz/41.7KHz/62.5KHz/125KHz/250KHz/500KHz
//  Coding:              NRZ
//  Tx Current:          about 120mA  (RFOP=+20dBm,typ.)
//  Rx Current:          about 11.5mA  (typ.)  
//  Packet_Length:       variable（可变长度的，最大256字节）
**********************************************************/
#ifndef SX1278_H
#define SX1278_H

#include "debug.h"
#include "gpio.h"

//配置参考定义 
/********************Laro***************************/
//Error Coding 表
#define ERROR_CODING_4_5    0x01
#define ERROR_CODING_4_6    0x02
#define ERROR_CODING_4_7    0x03
#define ERROR_CODING_4_8    0x04


//载波频率表
#define LORAFREQ_428MHZ LORAFREQ_433MHZ-4
#define LORAFREQ_429MHZ LORAFREQ_433MHZ-3
#define LORAFREQ_430MHZ LORAFREQ_433MHZ-3
#define LORAFREQ_431MHZ LORAFREQ_433MHZ-2
#define LORAFREQ_432MHZ LORAFREQ_433MHZ-1
#define LORAFREQ_433MHZ 5
#define LORAFREQ_434MHZ LORAFREQ_433MHZ+1
#define LORAFREQ_435MHZ LORAFREQ_433MHZ+2
#define LORAFREQ_436MHZ LORAFREQ_433MHZ+3
#define LORAFREQ_437MHZ LORAFREQ_433MHZ+4
#define LORAFREQ_438MHZ LORAFREQ_433MHZ+5
#define LORAFREQ_439MHZ LORAFREQ_433MHZ+6


//发射功率定义
#define LORAPOWER_20DBM 0xFF
#define LORAPOWER_17DBM 0xFC
#define LORAPOWER_14DBM 0xF9
#define LORAPOWER_11DBM 0xF6

//带宽表定义
#define LORABW_7_8KHz  0
#define LORABW_10_4KHz 1
#define LORABW_15_6KHz 2
#define LORABW_20_8KHz 3
#define LORABW_31_2KHz 4
#define LORABW_41_7KHz 5
#define LORABW_62_5KHz 6
#define LORABW_125KHz  7
#define LORABW_250KHz  8
#define LORABW_500KHz  9
/********************************************************/

 
#define LR_RegFifo                                  0x00 //FIFO读写访问，当器件处于睡眠模式时，FIFO被清零，无法访问
// Common settings
#define LR_RegOpMode                                0x01 //运行模式 Lora和FSK选择
#define LR_RegFrMsb                                 0x06 //RF载波频率最高有效位
#define LR_RegFrMid                                 0x07 //RF载波频率中间有效位
#define LR_RegFrLsb                                 0x08 //RF载波频率最低有效位  若F(XOSC)=32MHz，则分辨率为61.035Hz。
// Tx settings 发送设置
#define LR_RegPaConfig                              0x09 //PA选择和输出功率控制
#define LR_RegPaRamp                                0x0A //PA斜升/斜将时间和低相噪PLL的控制
#define LR_RegOcp                                   0x0B //过流保护控制
// Rx settings 接收设置
#define LR_RegLna                                   0x0C //LNA设置
// LoRa registers
#define LR_RegFifoAddrPtr                           0x0D //FIFO数据缓冲区中SPI接口地址指针
#define LR_RegFifoTxBaseAddr                        0x0E //FIFO数据缓冲区发送调制器的写入基地址
#define LR_RegFifoRxBaseAddr                        0x0F //FIFO数据缓冲区接收调制器的读取基地址
#define LR_RegFifoRxCurrentaddr                     0x10 //接收到最后一个数据包的起始地址
#define LR_RegIrqFlagsMask                          0x11 //可选中断标志屏蔽
#define LR_RegIrqFlags                              0x12 //中断标志
#define LR_RegRxNbBytes                             0x13 //最后一次接收到的数据包的负载字节数
#define LR_RegRxHeaderCntValueMsb                   0x14 //最后一次转换至Rx模式后接收的有效报头数（高位有效）
#define LR_RegRxHeaderCntValueLsb                   0x15 //最后一次转换至Rx模式后接收的有效报头数（低位有效）
#define LR_RegRxPacketCntValueMsb                   0x16 //最后一次转换至Rx模式后接收的有效数据包数（高位有效）
#define LR_RegRxPacketCntValueLsb                   0x17 //最后一次转换至Rx模式后接收的有效数据包数（低位有效）
#define LR_RegModemStat                             0x18 //LoRa调制解调器现场状态
#define LR_RegPktSnrValue                           0x19 //最后接收到的数据包的SNR(信噪比)预估值
#define LR_RegPktRssiValue                          0x1A //最后接收到的数据包的RSSI（dBm） RSSI[dBm]=-137+PacketRssi
#define LR_RegRssiValue                             0x1B //电流RSSI值（dBm）RSSI[dBm]=-137+Rssi 
#define LR_RegHopChannel                            0x1C //FHSS起始信道
#define LR_RegModemConfig1                          0x1D //调制解调器物理层配置1:7-4位信号带宽(0x07默认125kHz) 3-1位纠错编码率(默认001->4/5) 0位报头模式(0 0->显式报头模式 )
#define LR_RegModemConfig2                          0x1E //调制解调器物理层配置2:7-4位SF值(以 2 基对数表示 默认0x07->128 码片/符号 ) 3位(0正常模式发送单个数据包 1持续模式通过FIFO发送多个数据包(用于频谱分析)) 2位发送端CRC信息(0关闭CRC 1开启CRC) 1-0位RX超时最高有效位
#define LR_RegSymbTimeoutLsb                        0x1F //接收机超时值,RX超时最低有效位RX操作超时值以符号数表示：TimeOut=SymbTimeout-Ts 
#define LR_RegPreambleMsb                           0x20 //前导码长度（最高位）=PreambleLength+4.25 符号
#define LR_RegPreambleLsb                           0x21 //前导码长度（最低位）
#define LR_RegPayloadLength                         0x22 //LoRa负载长度:隐式报头模式下 需要设置寄存器，以达到预期的 数据包长度。不允许将寄存器值 设置为 0。 
#define LR_RegMaxPayloadLength                      0x23 //LoRa负载长度最大值:如果报头负载 长度超过该最大值，则会产生报 头 CRC 错误。允许对长度不正 确的数据包进行过滤。
#define LR_RegHopPeriod                             0x24 //FHSS跳频周期:（0= 关闭）。第一跳总是发生在第一个 报头符号后
#define LR_RegFifoRxByteAddr                        0x25 //FIFO中最后写入字节的地址,接收数据缓存当前指针（由 Lora 接收机写入的最后一个字节的地址） 
#define LR_RegModemConfig3                          0x26 //调制解调器物理层配置3
 
#define LR_RegPacketConfig2                         0x31 //RESERVED
#define LR_RegSeqConfig2                            0x37 //RESERVED
 
#define REG_LR_DIOMAPPING1                          0x40 //DIO0到DIO3引脚映射
#define REG_LR_DIOMAPPING2                          0x41 //DIO4到DIO5引脚映射、ClkOut频率
// Version
#define REG_LR_VERSION                              0x42 //芯片版本
// Additional settings
#define REG_LR_PLLHOP                               0x44 //unused
#define REG_LR_TCXO                                 0x4B //TCXO或XTAL输入设置
#define REG_LR_PADAC                                0x4D //PA更大功率设置 输出20dBm
#define REG_LR_FORMERTEMP                           0x5B //IQ校准期间存储温度
#define REG_LR_BITRATEFRAC                          0x5D
#define REG_LR_AGCREF                               0x61 //AGC阈值调整
#define REG_LR_AGCTHRESH1                           0x62 //AGC阈值调整
#define REG_LR_AGCTHRESH2                           0x63 //AGC阈值调整
#define REG_LR_AGCTHRESH3                           0x64 //AGC阈值调整
#define REG_LR_PLL                                  0x70 //PLL带宽控制





/*!
 * RegIrqFlags
 */
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80 
#define RFLR_IRQFLAGS_RXDONE                        0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20 
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10 
#define RFLR_IRQFLAGS_TXDONE                        0x08 
#define RFLR_IRQFLAGS_CADDONE                       0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02 
#define RFLR_IRQFLAGS_CADDETECTED                   0x01 

/********************FSK/ook mode***************************/
#define  RegFIFO  	 			0x00				//FIFO
#define  RegOpMode 	 			0x01				//操作模式
#define  RegBitRateMsb 		0x02				//BR 高位
#define  RegBitRateLsb 		0x03				//BR 低位
#define  RegFdevMsb	 			0x04				//FD 高位
#define  RegFdevLsb	 			0x05 				//FD 低位
#define  RegFreqMsb	 			0x06				//Freq 高位
#define  RegFreqMid	 			0x07 				//Freq 中位
#define  RegFreqLsb   		0x08				//Freq 低位
#define	 RegPaConfig			0x09
#define  RegPaRamp				0x0a
#define  RegOcp						0x0b
#define  RegLna						0x0c
#define  RegRxConfig			0x0d
#define  RegRssiConfig		0x0e
#define  RegRssiCollision 0x0f
#define  RegRssiThresh		0x10
#define  RegRssiValue			0x11
#define  RegRxBw					0x12
#define  RegAfcBw					0x13
#define  RegOokPeak				0x14
#define  RegOokFix				0x15
#define  RegOokAvg				0x16

#define  RegAfcFei				0x1a	
#define  RegAfcMsb				0x1b
#define  RegAfcLsb				0x1c
#define  RegFeiMsb				0x1d
#define  RegFeiLsb				0x1e
#define  RegPreambleDetect	0x1f
#define  RegRxTimeout1		0x20
#define  RegRxTimeout2		0x21
#define  RegRxTimeout3		0x22
#define  RegRxDelay		    0x23
#define  RegOsc  	 		0x24				//OSC设置
#define  RegPreambleMsb		0x25
#define  RegPreambleLsb		0x26
#define  RegSyncConfig		0x27
#define  RegSyncValue1		0x28
#define  RegSyncValue2		0x29
#define  RegSyncValue3		0x2a
#define  RegSyncValue4		0x2b
#define  RegSyncValue5		0x2c
#define  RegSyncValue6		0x2d
#define  RegSyncValue7		0x2e
#define  RegSyncValue8		0x2f
#define  RegPacketConfig1		0x30
#define  RegPacketConfig2		0x31
#define  RegPayloadLength		0x32
#define  RegNodeAdrs			0x33
#define  RegBroadcastAdrs		0x34
#define  RegFifoThresh		0x35
#define  RegSeqConfig1		0x36
#define  RegSeqConfig2		0x37
#define  RegTimerResol		0x38
#define  RegTimer1Coef		0x39
#define  RegTimer2Coef		0x3a
#define  RegImageCal			0x3b
#define  RegTemp			    0x3c
#define  RegLowBat				0x3d
#define  RegIrqFlags1			0x3e
#define  RegIrqFlags2			0x3f
#define  RegDioMapping1		0x40
#define  RegDioMapping2		0x41
#define  RegVersion				0x42

#define	 RegPllHop				0x44
#define  RegPaDac					0x4d
#define	 RegBitRateFrac		0x5d
/*********************************************************/
//command
/*********************************************************/
void SX1278_LoRaClearIrq(void);
void SX1278_Sleep(void);
void SX1278_Burst_Read(u8 adr, u8 *ptr, u8 length);
u8 SX1278_LoRaEntryRx(void);

u8 SX1278_Read_Reg(u8 adr);
void SX1278_Standby(void);
u8 SX1278_SPI_RW(u8 byte);
extern void SX1278_Init();
//extern void SX1278_Config(void);
//extern u8   SX1278_LoRaEntryRx(void); 
void SX1278_Write_Reg(u8 adr, u8 WrPara);
extern u8   SX1278_LoRaReadRSSI(void);
u8 SX1278_ReadRSSI(void);
extern u8 SX1278_LoRaRxPacket(u8 *valid_data, u16 timeout);
//extern u8   SX1278_LoRaEntryTx(u8 packet_length);
extern u8   SX1278_LoRaTxPacket(u8 *valid_data, u8 packet_length);
void SX1278_Receive() ;


#endif /* SX1278_H */

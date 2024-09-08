/*
 * gpio.c
 *
 *  Created on: 2024��9��1��
 *      Author: 12630
 */

#include "gpio.h"
void My_GPIO_Init(){

    GPIO_InitTypeDef GPIO_InitStructure={0};
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD , ENABLE );


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz; // GPIO �ٶ�����Ϊ 30MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);//pd6��lora���жϽ���



    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );//pc1��lora�ĸ�λ

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );//pc0��lora��nss



    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );//pA2��ģ���


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );//pA1��ص����ɼ�


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );//pc3�����pwm



    //LCD_DC_1   LCD_CS_1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �����������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz; // GPIO �ٶ�����Ϊ 30MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure); // ��ʼ�� GPIOC

    //LCD_0IN85_RST_1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz; // GPIO �ٶ�����Ϊ 30MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    

    //  LCD_SCK_1;  SPI_SDA_1;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    //���ź�loraͨ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //��������io��d3d4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);





}

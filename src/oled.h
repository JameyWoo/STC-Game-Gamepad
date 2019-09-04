#ifndef	__OLED_H
#define	__OLED_H

#define high 1
#define low 0 

/*************Pin Define***************/
sbit SCL=P1^0;
sbit SDA=P1^1;


void Initial_M096128x64_ssd1306();
void Delay_50ms(unsigned int Del_50ms);
void Delay_1ms(unsigned int Del_1ms);
void fill_picture(unsigned char fill_Data);
void Picture();
void IIC_Start();
void IIC_Stop();
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);

/*??????*/
void showMap();  // ??????
void showDot(unsigned char, unsigned char);  // ???4?????
char checkDot(unsigned char, unsigned char);  // ??????????????


#endif
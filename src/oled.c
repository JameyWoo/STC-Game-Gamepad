#include <STC15F2K60S2.H>
#include "oled.h"
#include "picture.h"


/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{
   SCL = high;		
   SDA = high;
   SDA = low;
   SCL = low;
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
   SCL = low;
   SDA = low;
   SCL = high;
   SDA = high;
}
/**********************************************
// IIC Write byte
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	for(i=0;i<8;i++)		
	{
		if(IIC_Byte & 0x80)
		SDA=high;
		else
		SDA=low;
		SCL=high;
		SCL=low;
		IIC_Byte<<=1;
	}
	SDA=1;
	SCL=1;
	SCL=0;
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
   IIC_Start();
   Write_IIC_Byte(0x78);            //Slave address,SA0=0
   Write_IIC_Byte(0x00);			//write command
   Write_IIC_Byte(IIC_Command); 
   IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
   Write_IIC_Byte(0x40);			//write data
   Write_IIC_Byte(IIC_Data);
   IIC_Stop();
}
/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		Write_IIC_Command(0xb0+m);		//page0-page1
		Write_IIC_Command(0x02);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				Write_IIC_Data(fill_Data);
			}
	}
}
/******************************************
// picture用来显示一个图片
******************************************/
void Picture()
{
  unsigned char x,y;
  unsigned int i=0;
  for(y=0;y<8;y++)
    {
      Write_IIC_Command(0xb0+y);
      Write_IIC_Command(0x02);
      Write_IIC_Command(0x10);
      for(x=0;x<128;x++)
        {
		//	Write_IIC_Data(maze_map[i++]);
           Write_IIC_Data(show[i++]);
			// Write_IIC_Data(gImage_me[i++]);
			// Write_IIC_Data(hehe[i++]);
        }
    }
}

/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
	unsigned int m;
	for(;Del_50ms>0;Del_50ms--)
		for(m=6245;m>0;m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
	unsigned char j;
	while(Del_1ms--)
	{	
		for(j=0;j<123;j++);
	}
}



void Initial_M096128x64_ssd1306()
{
	Delay_50ms(5);
	Write_IIC_Command(0xAE);   //display off
	Write_IIC_Command(0x02);	//Set Memory Addressing Mode	
	Write_IIC_Command(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	Write_IIC_Command(0x40);	//Set Page Start Address for Page Addressing Mode,0-7
	Write_IIC_Command(0xb0);	//Set COM Output Scan Direction
	Write_IIC_Command(0x81);//---set low column address
	Write_IIC_Command(0xff);//---set high column address
	Write_IIC_Command(0xa1);//--set start line address
	Write_IIC_Command(0xa6);//--set contrast control register
	Write_IIC_Command(0xa8);
	Write_IIC_Command(0x3f);//--set segment re-map 0 to 127
	Write_IIC_Command(0xad);//--set normal display
	Write_IIC_Command(0x8b);//--set multiplex ratio(1 to 64)
	Write_IIC_Command(0x33);//
	Write_IIC_Command(0xc8);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Write_IIC_Command(0xd3);//-set display offset
	Write_IIC_Command(0x00);//-not offset
	Write_IIC_Command(0xd5);//--set display clock divide ratio/oscillator frequency
	Write_IIC_Command(0x80);//--set divide ratio
	Write_IIC_Command(0xd9);//--set pre-charge period
	Write_IIC_Command(0x1f); //
	Write_IIC_Command(0xda);//--set com pins hardware configuration
	Write_IIC_Command(0x12);
	Write_IIC_Command(0xdb);//--set vcomh
	Write_IIC_Command(0x40);//0x20,0.77xVcc
//	Write_IIC_Command(0x8d);//--set DC-DC enable
//	Write_IIC_Command(0x14);//
	Write_IIC_Command(0xaf);//--turn on oled panel 2
}

/* 我的函数 */
void showMap() {
	unsigned char x,y;
	unsigned int i=0;
	for(y=0;y<8;y++) {
		Write_IIC_Command(0xb0+y);
		Write_IIC_Command(0x02);
		Write_IIC_Command(0x10);
		for(x=0;x<128;x++){
			//	Write_IIC_Data(maze_map[i++]);
			Write_IIC_Data(maze_map_1[i++]);
				// Write_IIC_Data(gImage_me[i++]);
				// Write_IIC_Data(hehe[i++]);
		}
	}
}

void showDot(unsigned char maze_x, unsigned char maze_y) {
	int loc = 128*(maze_x/8) + maze_y;
	int loc2 = loc + 1;

	unsigned char x,y;
	unsigned int i=0;
	for(y=0;y<8;y++) {
		Write_IIC_Command(0xb0+y);
		Write_IIC_Command(0x02);
		Write_IIC_Command(0x10);
		for(x=0;x<128;x++) {
			//	Write_IIC_Data(maze_map[i++]);
			if (i != loc && i != loc2)
				Write_IIC_Data(maze_map_1[i++]);
			else {
				Write_IIC_Data(maze_map_1[i] ^ ((1 << maze_x%8) | (1 << (maze_x + 1)%8)));
				i++;
			}
				// Write_IIC_Data(gImage_me[i++]);
				// Write_IIC_Data(hehe[i++]);
		}
	}
}

char checkDot(unsigned char x, unsigned char y) {
	int loc = 128*(x/8) + y;
	// P0 = maze_map_1[loc] ;
	P0 = (1 << (x%8));
	return (maze_map_1[loc] & (1 << (x%8)));
}

void swap_x_y(unsigned char* x, unsigned char* y) {
	unsigned char tmp = *x;
	*x = *y;
	*y = tmp;
}
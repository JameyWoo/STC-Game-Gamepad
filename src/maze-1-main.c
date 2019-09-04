/**********************
myNavKey 导航按键+数字按键控制数码管
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#include <intrins.h>
#include <stdio.h>
// #include "storage.c"
#include "oled.h"
#define uint  unsigned int
#define uchar unsigned char

/*---------宏定义---------*/
#define cstAdcPower 0X80     /*ADC电源开关*/
#define cstAdcFlag 0X10      /*当A/D转换完成后，cstAdcFlag要软件清零*/
#define cstAdcStart 0X08     /*当A/D转换完成后，cstAdcStart会自动清零，所以要开始下一次转换，则需要置位*/
#define cstAdcSpeed90 0X60   /*ADC转换速度 90个时钟周期转换一次*/
#define cstAdcChs17 0X07     /*选择P1.7作为A/D输入*/
#define cstFocs      11059200L  //晶振频率 11059200L
#define cstBaud1     9600       //波特率
#define cstKeyMaxNum 100        //按键抖动次数

/*---------引脚别名定义---------*/
sbit sbtLedSel = P2 ^ 3;     /*数码管和发光二极管选择位*/
sbit sbtBeep = P3 ^ 4;                  //蜂鸣器引脚
sbit Key2 = P3 ^ 3;  // 按下key2, 切换是否开启蜂鸣器
sbit Key1 = P3 ^ 2;  // 按下Key1, 切换模式

/*---------变量定义---------*/
uchar ucSegSelectState;         /*段选标志*/
bit  btBeepFlag;                //控制蜂鸣器开关的标志位
uint cnt_sound = 0;  // 声音计次
bit beepFlag = 1;  // key3蜂鸣器开关
uint display_data_test = 0xff;
uint cnt_led = 0;  // 切换数码管和led灯的计数
uint cnt_moves = 0;
int s;  // 迷宫碰到障碍时的声音循环数
uchar maze_x, maze_y;  // 地图中人物的位置
uchar last_check = 0x81;
bit mode = 0;  // 迷宫游戏模式0, 串口手柄模式1;

uchar duan[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71}; //数码管段选，显示0-f
uchar wei[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; //数码管位选
uchar wei_8;

/*收发显示数据相关*/
bit btUart1SendBusy = 0 ;
uchar ucDateTmp;               //传输数据暂存

/*---------串口1初始化函数--------*/
void Uart1_Init( void )
{
    AUXR = 0X80; //辅助寄存器 此时定时器0的速度是传统的12倍，不分频
    SCON |= 0X50; //允许接收
    TL1 = ( 65536 - ( cstFocs / 4 / cstBaud1 ) );
    TH1 = ( 65536 - ( cstFocs / 4 / cstBaud1 ) ) >> 8;
    AUXR |= 0X40; //辅助寄存器 此时定时器1的速度是传统的12倍，不分频
    RI = 0;      //接收中断标志位
    TI = 0;      //发送中断标志位
    TR1 = 1;     //启动定时器1
    ES = 1;      //串口中断允许位
    EA = 1;      //总中断允许位
    PS = 1 ;    //串口1中断高优先级
}

/*---------初始化函数--------*/
void Init()
{
    P0M1 = 0x00;
    P0M0 = 0xff;
    P2M1 = 0x00;
    P2M0 = 0xff;

    P1ASF = 0x80;       //P1.7作为模拟功能A/D使用
    ADC_RES = 0;        //转换结果清零
    ADC_CONTR = 0x8F;   //cstAdcPower = 1
    CLK_DIV = 0X00;     //ADRJ = 0    ADC_RES存放高八位结果

    IT0 = 0;            //设置IT0上升沿触发
    IT1 = 0;

    AUXR |= 0x80;                       //定时器时钟1T模式
    TMOD &= 0xF0;                       //设置定时器模式为16位自动重装
    // TL0 = 0xCD;                         //设置定时初值
    // TH0 = 0xF4;                         //设置定时初值
    TF0 = 0;                            //清除TF0标志
    TR0 = 1;                            //定时器0开始计时

    Uart1_Init();   //外部中断：低优先级

    btBeepFlag = 0;
    P0 = 0x00;                          //数码管和LED显示清零

    sbtBeep = 0;                        //蜂鸣器引脚置0，以保护蜂鸣器
    ET0 = 1;
    EA = 1;

    sbtLedSel = 0;  // 用数码管显示
}

/*---------发送数据函数--------*/
void SendData( unsigned char dat )
{
    while( btUart1SendBusy ); //发送单个字符给UART1以发送到PC机
    btUart1SendBusy = 1;
    SBUF = dat;
}

/*---------串口1中断处理函数--------*/
void Uart1_Process() interrupt 4 using 1
{
    if( RI )         //接受完数据后 RI自动制1
    {
        RI = 0;   ucDateTmp = SBUF;
    }
    if( TI )        //发送完数据后 RI自动制1
    {
        TI = 0;   btUart1SendBusy = 0;
    }
}

/* 数码管显示两个数据, 一个数字*/
void display(uint data1, uint data2) {
    wei_8++;
    if (wei_8 == 8) wei_8 = 0;
    P0 = 0x00;
    P2 = wei[wei_8];
    switch (wei_8) {
        case 0:
            P0 = duan[(data1 / 1000) % 10]; break;
        case 1:
            P0 = duan[(data1 / 100) % 10]; break;
        case 2:
            P0 = duan[(data1 / 10) % 10]; break;
        case 3:
            P0 = duan[data1 % 10]; break;
        case 4:
            P0 = duan[(data2 / 1000) % 10]; break;
        case 5:
            P0 = duan[(data2 / 100) % 10]; break;
        case 6:
            P0 = duan[(data2 / 10) % 10]; break;
        case 7:
            P0 = duan[data2 % 10]; break;
    }
}

/*---------延时5ms子函数--------*/
void Delay5ms()     //@11.0592MHz  延时5ms
{
    unsigned char i, j;
    i = 54;
    j = 199;
    do
    {
        while ( --j );
    }
    while ( --i );
}

/*---------延时100ms子函数--------*/
void Delay100ms()       //@11.0592MHz  延时100ms
{
    unsigned char i, j, k;
    _nop_();
    _nop_();
    i = 5;
    j = 52;
    k = 195;
    do
    {
        do
        {
            while ( --k );
        }
        while ( --j );
    }
    while ( --i );
}

/*---------延时子函数--------*/
void DelayMs( uint xms )  // 1ms
{
    uchar i;
    for( ; xms > 0; xms-- )
        for( i = 114; i > 0; i-- )
        {
            ;
        }
}

/*---------获取AD值子函数--------*/
unsigned char GetADC()
{
    uchar ucAdcRes;
    ADC_CONTR = cstAdcPower | cstAdcStart | cstAdcSpeed90 | cstAdcChs17;//没有将cstAdcFlag置1，用于判断A/D是否结束
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    while( !( ADC_CONTR & cstAdcFlag ) ); //等待直到A/D转换结束
    ADC_CONTR &= ~cstAdcFlag;           //cstAdcFlagE软件清0
    ucAdcRes = ADC_RES;                 //获取AD的值
    return ucAdcRes;
}

/*---------获取导航按键值子函数--------*/
uchar NavKeyCheck()
{
    unsigned char key;
    key = GetADC();     //获取AD的值
    if( key != 255 )    //有按键按下时
    {
        Delay5ms();
        key = GetADC();
        if( key != 255 )            //按键消抖 仍有按键按下
        {
            key = key & 0xE0;       //获取高3位，其他位清零
            key = _cror_( key, 5 ); //循环右移5位 获取A/D转换高三位值，减小误差
            return key;
        }
    }
    return 0x07;        //没有按键按下时返回值0x07
}

void MakeSound(uint num) {
    if (beepFlag == 1) {
        btBeepFlag = 1;
        TH0 = 0xF4 - num;
    }
}

/*---------导航按键处理子函数--------*/
void NavKey_Process()
{
    uchar ucNavKeyCurrent;  //导航按键当前的状态
    uchar ucNavKeyPast;     //导航按键前一个状态

    ucNavKeyCurrent = NavKeyCheck();    //获取当前ADC值
    if( ucNavKeyCurrent != 0x07 )       //导航按键是否被按下 不等于0x07表示有按下
    {
        ucNavKeyPast = ucNavKeyCurrent;
        while( ucNavKeyCurrent != 0x07 )        //等待导航按键松开
            ucNavKeyCurrent = NavKeyCheck();

        MakeSound(0);
        display_data_test = ucNavKeyPast;
        SendData( ucNavKeyPast ) ;
        cnt_moves++;
        switch( ucNavKeyPast )
        {
            case 0x05 :                     //上键：显示的数字加1
                last_check = checkDot(maze_x - 2, maze_y);
                if (last_check == 0) {
                    last_check = 0xf2;
                    maze_x -= 2;
                } else {
                    for (s = 0; s < 3; ++s) {
                        if (mode == 0)
                        MakeSound(s);
                        DelayMs(3000);
                    }
                }
                if( ucSegSelectState == 31 )
                {
                    ucSegSelectState = 0;
                }
                else
                    ucSegSelectState++;
                break;
            case 0x02 :                     //下键：显示的数字减1
                last_check = checkDot(maze_x + 2, maze_y);
                if (last_check == 0) {
                    last_check = 0xf3;
                    maze_x += 2;
                } else {
                    for (s = 0; s < 3; ++s) {
                        if (mode == 0)
                        MakeSound(s);
                        DelayMs(3000);
                    }
                }
                if( ucSegSelectState == 0 )
                    ucSegSelectState = 31;
                else
                    ucSegSelectState--;
                break;
            case 0x04:
                last_check = checkDot(maze_x, maze_y - 2);
                if (last_check == 0) {
                    last_check = 0xf4;
                    maze_y -= 2;
                } else {
                    
                    for (s = 0; s < 3; ++s) {
                        if (mode == 0)
                        MakeSound(s);
                        DelayMs(3000);
                    }
                }
                ucSegSelectState *= 2;
                break;
            case 0x01:
                last_check = checkDot(maze_x, maze_y + 2);  // 右
                if (last_check == 0) {
                    last_check = 0xf1;
                    maze_y += 2;
                } else {
                    for (s = 0; s < 3; ++s) {
                        if (mode == 0)
                        MakeSound(s);
                        DelayMs(3000);
                    }
                }
                // 似乎要判断是否等于某个数, 而不是直接把函数作为条件判断?!
                // if (checkDot(maze_x, maze_y + 2)) {

                // } else maze_y += 2;
                // maze_y += 2;
                ucSegSelectState /= 2;
                break;
        }
    }
    if (maze_x == 60 && maze_y == 126) {
        for (s = 0; s < 10; ++s) {
            MakeSound(s);
            DelayMs(5000);
        }
        maze_y += 2;
    }
    P0 = ucSegSelectState;
    // P0 = beepFlag;

    Delay100ms();
}

/*---------T0定时器中断服务处理函数--------*/
void T0_Process() interrupt 1
{
    if( btBeepFlag)
    {
        sbtBeep = ~sbtBeep;            //产生方波使得蜂鸣器发声
    }
    else
        sbtBeep = 0;                  //如果开关关闭,则蜂鸣器断电以保护蜂鸣器
    cnt_sound++;
    if (cnt_sound == 444) {
        btBeepFlag = 0;
        cnt_sound = 0;
    }

    cnt_led++;
    if (cnt_led >= 10) {
        sbtLedSel = 1;
        // P0 = ucSegSelectState;
        if (mode == 0) {
            P0 = 0x00;
        } else if (mode == 1) {
            P0 = 0xff;
        }
        if (cnt_led >= 10) cnt_led = 0;
    } else {
        sbtLedSel = 0;
        if (mode == 0) {
            display(cnt_moves, 00);  // 显示数字
        } else if (mode == 1) display(display_data_test, 0x00);
    }
}

/*---------主函数--------*/
void main()
{
    Init();
    Initial_M096128x64_ssd1306(); // 显示初始化

    Delay_1ms(5);
    fill_picture(0xff);
    P0 = 0x00;
    MakeSound(5);
    maze_x = 2, maze_y = 0;
    mode = 0; // 游戏模式
    // maze_x = 58, maze_y = 124;
    while( 1 ) {
        if (mode == 0) {
            showMap();
            showDot(maze_x, maze_y);
        } else if (mode == 1) {
            fill_picture(0x00);
        }
        
        if (Key2 == 0) {
            DelayMs(50);
            if (Key2 == 0) {
                beepFlag = ~beepFlag;
            }
        }
        if (Key1 == 0) {
            DelayMs(50);
            if (Key1 == 0) {
                mode = ~mode;
            }
        }
        NavKey_Process();         //获取按键按下情况
    }
}
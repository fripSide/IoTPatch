#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "w25qxx.h"
#include "stmflash.h"

#include "button.h"
#include "iap_patch.h"

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	正点原子 Pandora STM32L475 IoT开发板	实验24
 *	FLASH模拟EEPROM实验		HAL库版本
 *	技术支持：www.openedv.com
 *	淘宝店铺：http://openedv.taobao.com
 *	关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 *	广州市星翼电子科技有限公司
 *	作者：正点原子 @ALIENTEK
 *	******************************************************************************/



//要写入到STM32 FLASH的字符串数组


#define TEXT_Buffer_Size	(8*10)
u8 TEXT_Buffer[TEXT_Buffer_Size]={"STM32 FLASH TEST"};

#define FLASH_SAVE_ADDR  0X08031000 	//设置FLASH 保存地址(必须为8的倍数，且所在扇区,要大于本代码所占用到的扇区.
//否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.
typedef int (*JittedFunc)(void);

void MyHello() {
	printf("My Hello Before Hook!\n");
}

void MyHook() {
	printf("My Hello After Hook!\n");
}

void MyJitted() {
	unsigned char code[8] = {
		0x08, 0x20, 
		0x70, 0x47,  
	};
	uint32_t code_len = sizeof(code) / 8;
	STMFLASH_Write(FLASH_SAVE_ADDR, (uint64_t*) code, code_len);
}

__asm int add8() {
	MOV r0, #8
	BX LR
}

void OnKey0Click() {
	printf("On Key0 Click\n");
	// MyJitted();
	test_flash_iap_write(1);
}

void OnKey1Click() {
	printf("On Key1 Click\n");
	u8 datatemp[TEXT_Buffer_Size];
	LCD_ShowString(30, 185, 200, 16, 16, "Start Read FLASH.... ");
	STMFLASH_Read(FLASH_SAVE_ADDR, (u32*)datatemp, TEXT_Buffer_Size / 4);//每次读出一个字(32位)
	LCD_ShowString(30, 185, 200, 16, 16, "The Data Readed Is:  ");	//提示传送完成
	LCD_ShowString(30, 205, 200, 16, 16, (char *)datatemp);			//显示读到的字符串
}

void OnKey2Click() {
	printf("On Key2 Click\n");
	test_flash_iap_write(2);
	
	// + 1
	//JittedFunc func = (JittedFunc) (FLASH_SAVE_ADDR + 1);
	// func = add8;
	//int ret = func();
	//printf("Res: %d\n", ret);
}

//static Button btnKey0;
//static Button btnKey1;
//static Button btnKey2;

//uint8_t read_button1_GPIO()
//{
//	return KEY0;
//}

//void InitButtons() {
//	// https://github.com/0x1abin/MultiButton
//	button_init(&btnKey0, read_button1_GPIO, 0);
//	button_attach(&btnKey0, SINGLE_CLICK,  OnKey0Click);
//}

int main(void)
{
    u8 key = 0;
    u16 i = 0;
    

    HAL_Init();
    SystemClock_Config();		//初始化系统时钟为80M
    delay_init(80); 			//初始化延时函数    80M系统时钟
    uart_init(115200);			//初始化串口，波特率为115200
    usmart_dev.init(80);		//初始化USMART

    LED_Init();					//初始化LED
    KEY_Init();					//初始化按键
    LCD_Init();					//初始化LCD

    POINT_COLOR = RED;
    Display_ALIENTEK_LOGO(0, 0);
    LCD_ShowString(30, 85, 200, 16, 16, "Pandora STM32L4 IOT");
    LCD_ShowString(30, 105, 200, 16, 16, "FLASH EEPROM TEST");
    LCD_ShowString(30, 125, 200, 16, 16, "ATOM@ALIENTEK");
    LCD_ShowString(30, 145, 200, 16, 16, "2018/10/27");
    LCD_ShowString(30, 165, 200, 16, 16, "KEY1:Write  KEY0:Read");	//显示提示信息

    POINT_COLOR = BLUE;			//设置字体为蓝色
	
	//InitButtons();

    while(1)
    {
        key = KEY_Scan(0);
		
        if(key == KEY0_PRES)	//KEY0按下,读取字符串并显示
        {
//            LCD_ShowString(30, 185, 200, 16, 16, "Start Read FLASH.... ");
//            STMFLASH_Read(FLASH_SAVE_ADDR, (u32*)datatemp, TEXT_Buffer_Size / 4);//每次读出一个字(32位)
//            LCD_ShowString(30, 185, 200, 16, 16, "The Data Readed Is:  ");	//提示传送完成
//            LCD_ShowString(30, 205, 200, 16, 16, (char *)datatemp);			//显示读到的字符串
			
			OnKey0Click();
        }

        if(key == KEY1_PRES)	//KEY1按下,写入STM32 FLASH
        {
//            LCD_Fill(0, 185, 239, 319, WHITE); //清除半屏
//            LCD_ShowString(30, 185, 200, 16, 16, "Start Write FLASH....");
//            STMFLASH_Write(FLASH_SAVE_ADDR, (uint64_t*)TEXT_Buffer, TEXT_Buffer_Size/8); //每次写入两个字(64位)
//            LCD_ShowString(30, 185, 200, 16, 16, "FLASH Write Finished!");	//提示传送完成
			OnKey1Click();
        }
		
		if (key == KEY2_PRES) {
			OnKey2Click();
		}

		//button_ticks();

        i++;
        delay_ms(10);

        if(i == 20)
        {
            LED_B_TogglePin;//提示系统正在运行
            i = 0;
        }
    }
}



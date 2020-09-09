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
 *	����ԭ�� Pandora STM32L475 IoT������	ʵ��24
 *	FLASHģ��EEPROMʵ��		HAL��汾
 *	����֧�֣�www.openedv.com
 *	�Ա����̣�http://openedv.taobao.com
 *	��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 *	������������ӿƼ����޹�˾
 *	���ߣ�����ԭ�� @ALIENTEK
 *	******************************************************************************/



//Ҫд�뵽STM32 FLASH���ַ�������


#define TEXT_Buffer_Size	(8*10)
u8 TEXT_Buffer[TEXT_Buffer_Size]={"STM32 FLASH TEST"};

#define FLASH_SAVE_ADDR  0X08031000 	//����FLASH �����ַ(����Ϊ8�ı���������������,Ҫ���ڱ�������ռ�õ�������.
//����,д������ʱ��,���ܻᵼ�²�����������,�Ӷ����𲿷ֳ���ʧ.��������.
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
	STMFLASH_Read(FLASH_SAVE_ADDR, (u32*)datatemp, TEXT_Buffer_Size / 4);//ÿ�ζ���һ����(32λ)
	LCD_ShowString(30, 185, 200, 16, 16, "The Data Readed Is:  ");	//��ʾ�������
	LCD_ShowString(30, 205, 200, 16, 16, (char *)datatemp);			//��ʾ�������ַ���
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
    SystemClock_Config();		//��ʼ��ϵͳʱ��Ϊ80M
    delay_init(80); 			//��ʼ����ʱ����    80Mϵͳʱ��
    uart_init(115200);			//��ʼ�����ڣ�������Ϊ115200
    usmart_dev.init(80);		//��ʼ��USMART

    LED_Init();					//��ʼ��LED
    KEY_Init();					//��ʼ������
    LCD_Init();					//��ʼ��LCD

    POINT_COLOR = RED;
    Display_ALIENTEK_LOGO(0, 0);
    LCD_ShowString(30, 85, 200, 16, 16, "Pandora STM32L4 IOT");
    LCD_ShowString(30, 105, 200, 16, 16, "FLASH EEPROM TEST");
    LCD_ShowString(30, 125, 200, 16, 16, "ATOM@ALIENTEK");
    LCD_ShowString(30, 145, 200, 16, 16, "2018/10/27");
    LCD_ShowString(30, 165, 200, 16, 16, "KEY1:Write  KEY0:Read");	//��ʾ��ʾ��Ϣ

    POINT_COLOR = BLUE;			//��������Ϊ��ɫ
	
	//InitButtons();

    while(1)
    {
        key = KEY_Scan(0);
		
        if(key == KEY0_PRES)	//KEY0����,��ȡ�ַ�������ʾ
        {
//            LCD_ShowString(30, 185, 200, 16, 16, "Start Read FLASH.... ");
//            STMFLASH_Read(FLASH_SAVE_ADDR, (u32*)datatemp, TEXT_Buffer_Size / 4);//ÿ�ζ���һ����(32λ)
//            LCD_ShowString(30, 185, 200, 16, 16, "The Data Readed Is:  ");	//��ʾ�������
//            LCD_ShowString(30, 205, 200, 16, 16, (char *)datatemp);			//��ʾ�������ַ���
			
			OnKey0Click();
        }

        if(key == KEY1_PRES)	//KEY1����,д��STM32 FLASH
        {
//            LCD_Fill(0, 185, 239, 319, WHITE); //�������
//            LCD_ShowString(30, 185, 200, 16, 16, "Start Write FLASH....");
//            STMFLASH_Write(FLASH_SAVE_ADDR, (uint64_t*)TEXT_Buffer, TEXT_Buffer_Size/8); //ÿ��д��������(64λ)
//            LCD_ShowString(30, 185, 200, 16, 16, "FLASH Write Finished!");	//��ʾ�������
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
            LED_B_TogglePin;//��ʾϵͳ��������
            i = 0;
        }
    }
}



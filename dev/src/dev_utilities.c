#include "dev_utilities.h"
#include "dev_delay.h"
#include "dev_usart.h"

#define ESP             USART2
#define PC              USART3

extern GPIO_InitTypeDef GPIO_InitStructure;
extern uint8_t i_temp;
extern unsigned char temp[BUF_SIZE];
extern unsigned char elec[BUF_SIZE];
extern unsigned char water[BUF_SIZE];
extern unsigned char room[BUF_SIZE];

void dev_uti_show_electric_water(void)
{
	dev_lcd_clear();
	dev_lcd_goto_xy(1,0);
	dev_lcd_send_string(ELEC);
	dev_lcd_goto_xy(2,0);
	dev_lcd_send_string(WATER);
	dev_lcd_goto_xy(1,10);
}

void dev_uti_show_month(void)
{
	dev_lcd_clear();
	dev_lcd_goto_xy(1,0);
	dev_lcd_send_string("Room: ");
}

void dev_uti_make_confirmation(void)
{
	dev_lcd_clear();
	dev_lcd_goto_xy(1,0);
	dev_lcd_send_string("Send it to ESP?");
	dev_lcd_goto_xy(2,0);
	dev_lcd_send_string("A: YES   B: NO");
}

void dev_uti_string_copy(unsigned char *a, unsigned char *b){
	int i=0,j=0;
	while (b[j])
	{
		a[i++]=b[j++];
	}
}

void dev_uti_blink_init(void)
{
    // Initialize GPIOC as output for LED
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void dev_uti_blink(void)
{   
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    dev_delay_ms(200);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    dev_delay_ms(200);
}

/* from position back to index 0 of the string */
void dev_uti_reset_string(unsigned char* string, uint8_t position)
{
    uint8_t i;
    for(i = 0; i < position; i++)
    {
        string[i] = 0; //which is null
    }
}

void dev_uti_prompt_to_insert(void)
{
    dev_lcd_clear();
    dev_lcd_goto_xy(1, 0);
    dev_lcd_send_string("Insert value!");
}

void dev_uti_show_insertions(void)
{
    dev_lcd_clear();
    dev_lcd_goto_xy(1, 0);
    dev_lcd_send_string("R:");
    dev_lcd_send_string((char*)room);
    dev_lcd_goto_xy(2, 0);
    dev_lcd_send_string("E:");
    dev_lcd_send_string((char*)elec);
    dev_lcd_send_char(' ');
    dev_lcd_send_string("W:");
    dev_lcd_send_string((char*)water);
}
void dev_uti_send_to_esp(void)
{
    dev_lcd_clear();
    dev_lcd_send_string("Sending...");
    dev_delay_ms(500);
    dev_usart_send_char('R', PC);
    dev_usart_send_string(room, PC);
    dev_usart_send_char('E', PC);
    dev_usart_send_string(elec, PC);
    dev_usart_send_char('W', PC);
    dev_usart_send_string(water, PC);
    dev_lcd_clear();
    dev_delay_ms(500);
    dev_lcd_goto_xy(1, 6);
    dev_lcd_send_string("Sent");
    dev_lcd_goto_xy(2, 2);
    dev_lcd_send_string("successfully");
    dev_delay_ms(500);
}


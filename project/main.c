#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#include "dev_delay.h"
#include "dev_utilities.h"
#include "dev_keypad.h"
#include "dev_usart.h"
#include "dev_lcd_i2c.h"

#define ESP             USART2
#define PC              USART3

GPIO_InitTypeDef GPIO_InitStructure;

uint8_t                 key;
uint8_t                 i_temp = 0;	
uint8_t                 is_backlight_on = 1;
uint8_t                 usart_received_string_i = 0;        //Index for receiver array
uint8_t                 stop = 0;                           //stop flag when receive "OK" from esp
uint8_t                 month_ten_above = 0;
unsigned char           usart_received_string[BUF_SIZE];    //Receiver arry for usart
unsigned char           elec[BUF_SIZE]  = {'\0'};
unsigned char           water[BUF_SIZE] = {'\0'};
unsigned char           temp[BUF_SIZE]  = {'\0'};
unsigned char           room[BUF_SIZE] = {'\0'};
volatile uint8_t        mode = 0;

int main(void)
{
    dev_delay_init();
    dev_uti_blink_init();
    dev_keypad_init();
    dev_usart_usart2_init();
    dev_usart_usart3_init();
    dev_lcd_init();
    
    dev_uti_show_electric_water();

    // blink to indicate mcu has been started normally
    dev_uti_blink();
    
    dev_uti_show_electric_water();
    i_temp = 0;
    while(1)
    {
        key = dev_keypad_respond();
        switch(key)
        {
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            case 'A':
                if(mode == 0)                                       //Electric has not been confirmed yet
                {
                    if(*temp == '\0')                               //User has not entered any value
                    {
                        dev_uti_prompt_to_insert();
                        dev_delay_ms(1500);
                        dev_uti_show_electric_water();
                    }
                    else                                            //Electric value has been on the temp
                    {
                        dev_uti_string_copy(elec, temp);
                        dev_uti_reset_string(temp, i_temp);         //Reset [0; i_temp-1] to NULL
                        i_temp = 0;                                 //Get ready for the next insertion
                        mode = 1;
                        
                        dev_lcd_goto_xy(2, 10);                     //Move cursor to get ready for next insertion
                    }
                }
                else if(mode == 1)                                  //Electric has been confirmed
                {
                    if(*temp == '\0')                               //User has not entered any value for water, but hit A
                    {
                        dev_uti_prompt_to_insert();
                        dev_delay_ms(1500);
                        dev_uti_show_electric_water();
                        dev_lcd_goto_xy(1, 10);
                        dev_lcd_send_string((char*)elec);           //Reload electric value
                        dev_lcd_goto_xy(2, 10);
                    }
                    else
                    {
                        dev_uti_string_copy(water, temp);
                        dev_uti_reset_string(temp, i_temp);         //Reset [0; i_temp-1] to NULL
                        i_temp = 0;                                 //Get ready for the next insertion
                        mode = 2;
                        
                        dev_uti_show_month();
                    }
                }
                else if(mode == 2)
                {
                    if(*temp == '\0')                               //User has not entered any value for room, but hit A
                    {
                        dev_uti_prompt_to_insert();
                        dev_delay_ms(1500);
                        dev_uti_show_month();
                    }
                    else
                    {
                        dev_uti_string_copy(room, temp);
                        dev_uti_reset_string(temp, i_temp);
                        i_temp = 0;
                        mode = 3;
                        
                        dev_uti_show_insertions();

                    }
                }
                
                else if(mode == 3)
                {
                    dev_uti_make_confirmation();
                    mode = 4;
                }
                
                else if(mode == 4)
                {
                    dev_uti_send_to_esp();
                    dev_uti_reset_string(elec, 10);
                    dev_uti_reset_string(water, 10);
                    dev_uti_reset_string(room, 10);
                    dev_uti_reset_string(temp, 10);
                    dev_lcd_clear();
                    dev_uti_show_electric_water();
                    i_temp = 0;
                    mode = 0;
                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                case 'B':
                    dev_uti_reset_string(elec, 10);
                    dev_uti_reset_string(water, 10);
                    dev_uti_reset_string(temp, 10);
                    i_temp = 0;
                    mode = 0;
                    dev_lcd_clear();
                    dev_delay_ms(200);
                    dev_uti_show_electric_water();
                    break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                case 'C':
                    break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                case 'D':												
                    if(!is_backlight_on)
                    {                                                           //If LCD is off, we can turn on
                        dev_lcd_on_backlight();
                        is_backlight_on = 1;
                        dev_uti_show_electric_water();
                    }
                    else
                    {                                                           //If LCD is on, we can turn off
                        dev_lcd_off_backlight();
                        is_backlight_on = 0;
                    }
                    break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                case 'n':                                                       //No key pressed
                    break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                case '*':                                                       //The other key is disable
                    break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                case '#':                                                       //The other key is disable
                    break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                default:
                    if(is_backlight_on)
                    {
                        if(i_temp == 0)
                        {
                            if(key == '0')
                            {
                                break;                                          //Restrict user from inserting 0 as first digit
                            }
                        }
                        
                        else if(mode == 3)                                      //In the process of making confirmation
                            break;
                        dev_lcd_send_char(key);						
                        temp[i_temp++] = key;                                   //Get data from keypad into temp
                    }
                    break;
        }
    }
}


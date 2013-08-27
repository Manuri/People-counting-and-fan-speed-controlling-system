#include "p1.h"
#include "flex_lcd.c"

void init_pic();
void setTimer();
int checkTimer();
void rayCrossed_isr();
void clock_isr();
int32 convertToCelsius();
int convertToASCII(int16 number);
void lcd_display(int16 number, int x, int y);
void initialDisplay();
int16 read_temperature();
void lcd_display_temperature();

int16 noOfPeople = 1;    // Number of people inside the room
int int_count;         // Number of interrupts left before a second has elapsed
int unitsPosition, tensPosition, hundredsPosition;

int1 rayOne = 0;       // to record when first beam get crossed
int1 rayTwo = 0;       // to record when second beam get crossed

int16 mseconds =0;

int16 temperature;     // The A/D conversion of the analog input
                       // signal results in a corresponding 10-bit digital number.
                       // this variable is to store that

int16 t;           // The A/D conversion of the analog input
                   // signal results in a corresponding 10-bit digital number.
                   // this variable is to store that
int1 yChanged = 0;

int y=0;
int x=0;

void main()
{
   init_pic();

     initialDisplay();

     OUTPUT_HIGH(PIN_C5);

       while(1)
       {
//          if(mseconds<=1000)lcd_display(x, 5, 2);

          if(INPUT(PIN_C0))
          {
             y=0;
             yChanged = 1;
            // lcd_display(y, 7, 1);
          }
          else if(INPUT(PIN_C1))
          {
             Y=1;
              yChanged = 1;
            // lcd_display(y, 7, 1);
          }
          else if(INPUT(PIN_C2))
          {
             y=2;
              yChanged = 1;
            // lcd_display(y, 7, 1);
          }
          else if(INPUT(PIN_C3))
          {
             y=3;
              yChanged = 1;
            // lcd_display(y, 7, 1);
          }
          else if(INPUT(PIN_C4))
          {
             y=4;
              yChanged = 1;
            // lcd_display(y, 7, 1);
          }
if(INPUT(PIN_B0) && yChanged)
   {
    yChanged = 0;
    x++;
    switch(y)
   {
   case 0:
     // OUTPUT_LOW(PIN_B7);
      OUTPUT_HIGH(PIN_B2);
      break;

   case 1:
      delay_ms(8);
     // OUTPUT_HIGH(PIN_B7);
      OUTPUT_LOW(PIN_B2);
      delay_ms(2);
    //  OUTPUT_LOW(PIN_B7);
     OUTPUT_HIGH(PIN_B2);
      break;

   case 2:
      delay_ms(5);
      //OUTPUT_HIGH(PIN_B7);
      OUTPUT_LOW(PIN_B2);
      delay_ms(5);
     // OUTPUT_LOW(PIN_B7);
      OUTPUT_HIGH(PIN_B2);
      break;

   case 3:
      delay_ms(2);
     // OUTPUT_HIGH(PIN_B7);
     OUTPUT_LOW(PIN_B2);
      delay_ms(8);
     // OUTPUT_LOW(PIN_B7);
      OUTPUT_HIGH(PIN_B2);
      break;

   case 4:
     // OUTPUT_HIGH(PIN_B7);
      OUTPUT_LOW(PIN_B2);
      break;
   }
   }

       }

}

/* when timer0 counter overflows(0-255) an interrup occurs. int count which is initially
 * set to INTS_PER_mS, is decremented by 1 inside the isr.
 * When int_count reaches 0, no of miliseconds is incremented by 1
 */
#int_RTCC
void clock_isr() {

    if(--int_count==0)
    {
      ++mseconds;
      int_count=INTS_PER_mS;
    }

}

//#int_EXT
void rayCrossed_isr()
{
 /*  x++;
   switch(y)
   {
   case 0:
     // OUTPUT_LOW(PIN_B7);
      OUTPUT_HIGH(PIN_B7);
      break;

   case 1:
      delay_ms(8);
     // OUTPUT_HIGH(PIN_B7);
      OUTPUT_LOW(PIN_B7);
      delay_ms(2);
    //  OUTPUT_LOW(PIN_B7);
     OUTPUT_HIGH(PIN_B7);
      break;

   case 2:
      delay_ms(5);
      //OUTPUT_HIGH(PIN_B7);
      OUTPUT_LOW(PIN_B7);
      delay_ms(5);
     // OUTPUT_LOW(PIN_B7);
      OUTPUT_HIGH(PIN_B7);
      break;

   case 3:
      delay_ms(2);
     // OUTPUT_HIGH(PIN_B7);
     OUTPUT_LOW(PIN_B7);
      delay_ms(8);
     // OUTPUT_LOW(PIN_B7);
      OUTPUT_HIGH(PIN_B7);
      break;

   case 4:
     // OUTPUT_HIGH(PIN_B7);
      OUTPUT_LOW(PIN_B7);
      break;
   }*/
}


void setTimer()
{
   int_count=INTS_PER_mS;
   set_timer0(0);
   mseconds=0;
}


/* When both rayOne =1 and rayTwo = 1, this is called */
int checkTimer()
{
   if((mseconds <= MAX_CROSSING_TIME) && (mseconds >= MIN_CROSSING_TIME))
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

int32 convertToCelsius()
{
   return (t*150)/307;
}

void lcd_display_temperature()
{
          temperature =read_temperature();
          lcd_display(temperature,13,2);
}

int convertToASCII(int16 number)
{
   return (number+48);
}

void lcd_display(int16 number, int x, int y)
{
   int unitsPosition, tensPosition, hundredsPosition;

       unitsPosition =number % 10;
       tensPosition = ((number - (number % 10))%100)/10;
       hundredsPosition = ((number - (number%100))%1000)/100;

       lcd_gotoxy(x,y);
       lcd_putc(convertToASCII(hundredsPosition));
       lcd_putc(convertToASCII(tensPosition));
       lcd_putc(convertToASCII(unitsPosition));
}

int16 read_temperature()
{
    delay_ms(500);                  //a small delay is required after setting the channel
    t = read_adc();
    return convertToCelsius();
}

void initialDisplay()
{
     delay_ms(25);
     lcd_init();
    // delay_ms(6);

    lcd_gotoxy(1,1);
   lcd_putc("level:");
   lcd_gotoxy(1,2);
   lcd_putc("int:");

 /*    lcd_gotoxy(1,1);
     lcd_putc("*****CSE'10*****");
     lcd_gotoxy(1,2);
     lcd_putc("||||||||||||||||");
     delay_ms(2000);
     lcd_gotoxy(1,1);
     lcd_putc("\fNumberInside:");
     lcd_display(noOfPeople,14,1);
     lcd_gotoxy(1,2);
     lcd_putc("Temperature:   C");*/

}

void init_pic()
{
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_DIV_32);
   setup_psp(PSP_DISABLED);
   setup_spi(FALSE);
   setup_counters( RTCC_INTERNAL, RTCC_DIV_1 | RTCC_8_BIT);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   enable_interrupts(INT_RTCC);
  // enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);

  // EXT_INT_EDGE(L_TO_H);

   OUTPUT_B(0);
   OUTPUT_C(0);

   SET_TRIS_B(0b00000001);   //pins B0, B1 and B2 are set to give inputs. b0 is the external interuupt pin
   SET_TRIS_C(0b00011111);
   SET_TRIS_D(0b00000000);

   set_adc_channel(0);            //the next read_adc call will read channel 0
}

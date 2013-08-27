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
void phaseControl();

int16 noOfPeople = 1;    // Number of people inside the room
int int_count;         // Number of interrupts left before a second has elapsed
int unitsPosition, tensPosition, hundredsPosition;

int1 rayOne = 0;       // to record when first beam get crossed
int1 rayTwo = 0;       // to record when second beam get crossed
int1 pin_changed = 1;
int1 number_changed = 1;

int16 mseconds =0;
int16 counter_for_temp=0 ;

int16 temperature=31;     // The A/D conversion of the analog input
                       // signal results in a corresponding 10-bit digital number.
                       // this variable is to store that

int16 t;           // The A/D conversion of the analog input
                   // signal results in a corresponding 10-bit digital number.
                   // this variable is to store that

void main()
{

 /*  setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_DIV_32);
   setup_psp(PSP_DISABLED);
   setup_spi(FALSE);
   setup_counters( RTCC_INTERNAL, RTCC_DIV_1 | RTCC_8_BIT);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   enable_interrupts(INT_RTCC);
   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);

   EXT_INT_EDGE(L_TO_H);

   OUTPUT_B(0);
   OUTPUT_C(0);

   SET_TRIS_B(0b00000111);   //pins B0, B1 and B2 are set to give inputs. b0 is the external interuupt pin
   SET_TRIS_C(0b00000000);
   SET_TRIS_D(0b00000000);

   set_adc_channel(0); */           //the next read_adc call will read channel 0

   init_pic();

     initialDisplay();
    // OUTPUT_HIGH(PIN_C0);
    // OUTPUT_LOW(PIN_C1);

   OUTPUT_HIGH(PIN_C0);
   OUTPUT_HIGH(PIN_C2);
   OUTPUT_LOW(PIN_C3);

       while(1)
       {
           phaseControl();
         if(number_changed)
         {
            lcd_display(noOfPeople,14,1);
            number_changed = 0;
         }

         if(counter_for_temp == 10000)
         {
            lcd_display_temperature();
            counter_for_temp=0;
         }
         counter_for_temp++;

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

#int_EXT
void rayCrossed_isr()
{
   if(INPUT(PIN_B2)&& rayTwo!=1)
   {
      rayTwo = 1;
      if(rayOne == 1)
      {
         if(checkTimer() == 1)
         {
            noOfPeople++ ;
            number_changed = 1;
         }
         rayOne = 0;
         rayTwo = 0;
      }
      setTimer();
   }
  else  if(INPUT(PIN_B1)&& rayOne!=1)
   {
      rayOne = 1;
      if(rayTwo == 1)
      {
         if(checkTimer() == 1)
         {
            noOfPeople-- ;
            number_changed = 1;
         }
         rayOne = 0;
         rayTwo = 0;
      }
      setTimer();
   }
   delay_ms(MIN_DELAY_BETWEEN_TWO_INTERRUPTS/2);
   //OUTPUT_HIGH(PIN_B7);
   delay_ms(MIN_DELAY_BETWEEN_TWO_INTERRUPTS/2);
   //OUTPUT_LOW(PIN_B7);
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
    delay_us(12);                  //a small delay is required after setting the channel
    t = read_adc();
    return convertToCelsius();
}

void initialDisplay()
{
     delay_ms(25);
     lcd_init();
    // delay_ms(6);

     lcd_gotoxy(1,1);
     lcd_putc("*****CSE'10*****");
     lcd_gotoxy(1,2);
     lcd_putc("||||||||||||||||");
     delay_ms(2000);
     lcd_gotoxy(1,1);
     lcd_putc("\fNumberInside:");
     lcd_display(noOfPeople,14,1);
     lcd_gotoxy(1,2);
     lcd_putc("Temperature:   C");

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
   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);

   EXT_INT_EDGE(L_TO_H);

   OUTPUT_B(0);
   OUTPUT_C(0);

   SET_TRIS_B(0b01000111);   //pins B0, B1 and B2 are set to give inputs. b0 is the external interuupt pin
   SET_TRIS_C(0b00000000);
   SET_TRIS_D(0b00000000);

   set_adc_channel(0);            //the next read_adc call will read channel 0
}

void phaseControl()
{

 /*  switch(noOfPeople)
   {

   case 0 : OUTPUT_HIGH(PIN_B7);
            break;

   default :
   if(INPUT(PIN_B6) && pin_changed == 1)
   {
         pin_changed = 0;
         if(0<=temperature && temperature<20)
         {
            OUTPUT_HIGH(PIN_B7);
         }
       else if(temperature>=20&& temperature<35)
       {
           delay_ms(5);
           OUTPUT_HIGH(PIN_B7);
            delay_ms(5);
            OUTPUT_LOW(PIN_B7);

       }
       else if(temperature>=35)
       {
            OUTPUT_LOW(PIN_B7);
       }
   }
   //else if(!INPUT(PIN_B6) && pin_changed==0) pin_changed = 1;
   if(!INPUT(PIN_B6)) pin_changed = 1;
   }*/

    if(noOfPeople<1)
   {
      OUTPUT_HIGH(PIN_B7);
      //OUTPUT_LOW(PIN_B7);
   }
   else
   {
     // OUTPUT_LOW(PIN_B7);
      if(INPUT(PIN_B6) && pin_changed == 1)
      {

         if(0<=temperature && temperature<20)
         {
            OUTPUT_HIGH(PIN_B7);
         }
       else if(temperature>=20&& temperature<25)
       {
            delay_us(7000);
            OUTPUT_LOW(PIN_B7);
            delay_us(500);
            OUTPUT_HIGH(PIN_B7);

       }

       else if(temperature>=25&& temperature<28)
       {
           delay_us(6000);
            OUTPUT_LOW(PIN_B7);
            delay_us(500);
            OUTPUT_HIGH(PIN_B7);
       }
      else if(temperature>=28&& temperature<35)
       {
           delay_us(5000);
            OUTPUT_LOW(PIN_B7);
            delay_us(500);
            OUTPUT_HIGH(PIN_B7);
       }
       else if(temperature>=35)
       {
            OUTPUT_LOW(PIN_B7);
       }
        pin_changed = 0;
   }
   if(!INPUT(PIN_B6))
   {
      pin_changed = 1;
   }

   }
   }

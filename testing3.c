#include "p1.h"
#include "flex_lcd.c"

void setTimer();
int checkTimer();
void rayCrossed_isr();
void clock_isr();
int32 convertToCelsius();
void lcd_display_temperature();
void lcd_display_no_of_people();
void display_mseconds(); //test function

int noOfPeople = 0;    // Number of people inside the room
int int_count;         // Number of interrupts left before a second has elapsed
int unitsPosition, tensPosition, hundredsPosition;

int1 rayOne = 0;       // to record when first beam get crossed
int1 rayTwo = 0;       // to record when second beam get crossed

int16 mseconds =0;// MIN_DELAY_BETWEEN_TWO_INTERRUPTS;    // A running miliseconds counter

int32 temperature;     // The A/D conversion of the analog input
                       // signal results in a corresponding 10-bit digital number.
                       // this variable is to store that

int32 t;           // The A/D conversion of the analog input
                   // signal results in a corresponding 10-bit digital number.
                   // this variable is to store that

void main()
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

   SET_TRIS_B(0b00000111);   //pins B0 and B4 are set to give inputs
   SET_TRIS_C(0b00000000);
   SET_TRIS_D(0b00000000);

   set_adc_channel(0);            //the next read_adc call will read channel 0

     delay_ms(25);
     lcd_init();
     delay_ms(6);
     lcd_gotoxy(1,1);
     lcd_putc("\fnumber:");
     lcd_display_no_of_people();
     lcd_gotoxy(1,2);
     lcd_putc("ms:");

       while(1)
       {
        // lcd_display_no_of_people();

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
            lcd_display_no_of_people();
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
            lcd_display_no_of_people();
         }
         rayOne = 0;
         rayTwo = 0;
      }
      setTimer();
   }
   delay_ms(MIN_DELAY_BETWEEN_TWO_INTERRUPTS/2);
   OUTPUT_HIGH(PIN_B7);
   delay_ms(MIN_DELAY_BETWEEN_TWO_INTERRUPTS/2);
   OUTPUT_LOW(PIN_B7);
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
   display_mseconds();
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
   int unitsPosition, tensPosition, hundredsPosition;

       unitsPosition = temperature % 10 + 48;
       tensPosition = ((temperature - (temperature % 10))%100)/10 +48;
       hundredsPosition = (temperature - (temperature%100))/100 +48;

       lcd_gotoxy(8,2);
       lcd_putc(hundredsPosition);
       lcd_putc(tensPosition);
       lcd_putc(unitsPosition);

}

void lcd_display_no_of_people()
{

      int unitsPosition, tensPosition, hundredsPosition, thousandsPosition;

       unitsPosition =noOfPeople % 10 + 48;
       tensPosition = ((noOfPeople - (noOfPeople % 10))%100)/10 +48;
       hundredsPosition = ((noOfPeople - (noOfPeople%100))%1000)/100 +48;
       thousandsPosition = ((noOfPeople - (noOfPeople%1000))%100000)/1000 +48;

       lcd_gotoxy(8,1);
       lcd_putc(thousandsPosition);
       lcd_putc(hundredsPosition);
       lcd_putc(tensPosition);
       lcd_putc(unitsPosition);


}

void display_mseconds()
{
    int unitsPosition, tensPosition, hundredsPosition, thousandsPosition;

       unitsPosition = mseconds % 10 + 48;
       tensPosition = ((mseconds - (mseconds % 10))%100)/10 +48;
       hundredsPosition = ((mseconds - (mseconds%100))%1000)/100 +48;
       thousandsPosition = ((mseconds - (mseconds%1000))%100000)/1000 +48;

       lcd_gotoxy(4,2);
       lcd_putc(thousandsPosition);
       lcd_putc(hundredsPosition);
       lcd_putc(tensPosition);
       lcd_putc(unitsPosition);

}


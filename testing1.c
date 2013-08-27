//in this test I changed t and mseconds to int32
#include "p1.h"
#include "flex_lcd.c"

void set_t(int32 timeAtCrossing);
int check_t(int32 timeAtCrossing,int32 dt);
void rayCrossed_isr();
void clock_isr();
void lcd_display_no_of_people();
void display_dt(int32 t,int x,int y); //test function

//int1 first=1;
int noOfPeople = 0;    // Number of people inside the room
int int_count;         // Number of interrupts left before a second has elapsed

int1 rayOne = 0;       // to record when first beam get crossed
int1 rayTwo = 0;       // to record when second beam get crossed

int32 mseconds = 0;//MIN_DELAY_BETWEEN_TWO_INTERRUPTS;    // A running miliseconds counter
int32 t1=0;
int32 t2=0;
int32 dt1=0;
int32 dt2=0;

int t=0;

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

   set_adc_channel(0);            //the next read_adc call will read channel 0
   set_timer0(0);
     lcd_init();
     lcd_gotoxy(1,1);
     lcd_putc("number:");
     lcd_display_no_of_people();

       while(1)
      {
        // lcd_display_no_of_people();
         //display_mseconds();
      // display_dt(dt2,6,2);
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
     set_t(t2);
      rayTwo = 1;
      if(rayOne == 1)
      {
         if(check_t(t1,dt1) == 1)
         {
            noOfPeople++ ;
        //  lcd_display_no_of_people();
         }
         rayOne = 0;
         rayTwo = 0;
      }
   }
  else  if(INPUT(PIN_B1)&& rayOne!=1)
   {
      set_t(t1);
      rayOne = 1;
      if(rayTwo == 1)
      {
         if(check_t(t2,dt2) == 1)
         {
            noOfPeople-- ;
        // lcd_display_no_of_people();
         }
         rayOne = 0;
         rayTwo = 0;
      }
   }
   delay_ms(500);
   OUTPUT_HIGH(PIN_B7);
 }

void set_t(int32 timeAtCrossing)
{
   timeAtCrossing = mseconds;
}


int check_t(int32 timeAtCrossing,int32 dt)
{
   int32 x,p;
   x = mseconds;
   p = x-timeAtCrossing;
   display_dt(p,6,2);
   if((p <= MAX_CROSSING_TIME) && (p >= MIN_CROSSING_TIME))
   {
      timeAtCrossing = 0;
      return 1;
   }
   else
   {
      timeAtCrossing = 0;
      return 0;
   }

}

void lcd_display_no_of_people()
{
      int unitsPosition, tensPosition, hundredsPosition;

       unitsPosition =noOfPeople % 10 + 48;
       tensPosition = ((noOfPeople - (noOfPeople % 10))%100)/10 +48;
       hundredsPosition = ((noOfPeople - (noOfPeople%100))%1000)/100 +48;

       lcd_gotoxy(8,1);
       lcd_putc(hundredsPosition);
       lcd_putc(tensPosition);
       lcd_putc(unitsPosition);
}

void display_dt(int32 t,int x,int y)
{
    int unitsPosition, tensPosition, hundredsPosition, thousandsPosition;

       unitsPosition = t % 10 + 48;
       tensPosition = ((t - (t % 10))%100)/10 +48;
       hundredsPosition = ((t - (t%100))%1000)/100 +48;
       thousandsPosition = ((t - (t%1000))%10000)/1000 +48;

       lcd_gotoxy(x,y);
       lcd_putc(thousandsPosition);
       lcd_putc(hundredsPosition);
       lcd_putc(tensPosition);
       lcd_putc(unitsPosition);

}

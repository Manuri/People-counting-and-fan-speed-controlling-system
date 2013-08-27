#include <18F452.h>
#device adc=10

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES HS                       //High speed Osc (> 4mhz)
#FUSES NOPUT                    //No Power Up Timer
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NODEBUG                  //No Debug mode for ICD
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOCPD                    //No EE protection
#FUSES NOWRT                    //Program memory not write protected

#use delay(clock=20000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)

#define INTS_PER_mS 19         // (20000000/(4*256*1*1000))
                               //  20000000/4 = 5000000 -> internal clock
                               //  5000000/256 = 19531.25 -> RTCC roll over
                               //  19531.25/1 = 19531.25 -> RTCC_DIV_1
                               //  19531.25/1000 = 19.53125 -> miliseconds

#define MAX_CROSSING_TIME 10000   // the maximum time for a person to cross two beams
#define MIN_CROSSING_TIME 1    // the minimum time for a person to cross two beams
#define MIN_DELAY_BETWEEN_TWO_INTERRUPTS 2000 




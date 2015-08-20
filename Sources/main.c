#include <hidef.h>      /* common defines and macros */
#include <mc9s12c128.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12c128"


const struct State{
  void (*CmdPt[6])(void /*int balance*/);
  const struct State *Next[4]; 
};

typedef const struct State StateType;

void dispense (int diet);
void returnChange (int change);


int balance = 0;
unsigned int input;

void None (void /*int balance*/){
   input =0;
};

void Add25(void /*int balance*/){
   balance += 25;
   PORTB = 0x01;
   PORTB = 0x00;
   input = 1;
}

void Add10(void /*int balance*/){
   balance += 10;
   PORTB = 0x02;
   PORTB = 0x00;
   input = 2;
}

void Add5 (void /*int balance*/){
   balance += 5;
   PORTB = 0x04;
   PORTB = 0x00;  
   input = 3;
}

void VendSoda (void ){
   // Vend le SODAS
   if (balance >= 35){
      balance -= 35;
      dispense (0);
      returnChange(balance);
      balance =0;
   } 
   input = 0;
}

void VendDietSoda (void /*int balance*/){
  //VEND le Diet Sodas
   if (balance >= 35){
      balance -= 35;
      dispense (1);
      returnChange(balance);
      balance =0;
   } 
   input = 0;
}
  
  
//wait  for one milli second
void wait1ms (){
    unsigned int start = TCNT;
    unsigned int stop  = start + 2000;
    while (TCNT > stop);
    while (TCNT < stop);
  }
  
  
//wait for secs many seconds
void wait(unsigned int secs){
    int i=0;
    secs = secs*1000;
    for (i; i< secs; i++){
      wait1ms();
    }
  }
  
  
//how to get soda here ie what to light up where
  void dispense (int diet){
    if (diet == 0){
         PORTB=0x1;
         wait(5);
         PORTB=0x0;
         wait(2);
         PORTB=0x1;
         wait(4);
         PORTB=0x0;
    } else{
         PORTB=0x1;
         wait(2);
         PORTB=0x0;
         wait(4);
         PORTB=0x1;
         wait(7);
         PORTB=0x0;
    }
  }

//give client his money back
void returnChange (int change){
  while (change > 0){
    change -= 5;
    PORTB = 0x2;
    wait(1);
    PORTB = 0x0;
  }
}

StateType fsm[8]= 
{ 
  {
    { &None, &Add25, &Add10, &Add5, &None, &None },
    { &fsm[0], &fsm[5], &fsm[2], &fsm[1] }
  }, 
  { 
    { &None, &Add25, &Add10, &Add5, &None, &None }, 
    { &fsm[1], &fsm[6], &fsm[3], &fsm[2] }
  }, 
  {
    { &None, &Add25, &Add10, &Add5, &None, &None }, 
    { &fsm[2], &fsm[7], &fsm[4], &fsm[3] }
  }, 
  { 
    { &None, &Add25, &Add10, &Add5, &None, &None }, 
    { &fsm[3], &fsm[7], &fsm[5], &fsm[4] }
  }, 
  { 
    { &None, &Add25, &Add10, &Add5, &None, &None }, 
    { &fsm[4], &fsm[7], &fsm[6], &fsm[5] }
  }, 
  { 
    { &None, &Add25, &Add10, &Add5, &None, &None }, 
    { &fsm[5], &fsm[7], &fsm[7], &fsm[6] }
  }, 
  { 
    { &None, &Add25, &Add10, &Add5, &None, &None }, 
    { &fsm[6], &fsm[7], &fsm[7], &fsm[7] }
  }, 
  { 
    { &None, &Add25, &Add10, &Add5, &VendSoda, &VendDietSoda }, 
    { &fsm[7], &fsm[7], &fsm[7], &fsm[7] }
  } 
};

void main(void) {
  /* put your own code here */

  StateType *Pt;
  
  // Setting Ports to inputs
  DDRB_BIT0 = 0x1;
  DDRB_BIT1 = 0x1;
  DDRB_BIT2 = 0x1;
  DDRB_BIT3 = 0x1;
  DDRB_BIT4 = 0x1;
  
  //Setting ports to outputs  
  DDRA_BIT0 = 0x0;
  DDRA_BIT1 = 0x0;
  DDRA_BIT2 = 0x0;
  DDRA_BIT3 = 0x0;
  DDRA_BIT4 = 0x0;
  
  //Setting timer
  TSCR1    |=0x80;
  
  
  // assign to init state
  Pt = &fsm[0]; 

	EnableInterrupts;


  for(;;) {
    _FEED_COP(); /* feeds the dog */
    
      input = 0;
      
      if (PORTA_BIT0 == 0){
        input =  1;
        while (PORTA_BIT0 == 0){}
      }
      
      if (PORTA_BIT1 == 0){
        input =  2;
        while (PORTA_BIT1 == 0){}
      }
      
      if (PORTA_BIT2 == 0){
        input =  3;
        while (PORTA_BIT2 == 0){}
      }
      
      if (PORTA_BIT3 == 0){
        input =  4;
        while (PORTA_BIT3 == 0){}
        }
      
      if (PORTA_BIT4 == 0){
        input =  5;
        while (PORTA_BIT4 == 0){}
       }      
       
       Pt->CmdPt[input]();
       Pt = Pt->Next[input];
    
  } /* loop forever */
  /* please make sure that you never leave main */
}

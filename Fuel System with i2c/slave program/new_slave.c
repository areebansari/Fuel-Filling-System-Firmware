#include <18f252.h>
#fuses HS,NOLVP,NOWDT,NOPROTECT
#use delay(clock=25M)
#use rs232(baud=19200, xmit=PIN_C6, rcv=PIN_C7,parity=E, stream=MODBUS_SERIAL)
#use i2c(SLAVE, SDA=PIN_C4, SCL=PIN_C3, address=0xA0) 

#define MODBUS_TYPE MODBUS_TYPE_SLAVE
#define MODBUS_SERIAL_TYPE MODBUS_RTU 
#define MODBUS_SERIAL_RX_BUFFER_SIZE 60
#define MODBUS_SERIAL_BAUD 19200


#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_RDA
#define MODBUS_SERIAL_TX_PIN PIN_C6   // Data transmit pin
#define MODBUS_SERIAL_RX_PIN PIN_C7   // Data receive pin
#define MODBUS_SERIAL_ENABLE_PIN   PIN_C5   // Controls DE pin for RS485
#define MODBUS_ADDRESS 2
#define master1 PIN_C1    //MASTER1 SELECT PIN

#include "modbus.c"

int8 state,dummy;
int16 rcv_data[] = {0,0,0,0};
unsigned int16 hold_regs[2]={0x00,0x00};


#INT_SSP
void i2c_isr()
{
   state = i2c_isr_state();
   if(state == 0)
   {
      dummy = i2c_read();
   }
                                        //state 1 is address
   else if(state >=1 && state<=4) //from state 2 - state 0x80 the master is writing data to slave
   {
      rcv_data[state-1] = i2c_read();
   }
}
   
void main()
{
   
   //int poll = 0;
   int16 event_count = 0;
   setup_adc_ports(NO_ANALOGS);
   enable_interrupts(INT_SSP);
   enable_interrupts(GLOBAL);
   modbus_init();
   
   while(1)
   {
      output_high(PIN_A5);
      
      output_high(master1);
      delay_ms(13);
      output_low(master1);
      
      hold_regs[0] = make16(rcv_data[1],rcv_data[0]);
      hold_regs[1] = make16(rcv_data[3],rcv_data[2]);
      
      while(!modbus_kbhit()); delay_us(50);
      
         if((modbus_rx.address == MODBUS_ADDRESS) || modbus_rx.address == 0)
         {     
             switch(modbus_rx.func)
             {
               case FUNC_READ_HOLDING_REGISTERS:            
               if(modbus_rx.data[0] || modbus_rx.data[2] ||
                  modbus_rx.data[1] >= 2 || modbus_rx.data[3]+modbus_rx.data[1] > 2)
                  
                     modbus_exception_rsp(MODBUS_ADDRESS,modbus_rx.func,ILLEGAL_DATA_ADDRESS);
               else
               {                                
                  modbus_read_holding_registers_rsp(MODBUS_ADDRESS,(modbus_rx.data[3]*2),
                  hold_regs+modbus_rx.data[1]);
                  
                  event_count++;
               }
               break;
             }
         }
   }
}

   


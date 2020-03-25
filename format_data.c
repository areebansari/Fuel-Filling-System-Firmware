//************Funtion to calculate Price and Arrange LCD segments**************
void format_data(void)
{
   static int a=0;
   
   restart_wdt();
   clear_segments();                       
   
   if(filling_start)                //If filling process started
   {
      liters = (pulses_counter * calibration_value)/1000;
      
      amount = liters*unit_price;
      amount = amount/100;
      
      if(liters>=99000)                   //If full filling,then these are the
      {                                   //max ranges of price and weight                                   
         solenoid_and_motor_off();
      }                                        
                                         
      if(!full_filling)              //If filling according to rupees entered
      {
         if(pulses_counter == required_pulses)
         {                                          
            a++;                    //Done to update LCD with the correct value.
            if(a==2)
            { 
               filling_start = FALSE; 
               filling_performed = TRUE; load_values_in_ram = TRUE;
               a=0; 
            }
            solenoid_and_motor_off();
            
            if( kp_rupees ) { 
                              amount = kp_rupees * 100;
                              liters = amount * 100;
                              liters = liters / unit_price;
                            }
            else if( kp_liters ) { 
                                    liters = kp_liters * 100;
                                    amount = liters * unit_price;
                                    amount = amount / 100; 
                                 }
         }
         else if(pulses_counter == FS_turn_off_pulses)   output_high(FAST_SOL);
         else if(pulses_counter == LS_turn_off_pulses)   output_high(SLOW_SOL);
      }
   }
                      
   //--------------------Segments Arrangment for Unit Price--------------------
   if(unit_price==0)  
   {
      lcd_data[0] = 0x00; lcd_data[2] = 0x20; lcd_data[10] = 0xA0; lcd_data[11] = 0xB0;
   }
   if(unit_price>=1)
   {
      lcd_data[0]=0x00 | rate_hundredth;
   }
   if(unit_price>=10)
   {
      lcd_data[2]=0x20 | rate_tenth;
   }
   if(unit_price>=100)
   {
      lcd_data[11]=0xB0 | rate_ones;
   }
   if(unit_price>=1000)                               
   {
      lcd_data[10]=0xA0 | rate_tens;
   }
   
   //----------------------Segments Arrangment for liters----------------------
   
   temp32 = liters;
   hundred=0;tens=0;ones=0;tenth=0;hundredth=0;
   
   while(temp32>=100000)
   {
      temp32-=100000;
   }
   while(temp32>=10000)
   {
      temp32-=10000;
      hundred++;
   }
   while(temp32>=1000)
   {
      temp32-=1000;
      tens++;
   }
   while(temp32>=100)
   {
      temp32-=100;
      ones++;
   }
   while(temp32>=10)
   {
      temp32-=10;
      tenth++;
   }
   hundredth=temp32;
   
   lcd_data[4]=0x40 | hundredth;
   lcd_data[3]=0x30 | tenth;
   lcd_data[1]=0x10 | ones;
   
   if(liters>=1000)
   {
      lcd_data[8]=0x80 | tens;
   }
   if(liters>=10000)
   {
      lcd_data[9]=0x90 | hundred;
   }         
   //----------------------Segments Arrangment for Price-----------------------
                                                               
   temp32 = amount;
   tenthousand=0;thousand=0;hundred=0;tens=0;ones=0;tenth=0;hundredth=0;
   
   while(temp32>=1000000)
   {
      temp32-=1000000;
      tenthousand++;
   }
   while(temp32>=100000)
   {
      temp32-=100000;
      thousand++;
   }
   while(temp32>=10000)
   {
      temp32-=10000;
      hundred++;
   }
   while(temp32>=1000)
   {
      temp32-=1000;
      tens++;
   }
   while(temp32>=100)
   {
      temp32-=100;
      ones++;                           
   }
   while(temp32>=10)
   {
      temp32-=10;
      tenth++;
   }
      hundredth=temp32;
   
   lcd_data[7] =0x70 | tenth;
   lcd_data[6] =0x60 | ones;
   
   if(amount>=1000)
   {
      lcd_data[5]=0x50 | tens;
   }
   if(amount>=10000)
   {
      lcd_data[15]=0xF0 | hundred;
   }
   if(amount>=100000)
   {
      lcd_data[12]=0xC0 | thousand;
   }
   if(amount>=1000000)
   {
      lcd_data[13]=0xD0 | tenthousand;
   }
   if(input(POWER_SENSE)) lcd_data[14]=0xe2;  //If power loss then show P
   else lcd_data[14]=0xea;                    //else Blank that segment                          
}
//*****************************************************************************

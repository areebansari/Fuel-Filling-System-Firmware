void io_init(void)
{
// ========================= Setting Initial PIN states
   output_high(STROBE_K);     // make the strobe high (deactivate keypad strobe)
   delay_cycles(10);
   output_high(DATA_PIN);     // make the data high (just an initial state)
   delay_cycles(10);
   output_high(CLOCK_PIN);    // make the clock pin high (deactivate clock output)
   delay_cycles(10);
   output_low(STROBE_L);      // make the strobe low (deactivate LCD strobe)
   delay_cycles(10);
   
   output_high(KEY_PRESSED);  // make keypressed pin high(deactive pin)
   delay_cycles(10);
   
   output_high(MECH_COUNTER); //make the mechanical counter high (deactivate mech counter)
   delay_cycles(10);
   
   output_low(I2C_SELECT);    // make the I2C Select low (deactivate I2C Select)
   delay_cycles(10);
   output_high(FAST_SOL);     // make the fast solenoid high (deactivate fast solenoid)
   delay_cycles(10);
   output_high(SLOW_SOL);     // make the slow solenoid high (deactivate slow solenoid)
   delay_cycles(10);
   output_high(MOTOR_STATE);  // make the motor pin high (deactivate motor)
   delay_cycles(10);
   
   
// ========================== Setting pin directions (in/out)
/* Default state of any IO PIN after reset is INPUT, we only set output directions 
 * for desired pin
 */
 
   output_drive(STROBE_K);    // clear bit 4 of TrisA reg(make this pin as output)
   output_drive(DATA_PIN);    // clear bit 3 of TrisA reg(make this pin as output)
   output_drive(CLOCK_PIN);   // clear bit 2 of TrisA reg(make this pin as output)
   output_drive(STROBE_L);    // clear bit 1 of TrisA reg(make this pin as output)
   
   output_drive(MECH_COUNTER);  // clear bit 4 of TrisB reg(make this pin as output)
   
   output_drive(I2C_SELECT);    // clear bit 5 of TrisA reg(make this pin as output)
   output_drive(FAST_SOL);      // clear bit 2 of TrisA reg(make this pin as output)
   output_drive(SLOW_SOL);      // clear bit 1 of TrisA reg(make this pin as output)
   output_drive(MOTOR_STATE);   // clear bit 0 of TrisA reg(make this pin as output)
   
   output_low(PIN_B7);
   output_drive(PIN_B7);
   
   output_low(PIN_B6);
   output_drive(PIN_B6);
}

void peripheral_init(void)
{   
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);    /* 16-bit Timer, 8 usec tick time
                                                   Used with IR routine  */
   
   set_timer3(53035);                           // interrupt after 100 msec.
   setup_timer_3(T3_INTERNAL | T3_DIV_BY_8);    /* 16-bit Timer, 8 usec tick time
                                                   Used for different timings  */
                                                   
   enable_interrupts(INT_TIMER3);
   
   ext_int_edge(2,h_to_l);                      //Edge Selection for IR Routine.
   
   enable_interrupts(GLOBAL);
   
   setup_wdt(wdt_on);
   restart_wdt();
}

void parameters_init(void)
{
   int clear_locations;
   
   if(read_8_bit_values(_EE_FIRST_RUN))      // New programming? first time store
   {                                         // '0's in first 100 locations.
      for(clear_locations = 1; clear_locations <= 100 ; clear_locations++)
      {
         write_8_bit_values(0,clear_locations);
         delay_cycles(10);
      }
      write_8_bit_values(_EE_FIRST_RUN,0);   // write '0' in _EE_FIRST_RUN. 
   }
   
   ml_threshold = read_8_bit_values(_EE_ML_THRESHOLD);               // Menu 1 value.
   pulse_threshold = ml_threshold;
  
   false_pulses_timing = read_8_bit_values(_EE_FALSE_PULSES_TIMING); // Menu 2 value.
   
   fast_sol_turnoff_margin = read_8_bit_values(_EE_FAST_SOL_TURNOFF_MARGIN);//Menu3 value.
   slow_sol_turnoff_margin = read_8_bit_values(_EE_SLOW_SOL_TURNOFF_MARGIN);//Menu4 value.
   
   calibration_value = read_16_bit_values(_EE_CALIBRATION_VALUE);    // Menu 5 value.
   if(calibration_value < _CALIB_MIN)  calibration_value = _CALIB_MIN;
   if(calibration_value > _CALIB_MAX)  calibration_value = _CALIB_MAX;
   
   motor_timeout = read_8_bit_values(_EE_MOTOR_TIMEOUT);             // Menu 7 value.
   
   ml_setting = read_8_bit_values(_EE_ML_SETTING);                   // Menu 8 value.
   
   sign_flag = read_8_bit_values( _EE_SIGN_FLAG);                    // Menu 8 sign value.
   
   no_flow_timeout = read_8_bit_values(_EE_NO_FLOW_TIMEOUT);         // Menu 9 value.
   
   rate_ones = read_8_bit_values(_EE_RATE_ONES);   delay_cycles(4);
   rate_tens = read_8_bit_values(_EE_RATE_TENS);   delay_cycles(4);
   rate_tenth = read_8_bit_values(_EE_RATE_TENTH); delay_cycles(4);
   rate_hundredth = read_8_bit_values(_EE_RATE_HUNDREDTH);  delay_cycles(4);
   rate_hundred = read_8_bit_values(_EE_RATE_HUNDRED);  delay_cycles(4);
   
   unit_price = _mul(rate_hundred,10000) + _mul(rate_tens,1000)        // unit price value.
                + _mul(rate_ones,100) +_mul(rate_tenth,10)  + rate_hundredth;
   
   liters = read_32_bit_values( _EE_LITERS);delay_cycles(4);         // last filled.
   amount = read_32_bit_values( _EE_AMOUNT);delay_cycles(4);         // last filled.
   
   mech_count = read_32_bit_values( _EE_MECH_COUNT);delay_cycles(4); 
   remain_mech_value = read_32_bit_values( _EE_REMAIN_MECH_VALUE);delay_cycles(4);
   
   shift_totalizer = read_32_bit_values( _EE_SHIFT_TOTALIZER); delay_cycles(4);
   remain_shift_value = read_32_bit_values( _EE_REMAIN_SHIFT_VALUE);
   
   master_totalizer = read_32_bit_values( _EE_MASTER_TOTALIZER);delay_cycles(4);
   
   DEBUG_DATA("\r\n mech count = %u",mech_count);
   DEBUG_DATA("\r\n remain mech value = %u",remain_mech_value);
   DEBUG_DATA("\r\n shift = %lu",shift_totalizer);
   DEBUG_DATA("\r\n master = %lu",master_totalizer);
   DEBUG_DATA("\r\n remain shift = %lu",remain_shift_value);
   
   memset(ltr_history,0,sizeof(ltr_history));
   memset(rs_history,0,sizeof(rs_history));
   memset(rate_history,0,sizeof(rate_history));
   
   memset(ltr_10_history,0,sizeof(ltr_10_history));
   
   ltr_history[0] = liters;         // copy the last filling values in latest position  
   rs_history[0] = amount;          // of an array.
   rate_history[0] = unit_price;    //
   
   for(k=0; k< _LTR_10_ARRAY; k++)  // read the last 10 filled liters fromm EEPROM and 
   {                                // store it in an array.
      ltr_10_history[k]= read_32_bit_values(_EE_LTR_10_HISTORY + (k*4));
      delay_cycles(4);
   }
}



/*==================================================================================*/
/***
   Funtion     :  void display_888_pattern(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  shows the 888 pattern on LCD.
***/   
/*==================================================================================*/
void display_888_pattern(void)
{
   int loop=0;
   do
   {
      restart_wdt();
      dataout_lcd(lcd_888[loop]);
      
      dataout_lcd(lcd_888[loop+6]);
      
      dataout_lcd(lcd_888[loop+12]);
      
      delay_50();
      loop++;
   }while(loop<6);   

}



/*==================================================================================*/
/***
   Funtion     :  void clear_888_pattern(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  clear the 888 pattern form LCD.
***/   
/*==================================================================================*/
void clear_888_pattern(void)
{                                
   int loop=0;
   do
   {
      restart_wdt();
      dataout_lcd(lcd_blank[loop]);
      
      dataout_lcd(lcd_blank[loop+6]);
      
      dataout_lcd(lcd_blank[loop+12]);
      
      delay_50();
      loop++;
   }while(loop<6);
}



/*==================================================================================*/
/***
   Funtion     :  void dataout_lcd(int data)
   Parameters  :  data
   Return value:  None
   Purpose     :  This function receives the data, then this data will be out to LCD
                  bit by bit.
***/   
/*==================================================================================*/
void dataout_lcd(int data)
{
   restart_wdt();   
   int b=0b10000000;
   do
   {
      output_bit(DATA_PIN,data & b);   // data is AND with b, the result will be out.
      output_low(CLOCK_PIN);           // clock pin low.
      output_high(CLOCK_PIN);          // clock pin high.
      rotate_right(&b, 1);             // rotate right value of 'b' by 1 time. 
   }while(b!=0x80);
   
   output_high(STROBE_L);              // strobe signal high.
   delay_cycles(2);
   output_low(STROBE_L);               // strobe signal low.
}



//****************************Function to Clear LCD****************************
void clear_lcd(void)
{
   int x=0;
   restart_wdt();
   for(x=0;x<16;x++) dataout_lcd(lcd_clear[x]);
}                              
//*****************************************************************************

//**********************Function to Display values on LCD**********************
void show_on_lcd(void)                              
{
   int x=0;
   restart_wdt();
   for(x=0;x<16;x++) dataout_lcd(lcd_data[x]);
}                                              
//*****************************************************************************

//*********************Function to Clear the LCD Segments**********************
void clear_segments()
{ 
   lcd_data[0]=0x0A;lcd_data[1]=0x1A;lcd_data[2]=0x2A;lcd_data[3]=0x3A;
   lcd_data[4]=0x4A;lcd_data[5]=0x5A;lcd_data[6]=0x6A;lcd_data[7]=0x7A;
   lcd_data[8]=0x8A;lcd_data[9]=0x9A;lcd_data[10]=0xAA;lcd_data[11]=0xBA;  
   lcd_data[12]=0xCA;lcd_data[13]=0xDA;lcd_data[14]=0xEA;lcd_data[15]=0xFA;
}
//*****************************************************************************


//******************Fucntion to write 8 bit values in EEPROM*******************
void write_8_bit_values(int value,int location)
{
   write_eeprom(location,value);
} 
//*****************************************************************************

//******************Function to read 8 bit values from EEPROM******************
int read_8_bit_values(int location)
{
   return read_eeprom(location);
}                                                                             
//*****************************************************************************




//*****************Function to write 16 bit values in EEPROM*******************
void write_16_bit_values(int16 value, int location)
{
   write_eeprom(location,value);
   write_eeprom(location+1,value>>8);
}
//*****************************************************************************

//*****************Function to read 16 bit values form EEPROM******************
int16 read_16_bit_values(int location)
{
   return make16(read_eeprom(location+1),read_eeprom(location));
}
//*****************************************************************************



//******************Function to write 32 bit values in EEPROM******************
void write_32_bit_values(int32 value, int location)
{
   write_eeprom(location,value);
   write_eeprom(location+1,value>>8);
   write_eeprom(location+2,value>>16);
   write_eeprom(location+3,value>>24);           
}
//*****************************************************************************

//*****************Function to read 32 bit values form EEPROM******************
int32 read_32_bit_values(int location)
{
   return make32(read_eeprom(location+3),read_eeprom(location+2),
                  read_eeprom(location+1),read_eeprom(location));
}
//*****************************************************************************

//*****************************************************************************
void solenoid_and_motor_off(void)
{
   disable_interrupts(int_ext);        //Disable pulses interrupt                   
   output_high(FAST_SOL);     // make the fast solenoid high (deactivate fast solenoid)
   delay_cycles(4);
   output_high(SLOW_SOL);     // make the slow solenoid high (deactivate slow solenoid)
   delay_cycles(4);
   output_high(MOTOR_STATE);  // make the motor pin high (deactivate motor)
}                                                               
//*****************************************************************************

void solenoid_and_motor_off_ISR(void)  // same as above function but to be called in ISR
{
   disable_interrupts(int_ext);        //Disable pulses interrupt                   
   output_high(FAST_SOL);     // make the fast solenoid high (deactivate fast solenoid)
   delay_cycles(4);
   output_high(SLOW_SOL);     // make the slow solenoid high (deactivate slow solenoid)
   delay_cycles(4);
   output_high(MOTOR_STATE);  // make the motor pin high (deactivate motor)
}
//*****************************************************************************

void data_save(void)
{
   output_high(MECH_COUNTER);
   output_high(PIN_B6);
   if(totalizer_calculations)
   {
//!      temp2_shift = ( temp1_shift * calibration_value )/10000;
//!      shift_totalizer = shift_totalizer + temp2_shift;
//!      temp1_shift = 0;
      
//!      temp1_shift = temp1_shift * calib_value;
//!      shift_totalizer = shift_totalizer + temp1_shift;
//!      temp1_shift = 0;
      totalizers_calculations();
      
      temp2_master = ( temp1_master * calibration_value )/10000;
      master_totalizer = master_totalizer + temp2_master;
      temp1_master = 0;
      
      totalizer_calculations = FALSE;
   }
   write_32_bit_values( shift_totalizer, _EE_SHIFT_TOTALIZER);    delay_ms(1);
   write_32_bit_values( remain_shift_value, _EE_REMAIN_SHIFT_VALUE); delay_ms(1);
   
   write_32_bit_values( master_totalizer, _EE_MASTER_TOTALIZER);  delay_ms(1);
   
   write_32_bit_values( remain_mech_value, _EE_REMAIN_MECH_VALUE ); delay_ms(1);
   write_32_bit_values( mech_count, _EE_MECH_COUNT);  delay_ms(1);
   
   write_32_bit_values( amount, _EE_AMOUNT);          delay_ms(1);
   write_32_bit_values( liters, _EE_LITERS);          delay_ms(1);
   
   for(k=0; k<_LTR_10_ARRAY; k++)
   {
      write_32_bit_values( ltr_10_history[k], _EE_LTR_10_HISTORY + (k*4));
      delay_cycles(4);
   }
   output_low(PIN_B6);
}



/*==================================================================================*/
/***
   Funtion     :  void power_check(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  Purpose of this functions is to check if the power is available or
                  not. If power is not available then store the data into EEPROM and
                  stay here until power restores.
***/   
/*==================================================================================*/
void power_check(void)
{
   restart_wdt();
   if(input(POWER_SENSE))              // If power goes
   {
      disable_interrupts(INT_EXT);     //
      disable_interrupts(INT_EXT2);    // Disable all interrupts.
      disable_interrupts(INT_TIMER1);  //
      
      filling_start = FALSE;           // Stop filling process.
      solenoid_and_motor_off();        // Turn OFF solenoids and motor.
      
      if(new_filling)                  // if new filling occured then store all the   
      {                                // data in EEPROM.
         data_save(); 
         new_filling = FALSE;
      }  
      if(two_sec_passed)               // when system power up and 2 sec have passed then 
      {                                // update the LCD. otherwise, do not update LCD
         format_data();
         show_on_lcd();
      }
      
      while(input(POWER_SENSE)) restart_wdt();  // stay here until power restore.
      
      p_occured = TRUE;
   }
}

void show_liter_history(void)
{
   clear_lcd();
   clear_segments();
   
   DEBUG_DATA("Litre Histroy Routine");
   
   show_liter_history_flag = FALSE;
   history_no = 11;
   while(TRUE)
   {
      restart_wdt();
      read_keypad();
      power_check();
      keypad_timeout = 0;

      if( history_no == 10 ) return;
      
      temp32 = ltr_10_history[history_no];
      hundred = 0; tens = 0; ones = 0; tenth = 0; hundredth = 0;

      while(temp32>=10000) { restart_wdt(); temp32-=10000; hundred++; }
      while(temp32>=1000) { restart_wdt(); temp32-=1000; tens++; }
      while(temp32>=100) { restart_wdt(); temp32-=100; ones++; }
      while(temp32>=10) { restart_wdt(); temp32-=10; tenth++;}
      hundredth=temp32;
      
      lcd_data[4]=0x40 | hundredth;
      lcd_data[3]=0x30 | tenth;
      lcd_data[1]=0x10 | ones;
      
      if(ltr_10_history[history_no]>=1000) lcd_data[8]=0x80 | tens;
      else lcd_data[8] = 0x8A;
      
      if(ltr_10_history[history_no]>=10000)lcd_data[9]=0x90 | hundred;
      else lcd_data[9] = 0x9A;
      
      show_on_lcd();
   }
}



/*==================================================================================*/
/***
   Funtion     :  void delay_50(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  To generate a delay of approx 50mS.
***/   
/*==================================================================================*/
void delay_50(void)
{
   unsigned int8 delay=0;  

   delay=0;
   do
   {
      restart_wdt();
      delay++;
      delay_ms(10);
      power_check();    // Check if POWER is available or not.           
   }while(delay<=5);               
}



/*==================================================================================*/
/***
   Funtion     :  void delay_250(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  To generate a delay of approx 250mS.
***/   
/*==================================================================================*/
void delay_250(void)
{
   unsigned int8 delay=0;
   
   delay=0;
   do
   {      
      restart_wdt();
      delay++;
      delay_ms(10);
      power_check();    // Check if POWER is available or not.
   }while(delay<=25);             
}




void totalizers_calculations(void)
{
   temp2_shift = temp1_shift * calib_value;
   temp2_shift = temp2_shift + remain_shift_value;
   
   if(temp2_shift>=10000)
   {
      temp32 = temp2_shift;
      while(temp32>=10000)
      {
         shift_totalizer++;
         temp32-=10000;
      }
      remain_shift_value = temp32;
      temp1_shift = 0;
   }
   else 
   {
      remain_shift_value = temp2_shift;
      temp1_shift = 0;
   }
}
//*****************************************************************************


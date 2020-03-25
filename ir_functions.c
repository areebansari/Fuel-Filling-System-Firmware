void ir_routine(void)
{
   //int1 test_flag=FALSE,test_flag2=TRUE;
   
   clear_interrupt(int_ext2);
   enable_interrupts(int_ext2);
   
   key_press_check = TRUE; any_key_pressed=FALSE; main_menu=0; menu_no=0;
   nozzle_counter=0;
   while(TRUE)
   {
      restart_wdt();
      power_check();
      
//!      output_toggle(PIN_B7);
      if(input(NOZZLE))             //If switch is on
      {                             //Disable IR interrupt & exit the IR func      
//!         IR=FALSE; //disp_pattern=1; ir_key_press2=0;         
         nozzle_counter++;
         if(nozzle_counter>=500)
         {
            disable_interrupts(INT_EXT2);
            disable_interrupts(INT_TIMER1);                                               
            nozzle_counter=0;
            return;
         }
      }                                                                 
      
      if(any_key_pressed && key_press_check)
      {
         key_press_check = FALSE; any_key_pressed = FALSE;
         DEBUG_DATA("\r\nData is stored");
         data_save();
         main_menu = 1; //test_flag=TRUE;
      }
      
      switch(main_menu)
      {
         case 0:
         //if(test_flag2) { DEBUG_DATA("\r\nCase 0"); test_flag2=FALSE; }
         break;
         
         case _DISP_LAST_FILLING:
         //if(test_flag)  { DEBUG_DATA("\r\nDisplay last filling values"); test_flag=FALSE; }
         
         format_data();
         show_on_lcd();
         
         break;
         
         case _SUB_MENU_CASES:
         {
            DEBUG_DATA("\r\nCase 2");
            clear_lcd();
            
            while(menu)
            {
               restart_wdt();
               if(sub_menu)
               {
                  sub_menu=0;
                  if(prv_command==command) inc_var++;    
                  else prv_command=command;
                  
                  if(inc_var==4)         
                  {
                     menu_no=command;
                     inc_var=0; menu=0; msec_100=0; prv_command=0; enter=1;     
                  }                     
               }
            }
            
            switch(menu_no)
            {
               case 0:
               main_menu = 1; //test_flag=TRUE; 
               break;
               
               case _ML_TO_IGNORE://MENU NO 1
               DEBUG_DATA("\r\nmenu 1");
               
               parameters_value(ml_threshold,_ML_TO_IGNORE,TRUE);
               write_8_bit_values(ml_threshold,_EE_ML_THRESHOLD);
//!               pulse_threshold = ml_threshold/10;
               pulse_threshold = ml_threshold;
               
               DEBUG_DATA("\r\npulse_threshold=%u",pulse_threshold);
               main_menu = 1; menu_no = 0; //test_flag = TRUE; 
               break;
               
               
//!               case _LTRS_FOR_FAST_SOLENOID://MENU NO 2
//!               DEBUG_DATA("\r\nmenu 2");
//!               
//!               parameters_value(min_ltrs,_LTRS_FOR_FAST_SOLENOID,TRUE);
//!               write_8_bit_values( min_ltrs, _EE_MIN_LTRS);
//!               min_ltrs_100x = _mul(min_ltrs,100);
//!               
//!               DEBUG_DATA("\r\nmin_ltrs=%u",min_ltrs);
//!               main_menu = 1 ;menu_no = 0; //test_flag = TRUE; 
//!               break;
               
               case _FALSE_PULSES_IGNORE_TIMING://MENU NO 2
               DEBUG_DATA("\r\nmenu 2");
               
               parameters_value( false_pulses_timing, _FALSE_PULSES_IGNORE_TIMING, TRUE );
               if( false_pulses_timing >50 ) false_pulses_timing = 50;
               if( pulse_threshold == 0) false_pulses_timing = 0;
               write_8_bit_values( false_pulses_timing, _EE_FALSE_PULSES_TIMING );
               
//!               parameters_value(min_ltrs,_LTRS_FOR_FAST_SOLENOID,TRUE);
//!               write_8_bit_values( min_ltrs, _EE_MIN_LTRS);
//!               min_ltrs_100x = _mul(min_ltrs,100);
//!               
               DEBUG_DATA("\r\nfalse pulses=%u",false_pulses_timing);
               main_menu = 1 ;menu_no = 0; //test_flag = TRUE; 
               break;
                                                                                  
               
               case _FAST_SOLENOID_OFF_MARGIN://MENU NO 3
               DEBUG_DATA("\r\nmenu 3");                                         
               
               parameters_value(fast_sol_turnoff_margin,_FAST_SOLENOID_OFF_MARGIN,FALSE);
               write_8_bit_values(fast_sol_turnoff_margin,_EE_FAST_SOL_TURNOFF_MARGIN);
               
               DEBUG_DATA("\r\nfast_sol_turnoff_ml=%u",fast_sol_turnoff_margin);
               main_menu = 1; menu_no = 0; //test_flag = TRUE;
               break;
               
               case _SLOW_SOLENOID_OFF_MARGIN://MENU NO 4
               DEBUG_DATA("\r\nmenu 4");
               
               parameters_value(slow_sol_turnoff_margin,_SLOW_SOLENOID_OFF_MARGIN,TRUE);
               if(slow_sol_turnoff_margin >= fast_sol_turnoff_margin)
                  slow_sol_turnoff_margin = fast_sol_turnoff_margin-10;
               write_8_bit_values(slow_sol_turnoff_margin, _EE_SLOW_SOL_TURNOFF_MARGIN);
               
               DEBUG_DATA(" slow_sol_turnoff_ml=%u",slow_sol_turnoff_margin);
               main_menu = 1; menu_no = 0; //test_flag = TRUE; 
               break;
               
               case _CALIBRATION_FACTOR://MENU NO 5
               DEBUG_DATA("\r\nmenu 5");
               parameters_value(calibration_value,_CALIBRATION_FACTOR,TRUE);
               if(calibration_value < _CALIB_MIN)  calibration_value = _CALIB_MIN;
               if(calibration_value > _CALIB_MAX)  calibration_value = _CALIB_MAX;
                              
               write_16_bit_values(calibration_value, _EE_CALIBRATION_VALUE);

               DEBUG_DATA(" value=%lu",calibration_value);
               main_menu = 1; menu_no = 0; //test_flag = TRUE; 
               break;
               
               case _RESTORE_FACTORY_SETTING://MENU NO 6
               DEBUG_DATA("\r\nmenu 6");
               
               ml_threshold               = _ML_THRESHOLD;
//!               min_ltrs                   = _MIN_LTRS;
               false_pulses_timing        =  5;
               fast_sol_turnoff_margin    = _FAST_SOL_TURNOFF_MARGIN;
               slow_sol_turnoff_margin    = _SLOW_SOL_TURNOFF_MARGIN;
               calibration_value          = _CALIBRATION_VALUE;
               motor_timeout              = _MOTOR_TIMEOUT;
               ml_setting                 = _ML_SETTING;
               sign_flag                  = _SIGN_FLAG;
               no_flow_timeout            = _NO_FLOW_TIMEOUT;
               
//!               pulse_threshold = ml_threshold/10;
               pulse_threshold = ml_threshold;
//!               min_ltrs_100x = _mul(min_ltrs,100);
               
               rate_tens = 7; rate_ones = 8; rate_tenth = 1; rate_hundredth = 2;
               unit_price =_mul(rate_tens,1000) + _mul(rate_ones,100) +
                           _mul(rate_tenth,10)  + rate_hundredth; 
                           
               if(calibration_value < _CALIB_MIN)  calibration_value = _CALIB_MIN;
               if(calibration_value > _CALIB_MAX)  calibration_value = _CALIB_MAX;
                           
               write_8_bit_values(ml_threshold, _EE_ML_THRESHOLD);   delay_cycles(4);
               write_8_bit_values( false_pulses_timing, _EE_FALSE_PULSES_TIMING); delay_cycles(4);
//!               write_8_bit_values(min_ltrs, _EE_MIN_LTRS);  delay_cycles(4);
               write_8_bit_values(fast_sol_turnoff_margin, _EE_FAST_SOL_TURNOFF_MARGIN);
               delay_cycles(4);
               write_8_bit_values(slow_sol_turnoff_margin, _EE_SLOW_SOL_TURNOFF_MARGIN);
               delay_cycles(4);
               write_16_bit_values(calibration_value, _EE_CALIBRATION_VALUE);
               delay_cycles(4);
               write_8_bit_values(motor_timeout, _EE_MOTOR_TIMEOUT); delay_cycles(4);
               
               write_8_bit_values(ml_setting, _EE_ML_SETTING);  delay_cycles(4);
               write_8_bit_values( sign_flag, _EE_SIGN_FLAG);  delay_cycles(4);
               
               write_8_bit_values(no_flow_timeout, _EE_NO_FLOW_TIMEOUT);
               delay_cycles(4);
               
               write_8_bit_values(rate_tens, _EE_RATE_TENS);   delay_cycles(4);
               write_8_bit_values(rate_ones, _EE_RATE_ONES);   delay_cycles(4);
               write_8_bit_values(rate_tenth, _EE_RATE_TENTH); delay_cycles(4);
               write_8_bit_values(rate_hundredth, _EE_RATE_HUNDREDTH);
               
               main_menu = 1; menu_no = 0; //test_flag = TRUE; 
               break;
               
               case _MOTOR_OFF_TIMING://MENU NO 7
               DEBUG_DATA("\r\nmenu 7");
               
               parameters_value(motor_timeout,_MOTOR_OFF_TIMING,TRUE);
               write_8_bit_values(motor_timeout, _EE_MOTOR_TIMEOUT);
               
               DEBUG_DATA(" value=%u",motor_timeout);
               main_menu = 1; menu_no = 0; //test_flag = TRUE; 
               break;
               
               case _ML_SETTING_MENU://MENU NO 8
               DEBUG_DATA("\r\nmenu 8");
               
               parameters_value_ml(ml_setting,sign_flag,_ML_SETTING_MENU);
               write_8_bit_values(ml_setting, _EE_ML_SETTING);
               write_8_bit_values(sign_flag, _EE_SIGN_FLAG);
               
               DEBUG_DATA(" value=%u",ml_setting);
               main_menu = 1; menu_no = 0; //test_flag = TRUE; 
               break;
               
               case _TIMEOUT_FOR_NO_FLOW://MENU NO 9
               DEBUG_DATA("\r\nmenu 9");
               
               parameters_value(no_flow_timeout,_TIMEOUT_FOR_NO_FLOW,TRUE);
               write_8_bit_values(no_flow_timeout, _EE_NO_FLOW_TIMEOUT);
               
               DEBUG_DATA("\r\nno flow timeout=%u",no_flow_timeout);
               main_menu = 1; menu_no = 0; //test_flag = TRUE; 
               break;
               
               case _RATE_SETTING:
               
               ir_rate[0] = rate_hundredth;
               ir_rate[1] = rate_tenth;
               ir_rate[2] = rate_ones;
               ir_rate[3] = rate_tens;
               
               numeric = 0;
               
               while(enter)
               {
                  restart_wdt();
                  if(numeric)
                  {            
                     numeric = 0; 
                     ir_rate[3] = ir_rate[2]; ir_rate[2] = ir_rate[1]; 
                     ir_rate[1] = ir_rate[0]; ir_rate[0] = command;
                  }
                  
                  clear_segments();
                  
                  lcd_data[8] = 0x80 | ir_rate[3];
                  lcd_data[1] = 0x10 | ir_rate[2];
                  lcd_data[3] = 0x30 | ir_rate[1];          
                  lcd_data[4] = 0x40 | ir_rate[0];                  
    
                  
                  unit_price = _mul(ir_rate[3],1000) + _mul(ir_rate[2],100) +
                              _mul(ir_rate[1],10) + ir_rate[0];
                  show_on_lcd();               
                                       
                  if(command==0x5C)
                  {                                                 
                     enter=0; 
                     rate_hundredth=ir_rate[0];
                     rate_tenth=ir_rate[1];
                     rate_ones=ir_rate[2];
                     rate_tens=ir_rate[3];
                     
                     DEBUG_DATA("\r\nunit price=%Lu",unit_price);
                     
                     write_8_bit_values(rate_tens,_EE_RATE_TENS); delay_cycles(4);
                     write_8_bit_values(rate_ones,_EE_RATE_ONES); delay_cycles(4);
                     write_8_bit_values(rate_tenth,_EE_RATE_TENTH);  delay_cycles(4);
                     write_8_bit_values(rate_hundredth,_EE_RATE_HUNDREDTH);
                     delay_cycles(4);
                  } 
               }
               main_menu=1;
               break;
            }
         }
         break;
         
         case _SHIFT_TOTALIZER:
         
         lakh=0; tenthousand=0; thousand=0; hundred=0;
         tens=0; ones=0; tenth=0; hundredth=0;
         
         clear_segments();
//!         DEBUG_DATA("\r\nResetable Totalizer");
         enter=1;time_start=1;
         remaining_time = _REMAINING_TIMEOUT;
         
         temp32 = shift_totalizer;
//!         while(temp32>=100000000)   { temp32-=100000000; tenthousand++; }
//!         while(temp32>=10000000)    { temp32-=10000000;  thousand++; }
//!         while(temp32>=1000000)     { temp32-=1000000;   hundred++; }
//!         while(temp32>=100000)      { temp32-=100000;    tens++; }
//!         while(temp32>=10000)       { temp32-=10000;     ones++; }
//!         while(temp32>=1000)        { temp32-=1000;      tenth++; }
//!         while(temp32>=100)         { temp32-=100;       hundredth++;}
//!         
//!         lcd_data[0]=0x00 | hundredth;
//!         lcd_data[2]=0x20 | tenth;
//!         lcd_data[7]=0x70 | ones;
//!         
//!         if(shift_totalizer>=1000)       lcd_data[6]=0x60 | tens;
//!         if(shift_totalizer>=10000)      lcd_data[5]=0x50 | hundred;
//!         if(shift_totalizer>=100000)     lcd_data[15]=0xF0 | thousand;
//!         if(shift_totalizer>=1000000)    lcd_data[12]=0xC0 | tenthousand;
//!         if(shift_totalizer>=10000000)   lcd_data[13]=0xD0 | lakh;
         
         while( temp32>=100000 ) { temp32-=100000; lakh++;}
         while( temp32>=10000 )  { temp32-=10000;  tenthousand++;}
         while( temp32>=1000 )   { temp32-=1000;   thousand++;}
         while( temp32>=100 )    { temp32-=100;    hundred++;}
         while( temp32>=10 )     { temp32-=10;     tens++;}
         ones = temp32;
         
         lcd_data[7] = 0x70 | ones;
         
         if(shift_totalizer>=10)       lcd_data[6]=0x60 | tens;
         if(shift_totalizer>=100)      lcd_data[5]=0x50 | hundred;
         if(shift_totalizer>=1000)     lcd_data[15]=0xF0 | thousand;
         if(shift_totalizer>=10000)    lcd_data[12]=0xC0 | tenthousand;
         if(shift_totalizer>=100000)   lcd_data[13]=0xD0 | lakh;
         
         
         temp32 = remain_shift_value;
         while( temp32>=1000 )   { temp32-=1000;   tenth++;}
         while( temp32>=100 )    { temp32-=100;    hundredth++;}
         
         lcd_data[0]=0x00 | hundredth;
         lcd_data[2]=0x20 | tenth;
         
         show_on_lcd();
         
         while(enter)
         {
            restart_wdt();
            if(command==0x00)
            {
               inc_var++;
               command=0x99;     //Change the command to the value other than '0' 
                                 //to avoid re-entering
               if(inc_var>=5)
               {
                  DEBUG_DATA("\r\nTotalizer has been reset");
                  inc_var=0;
                  
                  mech_count = 0;
                  remain_mech_value = 0;
                  
                  temp1_shift = 0;
                  shift_totalizer = 0;
                  remain_shift_value = 0;                  
                  write_32_bit_values( shift_totalizer, _EE_SHIFT_TOTALIZER);
                  write_32_bit_values( remain_shift_value, _EE_REMAIN_SHIFT_VALUE);
                  write_32_bit_values( remain_mech_value, _EE_REMAIN_MECH_VALUE );
                  write_32_bit_values( mech_count, _EE_MECH_COUNT);
                  clear_segments();                                      
                  
                  //Display total_kg 0000 when reset 
                  lcd_data[0] = 0x00; lcd_data[2] = 0x20; lcd_data[7] = 0x70; 
                  show_on_lcd();
               }
            }
            
            if(ok_key || !time_start) { enter=0; time_start=0; ok_key=0; }
         }
         main_menu = 1; //test_flag = TRUE; 
         break;
         
         case _MASTER_TOTALIZER:
         
         lakh=0; tenthousand=0; thousand=0; hundred=0;
         tens=0; ones=0; tenth=0; hundredth=0;
         
         clear_segments();
//!         DEBUG_DATA("\r\nNon Resetable Totalizer");
         enter=1; time_start=1;
         remaining_time = _REMAINING_TIMEOUT;
         
         temp32 = master_totalizer;
         
         while(temp32>=100000000)   { temp32-=100000000; lakh++; }
         while(temp32>=10000000)    { temp32-=10000000;  tenthousand++; }
         while(temp32>=1000000)     { temp32-=1000000;   thousand++; }
         while(temp32>=100000)      { temp32-=100000;    hundred++; }
         while(temp32>=10000)       { temp32-=10000;     tens++; }
         while(temp32>=1000)        { temp32-=1000;      ones++; }
         while(temp32>=100)         { temp32-=100;       tenth++;}
         while(temp32>=10)          { temp32-=10;        hundredth++;}
         
         lcd_data[0]=0x00 | hundredth;
         lcd_data[2]=0x20 | tenth;                                               
         lcd_data[7]=0x70 | ones;
         
         if(master_totalizer>=1000)       lcd_data[6]=0x60 | tens;
         if(master_totalizer>=10000)      lcd_data[5]=0x50 | hundred;
         if(master_totalizer>=100000)     lcd_data[15]=0xF0 | thousand;
         if(master_totalizer>=1000000)    lcd_data[12]=0xC0 | tenthousand;
         if(master_totalizer>=10000000)   lcd_data[13]=0xD0 | lakh;
         
         show_on_lcd();
         
         while(enter)
         {
            restart_wdt();
            if(ok_key || !time_start) { enter=0; time_start=0; ok_key=0; }
         }
         main_menu = 1; //test_flag = TRUE; 
         
         break;                                                                
      }
   }
}

void parameters_value(int &value, int menu_number, int1 fine_tuning)
{
   int temp;
   
   time_start=1;
   remaining_time  = _REMAINING_TIMEOUT;
   
   while(enter)
   {
      restart_wdt();
      if(fine_tuning)
      {
         if(command==UP_KEY)       value+=1;
         else if(command==DOWN_KEY)  value-=1;
         else if(command==RIGHT_KEY)  value+=10;
         else if(command==LEFT_KEY)  value-=10;
      }
      else
      {
         if(value%10)   value = value - (value%10);
         if(command==RIGHT_KEY)  value+=10;
         else if(command==LEFT_KEY)  value-=10;
      }
      
      if(any_key_pressed) 
      {
         any_key_pressed=FALSE;
         remaining_time  = _REMAINING_TIMEOUT;
      }
      
      command=0x99;
      
      hundred=0;tens=0;ones=0;
      clear_segments();
      
      temp=value;
      
      while(temp>=100)  { temp-=100; hundred++; }
      while(temp>=10)   { temp-=10;  tens++; }
      ones=temp;
                  
                     lcd_data[5]=0x50  | ones;
      if(value>=10)  lcd_data[15]=0xF0 | tens;
      if(value>=100) lcd_data[12]=0xC0 | hundred;
                     lcd_data[7]=0x70  | menu_number;
                                            
      show_on_lcd();
      
      if(command==0x5C || time_start==0)  { enter=0; time_start=0; }
   }
}

void parameters_value(int16 &value, int menu_number, int1 fine_tuning)
{
   int16 temp;
   
   time_start=1;
   remaining_time  = _REMAINING_TIMEOUT;
   
   while(enter)
   {
      restart_wdt();
      if(fine_tuning)
      {
         if(command==UP_KEY)       value+=1;
         else if(command==DOWN_KEY)  value-=1;
         else if(command==RIGHT_KEY)  value+=10;
         else if(command==LEFT_KEY)  value-=10;
      }
      else
      {
         if(value%10)   value = value - (value%10);
         if(command==RIGHT_KEY)  value+=10;
         else if(command==LEFT_KEY)  value-=10;
      }
      
      if(any_key_pressed) 
      {
         any_key_pressed=FALSE;
         remaining_time  = _REMAINING_TIMEOUT;
      }
      
      command=0x99;
      
      thousand=0;hundred=0;tens=0;ones=0;
      clear_segments();
      
      temp=value;
      
      while(temp>=1000) { temp-=1000; thousand++; }
      while(temp>=100)  { temp-=100; hundred++; }
      while(temp>=10)   { temp-=10;  tens++; }
      ones=temp;
                  
                     lcd_data[5]=0x50  | ones;
      if(value>=10)  lcd_data[15]=0xF0 | tens;
      if(value>=100) lcd_data[12]=0xC0 | hundred;
      if(value>=1000)lcd_data[13]=0xD0 | thousand;
                     lcd_data[7]=0x70  | menu_number;
                                            
      show_on_lcd();
      
      if(command==0x5C || time_start==0)  { enter=0; time_start=0; }
   }
}

void parameters_value_ml(int &value, int1 &flag, int menu_number)
{
   time_start=1;
   remaining_time  = _REMAINING_TIMEOUT;
   
   while(enter)
   {
      restart_wdt();
      
      if(command==UP_KEY)
      {
         if(value>9) { value = 0; flag = 1;}
         else value+=1;
      }
      else if(command==DOWN_KEY)
      {
         if(value==0) { value = 9; flag = 0;}
         else value-=1;
      }
      
      if(any_key_pressed) 
      {
         any_key_pressed=FALSE;
         remaining_time  = _REMAINING_TIMEOUT;
      }
      
      command=0x99;
      clear_segments();

      if(flag) lcd_data[13]=0xD1;
      else lcd_data[13]=0xD0;

      lcd_data[5]=0x50  | value;

      show_on_lcd();
      
      if(command==0x5C || time_start==0)  { enter=0; time_start=0; }
   }
}

/*************************************************************************************
**    Project  : Petrol Dispenser (Processor Card)                                  **
**    Author   : Abdul hayee                                                        **        
**    Version  : V1.0                                                               **
**    Date     : 28-Oct-2015                                                        **  
**  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  **
**                                                                                  **
**    Pin Assignments:                                                              **
**    ¯¯¯¯¯¯¯¯¯¯¯¯|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|¯¯¯¯¯¯¯¯¯¯¯¯¯|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯  **
**    Pin Number  |  Name Given     |  Direction  | Purpose                         **
**    ¯¯¯¯¯¯¯¯¯¯¯¯|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|¯¯¯¯¯¯¯¯¯¯¯¯¯|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯  **
**      PIN_A1    |   STROBE_L      |   Output    | Strobe pin for lcd              **
**      PIN_A2    |   CLOCK_PIN     |   Output    | Clock pin for lcd and keypad    **
**      PIN_A3    |   DATA_PIN      |   Output    | Data pin for lcd and keypad     **
**      PIN_A4    |   STROBE_K      |   Output    | Strobe pin for keypad           **
**      PIN_A5    |   POWER_SENSE   |   Input     | Power sense pin                 **
**      PIN_B1    |   NOZZLE        |   Input     | Nozzle                          **
**      PIN_B2    |   IR_PIN        |   Input     | IR sensor data capture pin      **
**      PIN_B3    |   KEY_PRESSED   |   Input     | keypad "key pressed" pin        **
**      PIN_B4    |   MECH_COUNTER  |   Output    | Mechanical counter pin          **
**      PIN_C0    |   MOTOR_STATE   |   Output    | Motor ON/OFF                    **
**      PIN_C1    |   SLOW_SOL      |   Output    | Slow solenoid pin               **
**      PIN_C2    |   FAST_SOL      |   Output    | Fast solenoid pin               **
**  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  **
**                                                                                  **
**    Interrupts:                                                                   **
**    INT_EXT     -->   Interrupt for capturing pulses                              **
**    INT_TIMER3  -->   100mS interrupt for different tasks                         **
**    INT_EXT2    -->   |                                                           **
**    INT_TIMER1  -->   | EXT2 & TIMER1 works together to capture IR Stream         **
**  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  **
**                                                                                  **
**    This program routine have the following main processes                        **
**    [+] Filling according to Rupees & Liters entered                              **
**    [+] Store Last 10 fillings (only liter values)                                **
**    [+] DotPoint Shifted toward Right side by 1 place of Price LCD.               **
**    [+] Filling Stops if 990 Lts of Fuel filled.                                  **
**    [+] Delay is added in Displaying 888 Pattern when nozzle is lifted.           **
**    [+] Deactivated PowerSense Signal for 5sec whenever filling starts (b/c when  **
**        motor will start maybe there will be fluctuation in AC voltage)           **
**************************************************************************************/

/*-----Include files-----*/
#include "petrol_prog.h"
#include "functions.c"
#include "ir_functions.c"
#include "ir_interrupt.c"
#include "keypad_routine.c"
#include "format_data.c"
/*-----------------------*/

/*==================================================================================*/
/***
   Funtion     :  void EXT_isr(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  Count no of pulses and do some calculations.
***/   
/*==================================================================================*/
#int_EXT
void  EXT_isr(void) 
{
   if(filling_start)
   {
      no_flow_count = no_flow_timeout; // reset no flow time everytime interrupt occurs.
      
      if(pulses_ignore_flag)           // pulses ignore flag? TRUE
      {
         if(pulses_to_ignore)          // if pulses to ignore > 0
         { 
            pulses_to_ignore--; 
            ignored_pulses++;
         }
         else if(false_p_time)         // Note 1.
         {
            false_p_time = 0;

            pulses_counter = ignored_pulses;
            mech_count+=ignored_pulses;            // add ignored_pulses value to all 3
            temp1_shift+= ignored_pulses;          // totalizers. 
            
            ignored_pulses = ignored_pulses * 100; // scale up the ignored pulses for 
                                                   // exact calculations.
            
            temp1_master+= ignored_pulses;         // totalizers values.
                        
            pulses_ignore_flag = FALSE;            // 
         }
      }
      
      if(!pulses_ignore_flag)                   //pulses ignore flag? FALSE
      {   
         if(full_filling) pulses_counter++;     // if full filling performed.
         else                                   // if filling performed according to 
         {                                      // rupees or liter.
            if(pulses_counter < required_pulses) pulses_counter++;
         }
         
         mech_count++;                          // increment in mech counter.
         temp1_shift++;                         // increment in temp shift totalizer. 

         temp1_master = temp1_master + 100;     // scale of 100.
                  
         temp_mech_count = (mech_count * calib_value);   // for exact mechanical counter
                                             // value multiply it with calibration value.
         
         temp_mech_count = temp_mech_count + remain_mech_value;   // add previous
                                       // remainder in new value for exact calculations.
         
         new_filling = TRUE;           // This variable is used to save data when power
                                       // Turn OFF.
                        
         if(temp_mech_count >= 10000)  // If >= 1 liter filled 
         { 
            output_low(MECH_COUNTER);                    // Turn ON mech counter.
            mech_counter_time = _MECH_COUNTER_TIME;      // set time for mech counter. 
            mech_count = 0;                              // reset variable.
            remain_mech_value = temp_mech_count - 10000; // save remainder in a variable.
         }
         load_values_in_ram = TRUE;                // Indicates new filling occured.
         
         totalizer_calculations = TRUE;            // Indicates to calculate totalizers
                                                   // after filling.
      }
   }
}



/*==================================================================================*/
/***
   Funtion     :  void timer3_interrupt(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  This Timer is used for timing purpose. Timer interrupts after every
                  100ms.
***/   
/*==================================================================================*/
#INT_TIMER3
void timer3_interrupt(void)
{
   set_timer3(53035);                        // value to be loaded for 100mS interrupt.
   
   if(menu)                                  // if menu key is pressed form remote.
   {
      msec_100++;                            // increment counter.
      if(msec_100 == _MENU_KEY_TIME)         // if time = defined time.                 
      {      
         msec_100=0; menu=0; main_menu=1; inc_var=0; numeric=0;   // Set/Reset variables.
      }
   }
   
   if( filling_start )                       // filling process starts?
   {
      if(pulses_ignore_flag)                 // pulses to ignore flag? TRUE
      {
         if(!pulses_to_ignore) false_p_time--;  // Note 2. 
         
         if(!false_p_time) { pulses_ignore_flag = FALSE; ignored_pulses=0; }  // Note 3.
      }   
   }
   
   if(mech_counter_time) mech_counter_time--;// if mech counter time other than 0 then
                                             // decrement counter value.
   else output_high(MECH_COUNTER);           // Turn OFF mech counter.
  
   milli_sec++;
   if(milli_sec==10)                         // equals to 1 sec.
   {
      milli_sec=0;                           // reset variable to calculate next sec.
      
      if(time_start)                         // indicates that the user is in any menu
      {                                      // in IR routine.
         remaining_time--;                   // decrement allowable time of menu's in IR.
         if(!remaining_time) time_start=0;
      }
      
      if(keypad_timeout==1)  { kp_n=10; keypad_timeout--; }
      else if(keypad_timeout)   keypad_timeout--;  // decrement allowable time of keypad 
      else kp_n=0;
      
      if(filling_start) 
      {
         if(pulses_counter>=5)         // After filling some ml, no flow timeout will
         {                             // be activated.
            if(no_flow_count == 1)     // comparing with 1 is done for exact timing
            {                          // entered by remote.
               filling_start = FALSE;        // Stop filling process.
               filling_performed = TRUE;     // Filling is performed.
               no_flow_count--;              // make this variable 0.
               solenoid_and_motor_off_ISR(); // Trun OFF motor and solenoids.
//!               testing_variable = 1;
            }
            else if(no_flow_count)  no_flow_count--;
         }
         else if(pulses_counter<5)     //Motor timeout will be activated when filling 
         {                             //starts and remains active when filled fuel is 
                                       //less than 50ml
            if(motor_count == 1)       //comparing with 1 is done for exact timing entered
            {                          //by remote.    
               filling_start = FALSE;        // Stop filling process.
               filling_performed = TRUE;     // Filling is performed.
               motor_count--;                // Make this variable 0.
               solenoid_and_motor_off_ISR(); // Turn OFF motor and solenoids.
               testing_variable = 2;
            }
            else if(motor_count) motor_count--;
         }
      }
      
      if(time_to_inactive_psense) time_to_inactive_psense--;
   }
}



/*==================================================================================*/
/***
   Funtion     :  void check_nozzle(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  check if Nozzle is Lifted or not. if Lifted then perform filling or
                  go to IR routine. if not Lifted then scan keypad.
***/   
/*==================================================================================*/
void check_nozzle(void)
{
   int k;
   
   if(input(NOZZLE))                      // NOZZLE is Lifted?
   {         
      restart_wdt();
      nozzle_counter++;
                                          // condition ? TRUE, then show the 888 pattern.
      if(check_disp_pattern && nozzle_counter>=1000)
      {
         display_888_pattern();
         delay_250(); delay_250();        // some delay.
         if(!input(NOZZLE)) ir_routine(); // If nozzle is put back then goto IR routine.
         else                             // else ready for filling process.
         { 
            clear_888_pattern();          // clear the 888 pattern.
            delay_250(); delay_250();
            initialize_parameters = TRUE;
            check_disp_pattern = FALSE;
            nozzle_counter=0;
         }
      }
      
      if(initialize_parameters)
      {
         
         //***************************shifting history values****************************
         for (k=0; k < (_MAX_LENGTH-1); k++)
         memmove(ltr_history+(_MAX_LENGTH-1-k),ltr_history+(_MAX_LENGTH-2-k),4);
         
         for (k=0; k < (_MAX_LENGTH-1); k++) 
         memmove(rs_history+(_MAX_LENGTH-1-k),rs_history+(_MAX_LENGTH-2-k),4);
         
         for (k=0; k < (_MAX_LENGTH-1); k++) 
         memmove(rate_history+(_MAX_LENGTH-1-k),rate_history+(_MAX_LENGTH-2-k),2);
         
         
         for(k=0; k< (_LTR_10_ARRAY-1); k++)
         memmove(ltr_10_history+(_LTR_10_ARRAY-1-k),ltr_10_history+(_LTR_10_ARRAY-2-k),4);
         //******************************************************************************
         
         load_values_in_ram = FALSE;   // These flag will indicate if new filling 
         load_ram_flag = TRUE;         // occured or not. if new filling occured then 
                                       // store the last filling in latest postion of an
                                       // an array otherwise shift the array back to its
                                       // previous postion.
         
         if(kp_rupees)                 // Are rupees entered?
         {
            full_filling = FALSE;      // Disable full filling.
            
                                       // calculating nos of pulses form entered rupees.
            kp_liters = (kp_rupees * 10000) / unit_price;            
            required_pulses = (kp_liters * 1000) / calibration_value; 
                                                                                                    
                              // if sign flag then subtract pulses form required pulses.
            if(sign_flag) required_pulses = required_pulses - ml_setting;
                              // if no sign flag then add pulses in required pulses.                                       
            else required_pulses = required_pulses + ml_setting;
            
                              // Calculating Fast solenoid Turn OFF value. 
            if(((signed int32)required_pulses - (signed int32)fast_sol_turnoff_margin) > 0)
            {
               FS_turn_off_pulses = required_pulses - fast_sol_turnoff_margin;
            }
            else FS_turn_off_pulses = 0 ;
                              
                              // Calculating Slow solenoid Turn OFF value.
            if(((signed int32)required_pulses - (signed int32)slow_sol_turnoff_margin) > 0)
            {
               LS_turn_off_pulses = required_pulses - slow_sol_turnoff_margin;
            }
            else LS_turn_off_pulses = 0 ;
            
            DEBUG_DATA("\r\n fast solenoid off at =%ld",required_pulses - fast_sol_turnoff_margin );
            DEBUG_DATA("\r\n fast solenoid off at =%ld",(signed int32)required_pulses - (signed int32)fast_sol_turnoff_margin);
            DEBUG_DATA("\r\n slow solenoid off at =%ld",(signed int32)required_pulses - (signed int32)slow_sol_turnoff_margin);
            DEBUG_DATA("\r\n amount : kp_liters=%lu",kp_liters);
            DEBUG_DATA("\r\n Actual Liters : %1.2w",kp_liters);   
            DEBUG_DATA("\r\n liters : ml_setting=%u", ml_setting);
            DEBUG_DATA("\r\n liters : sign_flag=%u", sign_flag);
            DEBUG_DATA("\r\n amount : required_pulses=%lu",required_pulses);
            DEBUG_DATA("\r\n amount : slow_solenoid_only=%u",slow_solenoid_only);
            
         }
         else if(kp_liters)            // Are liters entered? 
         {
            full_filling = FALSE;      // Disable full filling.
            required_pulses = kp_liters * 100;  // 1.00 liter = 100 pulses in real world.
                                       
                                       // calculating nos of pulses form entered liters.
            required_pulses = ( required_pulses * 1000) / calibration_value;
            
                              // if sign flag then subtract pulses form required pulses.
            if(sign_flag) required_pulses = required_pulses - ml_setting;
                              // if no sign flag then add pulses in required pulses.
            else required_pulses = required_pulses + ml_setting;
            
                              // Calculating Fast solenoid Turn OFF value.
            if(((signed int32)required_pulses - (signed int32)fast_sol_turnoff_margin) > 0)
            {
               FS_turn_off_pulses = required_pulses - fast_sol_turnoff_margin;
            }
            else FS_turn_off_pulses = 0 ;
                              
                              // Calculating Slow solenoid Turn OFF value.
            if(((signed int32)required_pulses - (signed int32)slow_sol_turnoff_margin) > 0)
            {
               LS_turn_off_pulses = required_pulses - slow_sol_turnoff_margin;
            }
            else LS_turn_off_pulses = 0 ;
            
            DEBUG_DATA("\r\n fast solenoid off at =%ld",(signed int32)required_pulses - (signed int32)fast_sol_turnoff_margin);
            DEBUG_DATA("\r\n slow solenoid off at =%ld",(signed int32)required_pulses - (signed int32)slow_sol_turnoff_margin);
            DEBUG_DATA("\r\n liters : kp_liters=%lu",kp_liters);
            DEBUG_DATA("\r\n liters : ml_setting=%u", ml_setting);
            DEBUG_DATA("\r\n liters : sign_flag=%u", sign_flag);
            DEBUG_DATA("\r\n liters : required_pulses=%lu",required_pulses);
            DEBUG_DATA("\r\n liters : slow_solenoid_only=%u",slow_solenoid_only);
            
         }
         
         else
         {
            full_filling = TRUE;       // Enable full filling.
            slow_solenoid_only = FALSE;
            
            DEBUG_DATA("\r\n Actual Liters : %1.2w",kp_liters);
            DEBUG_DATA("\r\n full filling : required_pulses=%lu",required_pulses);
            DEBUG_DATA("\r\n full filling : slow_solenoid_only=%u",slow_solenoid_only);
            
         }
         
         output_low(MOTOR_STATE);                        // Motor ON.
         delay_cycles(4);
         output_low(SLOW_SOL);                           // Slow Sol ON.
         delay_cycles(4);
         if(!slow_solenoid_only) output_low(FAST_SOL);   // Fast Sol ON.
         
         time_to_inactive_psense = _TIME_TO_INACTIVE_PSENSE;
         
         pulses_to_ignore = pulse_threshold;             // copy value of pulses to ignore.
         if(pulses_to_ignore) pulses_ignore_flag = TRUE; // if value is greater than 0,
                                                         // then pulses_ignore_flag = TRUE
         else pulses_ignore_flag = FALSE;                // else FALSE
         
                                 // same as calib_value = (calibration_value*100)/1000;
         calib_value = calibration_value/10; 
         
         motor_count = motor_timeout;        // copy value of motor timeout.
         false_p_time = false_pulses_timing; // copy value of ignore pulses timeout.
         no_flow_count = no_flow_timeout;    // copy value of no flow time.
         
         pulses_counter = 0;                 //
         ignored_pulses = 0;                 // set/reset some variables.
         new_filling = FALSE;                //
         filling_start = TRUE;               //
         
         DEBUG_DATA("\r\npulses to ignore=%u",pulses_to_ignore);
         DEBUG_DATA("\r\nfalse_p_time=%u",false_p_time);
         DEBUG_DATA("\r\npulses_ignore_flag=%u",pulses_ignore_flag);
         DEBUG_DATA("\r\n initial mech counter = %u\r\n",mech_count);
         DEBUG_DATA("\r\n calib_value = %lu\r\n",calib_value);
         DEBUG_DATA("temp mech count = %lu\r\n",temp_mech_count);
         DEBUG_DATA("temp mech count = %lu\r\n",remain_mech_value);

//!         if(!no_flow_timeout || no_flow_timeout == 255) no_flow_count = no_flow_timeout;
//!         else no_flow_count = no_flow_timeout + 1;
//!         
//!         if(!motor_timeout || motor_timeout == 255)   motor_count = motor_timeout;
//!         else motor_count = motor_count + 1;
         
         clear_interrupt(int_ext);
         ext_int_edge(0,l_to_h);             // Edge low to high.
         enable_interrupts(int_ext);         // Enable Pulses Interrupt.
         
         initialize_parameters = FALSE;      // Execute this loop only once before every
                                             // filling.
      }

      format_data();                         // Arrangment of Segments.
      show_on_lcd();                         // Display values on LCD.
    
      if(filling_performed)                  // Is filling completed?      
      {
         i2c_flag = 1;
         for(blink=0; blink<2; blink++)      // Blink LCD 2 times to show the filling is 
         {                                   // completed.
            clear_lcd();   delay_250(); delay_250();
            show_on_lcd(); delay_250(); delay_250();
         }
         filling_performed = FALSE;
         kp_n=10; value_from_kp();            // set/reset some variables here.
         
         totalizers_calculations();          // calculate totalizers value.
         
         temp2_master = ( temp1_master * calibration_value )/10000;
         master_totalizer = master_totalizer + temp2_master;
         temp1_master = 0;
         
         totalizer_calculations = FALSE;     // after calculating make this check FALSE.
                                             // This flag will be check in Power Check
                                             // routine and if flag will be TRUE then it 
                                             // will calculate values here and store it
                                             // in EEPROM.
      }
      
//!      if(testing_variable==1)       { DEBUG_DATA("\r\n no flow "); testing_variable=0; }
//!      else if(testing_variable==2)  { DEBUG_DATA("\r\n motor timeout"); testing_variable=0; }
   }
   else
   {
      restart_wdt();
      check_disp_pattern = TRUE;             // Make this flag TRUE. so that when again
                                             // Nozzle lifted then 888 pattern will be
                                             // shown on LCD.
      nozzle_counter=0;
      if(filling_start)                      // if filling_start is TRUE and Nozzle is 
      {                                      // put back then do the following steps.
         
         solenoid_and_motor_off();           // TURN-OFF motor and solenoids.
         filling_start = FALSE;  filling_performed = FALSE; // |reset variables.
         required_pulses = 0;                               // |

         for(blink=0; blink<2; blink++)      // Blink the LCD 2 times to show the filling
         {                                   // is ended.
            clear_lcd();   delay_250(); delay_250();
            show_on_lcd(); delay_250(); delay_250();
         }
         totalizers_calculations();          // calculate totalizers value.
         
         temp2_master = ( temp1_master * calibration_value )/10000;
         master_totalizer = master_totalizer + temp2_master;
         temp1_master = 0;
         
         totalizer_calculations = FALSE;     // after calculating make this check FALSE.
                                             // This flag will be check in Power Check
                                             // routine and if flag will be TRUE then it 
                                             // will calculate values here and store it
                                             // in EEPROM.
         i2c_flag = 1;
      }
      
      if( load_values_in_ram && load_ram_flag)  // if filling performed and load_ram_flag 
      {                                         // is TRUE.
         
         load_ram_flag = FALSE;              // make it FALSE so that values updated  
                                             // in ram only once after every filling.
         
         rs_history[0]  = amount;            // save last value of amount in history[0]. 
         ltr_history[0] = liters;            // save last value of liters in history[0].
         rate_history[0]= unit_price;        // save last value of unit price in 
                                             // history[0].
         ltr_10_history[0] = liters;
         
         DEBUG_DATA("******************");
         DEBUG_DATA("\r\nFilling");
         for (k=0; k < _MAX_LENGTH; k++)
         {
            DEBUG_DATA("\r\n k=%u ",k);
            DEBUG_DATA(" liters = %lu ",ltr_history[k]);
            DEBUG_DATA(" rs = %lu ",rs_history[k]);
            DEBUG_DATA(" unit_price = %lu",rate_history[k]);
         }
         DEBUG_DATA("\r\n ignored pulses = %lu",ignored_pulses);
         DEBUG_DATA("\r\n pulses counter = %lu",pulses_counter);
         DEBUG_DATA("\r\n mech count = %lu",mech_count);
         DEBUG_DATA("\r\n temp mech count = %lu",temp_mech_count);
         DEBUG_DATA("\r\n remain mech value = %lu",remain_mech_value);
         DEBUG_DATA("\r\n shift total = %lu", shift_totalizer);
         DEBUG_DATA("\r\n remain shift value = %lu", remain_shift_value);
         DEBUG_DATA("\r\n******************");
      }
      else if( !load_values_in_ram && load_ram_flag)  // if filling not performed and 
      {                                               // load ram flag is TRUE.
         load_ram_flag = FALSE;              // make it FALSE so that values updated  
                                             // in ram only once.
         
         // Shift back all the values in history.
         liters = ltr_history[0];
         for (k=0; k < (_MAX_LENGTH-1); k++) memmove(ltr_history+k,ltr_history+(k+1),4);
         
         amount = rs_history[0];
         for (k=0; k < (_MAX_LENGTH-1); k++) memmove(rs_history+k,rs_history+(k+1),4);
         
//!         unit_price = rate_history[0];
         for (k=0; k < (_MAX_LENGTH-1); k++) memmove(rate_history+k,rate_history+(k+1),2);
         
         
         for (k=0; k < (_LTR_10_ARRAY-1); k++) memmove(ltr_10_history+k,ltr_10_history+(k+1),4);
         
         DEBUG_DATA("******************");
         DEBUG_DATA("\r\n Non Filling");
         for (k=0; k < _MAX_LENGTH; k++)
         {
            DEBUG_DATA("\r\n k=%u ",k);
            DEBUG_DATA(" liters = %lu",ltr_history[k]);
            DEBUG_DATA(" rs = %lu",rs_history[k]);
            DEBUG_DATA(" unit_price = %lu",rate_history[k]);
         }
         DEBUG_DATA("******************");
         DEBUG_DATA("\r\n liters = %lu ",liters);
         DEBUG_DATA("\r\n rs = %lu ",amount);
         DEBUG_DATA("\r\n unit_price = %lu",unit_price);
         DEBUG_DATA("\r\n ignored pulses = %lu",ignored_pulses);
         DEBUG_DATA("\r\n pulses counter = %lu",pulses_counter);
         DEBUG_DATA("\r\n mech count = %lu",mech_count);
         DEBUG_DATA("\r\n temp mech count = %lu",temp_mech_count);
         DEBUG_DATA("\r\n temp mech count = %lu",remain_mech_value);
         DEBUG_DATA("\r\n shift total = %lu", shift_totalizer);
         DEBUG_DATA("\r\n remain shift value = %lu", remain_shift_value);
         DEBUG_DATA("\r\n******************");
      }
      read_keypad();             // scans keypad
      if(kp_pressed)             // any key pressed ?
      {
         clear_segments();
         
         value_from_kp();
         if(!ltr_selection)   {lcd_data[7]=0x70;}
         else  { lcd_data[3] = 0x30; lcd_data[4] = 0x40; }
      }                                                           
      else  format_data();          //Arrangments of segments
      show_on_lcd();
      
      if(show_liter_history_flag) 
      {
         show_liter_history();
      }
   }
}



/*==================================================================================*/
/***
   Funtion     :  void main(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  Main loop of the program, handles all the process.
***/   
/*==================================================================================*/
void main(void)
{
   delay_ms(1000);
   /*========================================================================*/
   /*
      This loop will execute for 2 sec and check if there is a fluctuation in the
      power or power is turnig ON/OFF frequently, then it will stay here as long as 
      problem is not settle down.
   */
   do
   {
      power_check();    // check if power is available or not.
      if(p_occured)     // Power is OFF, restart power_loop_counter.  
      {                                                                
         power_loop_counter=0; 
         p_occured=FALSE; 
      }
      delay_ms(10);
      power_loop_counter++;    
   }
   while(power_loop_counter<200);
   two_sec_passed=TRUE;
   /*========================================================================*/
   
   
   io_init();           // Initialize IO states and directions. This step is essential
                        // since we are using FAST_IO() preprocessor directive.
   
   peripheral_init();   // Function for initialization of peripherals.
   parameters_init();   // Retreive data form EEPROM and store in variables.
   
   DEBUG_DATA("\r\nSoftware Compiled on %s\r\n\r\n",__TIME__);
   
   while(TRUE)
   {  
      restart_wdt();    
      check_nozzle();   // Check if Nozzle is LIFTED or NOT.  
      
      if(i2c_flag)
      {
         i2c_data = liters; 
         output_float(i2c_select);
         if(input(i2c_select))
         {
            i2c_start(); // begin transmission
            i2c_write(0xA0); // slave address for R/W 
            i2c_write(i2c_data>>24); // write actual data to slave
            i2c_write(i2c_data>>16); //sending 32bit data byte by byte
            i2c_write(i2c_data>>8);
            i2c_write(i2c_data);
            i2c_stop(); // stop condition
            i2c_flag = 0;
         }
      }
      
      if(!time_to_inactive_psense)  power_check(); // Check if POWER is available or not.
   }
}

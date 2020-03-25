/*==================================================================================*/
/***
   Funtion     :  void read_keypad(void)
   Parameters  :  None
   Return value:  None
   Purpose     :  Selects keypad columns one by one and scan if any key is pressed.
                  This function has sub functions like dataout_kp(); scan_keypad();
                  which all works togeather to scan keypad.
***/   
/*==================================================================================*/
void read_keypad(void)
{
   for(col_loop=0; col_loop<4; col_loop++)   // selects column one by one.
   {                                             
      dataout_kp(col_data[col_loop]);
      output_high(STROBE_K);
      delay_us(10);                    
      output_low(STROBE_K);      
   }
}



/*==================================================================================*/
/***
   Funtion     :  void dataout_kp(void)
   Parameters  :  kp_val 
   Return value:  None
   Purpose     :  This function is used to output every column value and scan if key
                  is pressed or not.
***/   
/*==================================================================================*/
void dataout_kp(int kp_val)
{
   int8 z=0;
   
   for(z=0; z<8; z++)
   {
      kp_value_flag = bit_test(kp_val,z);
      output_bit(DATA_PIN,kp_value_flag);
      
      output_low(CLOCK_PIN); clock_status=0;
      delay_us(10);
      if(!input(KEY_PRESSED)) //Key pressed 
      {
         keypad_timeout = _KEYPAD_TIME_OUT + 1;
         kp_pressed = TRUE;
         
         col=col_loop;
         scan_keypad();
         while(!input(KEY_PRESSED)) { restart_wdt(); delay_ms(10); }//delay for debouncing
      }
      
      output_high(CLOCK_PIN); clock_status=1;
      delay_us(10);
      if(!input(KEY_PRESSED)) //Key pressed
      {
         keypad_timeout = _KEYPAD_TIME_OUT + 1;
         kp_pressed = TRUE;
         
         col=col_loop;
         scan_keypad();
         while(!input(KEY_PRESSED)) { restart_wdt(); delay_ms(10); }//delay for debouncing
      }
   }   
}

void scan_keypad(void)
{
   switch(col)
   {
      case 0:               
         if(!kp_value_flag)    
         {
            if(clock_status)           //Preset P2=150 is pressed
            {
               kp_n=8;                                         
               kp_data[5]=0; kp_data[4]=0; kp_data[3]=1; kp_data[2]=5; kp_data[1]=0;
            }
            else
            {
               kp_n=6;                 //Preset P1=50 is pressed                                            
               kp_data[5]=0; kp_data[4]=0; kp_data[3]=0; kp_data[2]=5; kp_data[1]=0;
            }   
            preset_key = TRUE;
         }
         else
         {
            if(clock_status)
            {
               kp_n=9;                 //Preset P3=200 is pressed                 
               kp_data[5]=0; kp_data[4]=0; kp_data[3]=2; kp_data[2]=0; kp_data[1]=0;
            }
            else
            {
               kp_n=7;                 //Preset P4=100 is pressed                 
               kp_data[5]=0; kp_data[4]=0; kp_data[3]=1; kp_data[2]=0; kp_data[1]=0;
            }
            preset_key = TRUE;
         }
      break;
   
      case 1:               
         if(!kp_value_flag)
         {
            if(clock_status)  { key=7; history_no = 7;}   //key 7 is pressed
            else              { key=1; history_no = 1;}  //key 1 is pressed                 
            preset_key = FALSE;
         }
         else
         {
            if(clock_status)            
            {
               ltr_selection = ~ltr_selection;//Toggle selection everytime when key
                                                    //pressed.
               kp_data[5]=0; kp_data[4]=0;  kp_data[3]=0;  kp_data[2]=0;  kp_data[1]=0;
               kp_n = 0; kp_value = 0; kp_rupees = 0; kp_liters = 0;
               history_counter = 0;
               while(!input( KEY_PRESSED ))
               {
                  restart_wdt();
                  power_check();
                  history_counter++;
                  delay_ms(10);
                  if(history_counter>=250) 
                  { 
                     show_liter_history_flag = TRUE; 
                     history_counter = 0;
                  }
               }
//!               keypad_timeout = KEYPAD_TIME_OUT + 1;
//!               keypad_cntr = TRUE; preset_key = TRUE;
//!               
//!               while(keypad_cntr)
//!               {  //power down scenario? 
//!                  restart_wdt();
//!                  weight=ram_weight[1];
//!                  price=ram_rupees[1];
//!                  format_data();
//!                  show_on_lcd();
//!               }
//!               weight=ram_weight[0];
//!               price=ram_rupees[0];
//!               format_data();
//!               show_on_lcd();
//!               kp_n=10;
            }
            else  { key = 4; history_no = 4; preset_key = FALSE; }
         }
      break;
   
      case 2:              
         if(!kp_value_flag)
         {
            if(clock_status)  { key=8; history_no = 8;}
            else              { key=2; history_no = 2;}
            preset_key = FALSE;
         }
         else
         {
            if(clock_status)  { key=0; history_no = 0;}
            else              { key=5; history_no = 5;}
            preset_key = FALSE;
         }
      break;
   
      case 3:  
         if(!kp_value_flag)
         {
            if(clock_status)  { key=9; history_no = 9;}
            else              { key=3; history_no = 3;}
            preset_key = FALSE;
         }
         else
         {
            if(clock_status)  {kp_n=10; history_no = 10;preset_key = TRUE;}  //F2 key is pressed
            else              {key=6;  history_no = 6; preset_key = FALSE;}
         } 
      break;
   }
   if(!preset_key)
   {
      kp_n++;
      
      if(!ltr_selection)
      {
         if(kp_n>5)
         {
            kp_n=1;
            kp_data[5]=0;  kp_data[4]=0;  kp_data[3]=0; 
            kp_data[2]=0;  kp_data[1]=key;
         }
         else
         {
            kp_data[5]=kp_data[4];  kp_data[4]=kp_data[3];  kp_data[3]=kp_data[2]; 
            kp_data[2]=kp_data[1];  kp_data[1]=key;
         }
      }
      else
      {
         if(kp_n>3)
         {
            kp_n=1;
            kp_data[5]=0;  kp_data[4]=0;  kp_data[3]=0; 
            kp_data[2]=0;  kp_data[1]=key;
         }
         else
         {
            kp_data[5]=0;  kp_data[4]=0;
            kp_data[3]=kp_data[2];
            kp_data[2]=kp_data[1];  kp_data[1]=key;
         }
      }
      preset_key = TRUE;
   }
}

void value_from_kp(void)
{
   switch(kp_n)
   {
      case 1:                                                 //1 key is pressed e.g. "9"
      {
         if(!ltr_selection)   lcd_data[6]=0x60 | kp_data[1];
         else  lcd_data[1]=0x10 | kp_data[1];
      }
      break;                            
      
      case 2:                                              //2 keys are pressed e.g. "99"
      {
         if(!ltr_selection)
         {
            lcd_data[6]= 0x60 | kp_data[1];
            lcd_data[5]= 0x50 | kp_data[2];
         }
         else
         {
            lcd_data[1]= 0x10 | kp_data[1];
            lcd_data[8]= 0x80 | kp_data[2];
         }
      }
      break;
      
      case 3:                                             //3 keys are pressed e.g. "999"
      {      
         if(!ltr_selection)
         {
            lcd_data[6]= 0x60 | kp_data[1];
            lcd_data[5]= 0x50 | kp_data[2];
            lcd_data[15]=0xF0 | kp_data[3];
         }
         else
         {
            lcd_data[1]= 0x10 | kp_data[1];
            lcd_data[8]= 0x80 | kp_data[2];
            lcd_data[9]= 0x90 | kp_data[3];
         }
      }
      break;
                                      
      case 4:                                            //4 keys are pressed e.g. "9999"
      {      
         if(!ltr_selection)
         {
            lcd_data[6]= 0x60 | kp_data[1];
            lcd_data[5]= 0x50 | kp_data[2];
            lcd_data[15]=0xF0 | kp_data[3];
            lcd_data[12]=0xC0 | kp_data[4];
         }   
      }
      break;
      
      case 5:                                           //5 keys are pressed e.g. "99999"
      {
         if(!ltr_selection)
         {
            lcd_data[6]= 0x60 | kp_data[1];
            lcd_data[5]= 0x50 | kp_data[2];
            lcd_data[15]=0xF0 | kp_data[3];
            lcd_data[12]=0xC0 | kp_data[4];
            lcd_data[13]=0xD0 | kp_data[5];
         }
      }
      break;
      
      case 6:                          //P1=50 (Display 50 on LCD)
      {
         if(!ltr_selection)
         {
            lcd_data[6]=0x60 | kp_data[1];
            lcd_data[5]=0x50 | kp_data[2];
         }
      }
      break;
      
      case 7:                          //P2=100 (Display 100 on LCD)
      {                                            
         if(!ltr_selection)
         {
            lcd_data[6]= 0x60 | kp_data[1];
            lcd_data[5]= 0x50 | kp_data[2];
            lcd_data[15]=0xF0 | kp_data[3];
         }
      }
      break;
      
      case 8:                          //P3=150 (Display 150 on LCD)
      {
         if(!ltr_selection)
         {
            lcd_data[6]= 0x60 | kp_data[1];
            lcd_data[5]= 0x50 | kp_data[2];
            lcd_data[15]=0xF0 | kp_data[3];
         }
      }
      break;
      
      case 9:                          //P4=200 (Display 200 on LCD)
      {                               
         if(!ltr_selection)
         {
            lcd_data[6]= 0x60 | kp_data[1];
            lcd_data[5]= 0x50 | kp_data[2];
            lcd_data[15]=0xF0 | kp_data[3];
         }
      }
      break;
      
      case 10:                          //Reset all the values.  
      {
         kp_data[1]=0; kp_data[2]=0; kp_data[3]=0; kp_data[4]=0; kp_data[5]=0;
         
         kp_value=0; kp_pressed=FALSE; ltr_selection=FALSE; kp_n=0;
         kp_rupees = 0; kp_liters = 0;
         lcd_data[7] = 0x7A;
         lcd_data[4] = 0x4A;
      }
      break;
   }
   
   if(kp_n>0 && kp_n<10)                 //if kp_n<9 then calculate kp_value for kp_price
   {
      kp_value = _mul(kp_data[5],10000) + _mul(kp_data[4],1000) + _mul(kp_data[3],100) + 
                 _mul(kp_data[2],10) + kp_data[1];
      
      if(!ltr_selection) kp_rupees = kp_value;
      else kp_liters = kp_value;
   }
}

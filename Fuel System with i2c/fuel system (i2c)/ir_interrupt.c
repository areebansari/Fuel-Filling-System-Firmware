#INT_TIMER1
void timer1_interrupt(void)                       
{
   restart_wdt();

   set_timer1(65486);                             
 
   if(!input(IR_PIN))      // If IR pin low
   {                                                    
      irs[state]=0;                
      state++;                             
   }
   else if(input(IR_PIN))  // If IR pin high                    
   {                                                         
      irs[state]=1;                                         
      state++;                            
   }   
                                                          
   if(state>=58)  // complete data stream captured ?
   {                                                    
      disable_interrupts(INT_TIMER1);
      state=0;
  
      if(irs[7]) bit_set(leader,0);
      else bit_clear(leader,0);
      
      if(irs[6]) bit_set(leader,1);
      else bit_clear(leader,1);
      
      if(irs[5]) bit_set(leader,2);
      else bit_clear(leader,2);
      
      if(irs[4]) bit_set(leader,3);
      else bit_clear(leader,3);
      
      if(irs[3]) bit_set(leader,4);
      else bit_clear(leader,4);
      
      if(irs[2]) bit_set(leader,5);
      else bit_clear(leader,5);
      
      if(irs[1]) bit_set(leader,6);
      else bit_clear(leader,6);
      
      if(irs[0]) bit_set(leader,7);
      else bit_clear(leader,7);
     
      if(leader==VALID_LEADER)                                                                   
      {
         if(irs[9]) bit_set(startbit,0);
         else bit_clear(startbit,0);
         
         if(irs[8]) bit_set(startbit,1);
         else bit_clear(startbit,1);
         
         if(startbit==VALID_START)
         {                                       
            if(irs[15]) bit_set(mode,0);
            else bit_clear(mode,0);
            
            if(irs[14]) bit_set(mode,1);
            else bit_clear(mode,1);
            
            if(irs[13]) bit_set(mode,2);
            else bit_clear(mode,2);
            
            if(irs[12]) bit_set(mode,3);
            else bit_clear(mode,3);
            
            if(irs[11]) bit_set(mode,4);
            else bit_clear(mode,4);
            
            if(irs[10]) bit_set(mode,5);
            else bit_clear(mode,5);
                  
            if(mode==VALID_MODE)                                                  
            {
               if(irs[19]) bit_set(toggle,0);
               else bit_clear(toggle,0);
               
               if(irs[18]) bit_set(toggle,1);
               else bit_clear(toggle,1);
               
               if(irs[17]) bit_set(toggle,2);
               else bit_clear(toggle,2);
               
               if(irs[16]) bit_set(toggle,3);
               else bit_clear(toggle,3);
               
               if(toggle==VALID_TOGGLE1 || toggle==VALID_TOGGLE2)
               {                                        
                  if(irs[35]) bit_set(address,0);
                  else bit_clear(address,0);
                  
                  if(irs[33]) bit_set(address,1);
                  else bit_clear(address,1);
                  
                  if(irs[31]) bit_set(address,2);
                  else bit_clear(address,2);
                                                    
                  if(irs[29]) bit_set(address,3);
                  else bit_clear(address,3);                 
                  
                  if(irs[27]) bit_set(address,4);
                  else bit_clear(address,4);
                  
                  if(irs[25]) bit_set(address,5);
                  else bit_clear(address,5);
                  
                  if(irs[23]) bit_set(address,6);
                  else bit_clear(address,6);
                  
                  if(irs[21]) bit_set(address,7);
                  else bit_clear(address,7);
                  
                   
                  if(address==VALID_ADDRESS)
                  {  
                     if(toggle!=prv_toggle)
                     {    
                        prv_toggle=toggle;
                        if(irs[51]) bit_set(command,0);
                        else bit_clear(command,0);
                        
                        if(irs[49]) bit_set(command,1);
                        else bit_clear(command,1);
                        
                        if(irs[47]) bit_set(command,2);
                        else bit_clear(command,2);                
                        
                        if(irs[45]) bit_set(command,3);
                        else bit_clear(command,3);
                        
                        if(irs[43]) bit_set(command,4);
                        else bit_clear(command,4);
                        
                        if(irs[41]) bit_set(command,5);
                        else bit_clear(command,5);
                        
                        if(irs[39]) bit_set(command,6);
                        else bit_clear(command,6);
                        
                        if(irs[37]) bit_set(command,7);
                        else bit_clear(command,7);
                           
                        ir_key_press=1; ir_key_press2=1; any_key_pressed=1;
                        
                        if(command>0x09)
                        {
                           if(command==MENU_KEY)
                           {    
                              menu=1; main_menu=2; sub_menu=1;//Menu key
                           }       
                           else if(command==DISPLAY_TOTAL_KEY)
                           {    
                              main_menu=3;sub_menu=1; //Resetable Totalizer  
                           }       
                           else if(command==AUDIO_300_KEY)
                           {    
                              main_menu=4;            //Non-Resetable Totalizer 
                           }       
                           else if(command==_OK_KEY)
                           {    
                              ok_key=1;               //OK key 
                           }       
                           else if(command==F2_KEY || command==SETUP_KEY)
                           {   
                              sub_menu=1;
                           }       
                           else if(command==TITLE_F3_KEY)
                           {    
                              sub_menu=1;
                           }       
                           else if(command==SUBTITLE_200_KEY)
                           {
                              sub_menu=1;
                           }      
                        }       
                        else if(command>=0x00 && command<=0x09)//numeric key                        
                        {                           
                           sub_menu=1; numeric=1;
                        }       
                     }//toggle!=prv_toggle
                  }//address==VALID_ADDRESS                                       
               }//toggle==VALID_TOGGLE1 || toggle==VALID_TOGGLE2
            }//mode==VALID_MODE                                         
         }//startbit==VALID_START                                         
      }//leader==VALID_LEADER             
   }//state>=58    

}

//*******External2 Interrupt for enabling Timer1 (Used for IR interrupt)*******
#INT_EXT2
void isr_ext2(void)       
{                                                       
   clear_interrupt(INT_TIMER1);
   enable_interrupts(INT_TIMER1);                                     
   set_timer1(65514);                                
}   
//*****************************************************************************


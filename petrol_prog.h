#include <18f252.h>
#fuses HS,PUT,WDT,WDT128,NOLVP,NOBROWNOUT
#use delay(crystal=4M, RESTART_WDT)
#use rs232(baud=19200, UART1)

#include <string.h>

#use FAST_IO(A)
#use FAST_IO(B)
#use FAST_IO(C)

//------------------------------Pin's Assignments------------------------------
#define  I2C_SELECT     PIN_C5      // I2C master selection pin      (OUT)
#define  FAST_SOL       PIN_C2      // fast solenoid pin             (OUT)
#define  SLOW_SOL       PIN_C1      // slow solenoid pin             (OUT)
#define  MOTOR_STATE    PIN_C0      // Motor ON/OFF                  (OUT)

#define  MECH_COUNTER   PIN_B4      // Mechanical counter pin        (OUT)
#define  KEY_PRESSED    PIN_B3      // keypad "key pressed" pin      (IN)
#define  IR_PIN         PIN_B2      // IR sensor data capture pin    (IN)
#define  NOZZLE         PIN_B1      // ON/OFF NOZZLE                 (IN)

#define  POWER_SENSE    PIN_A5      // power sense pin               (IN)
#define  STROBE_K       PIN_A4      // strobe pin for keypad         (OUT)
#define  DATA_PIN       PIN_A3      // data pin for lcd and keypad   (OUT)
#define  CLOCK_PIN      PIN_A2      // clock pin for lcd and keypad  (OUT)
#define  STROBE_L       PIN_A1      // strobe pin for lcd            (OUT)

//---------------------------------IR defines----------------------------------
#define  VALID_LEADER      0x03                                       
#define  VALID_START       0x01                                              
#define  VALID_MODE        0x2A                                                 
#define  VALID_TOGGLE1     0x03                                                                    
#define  VALID_TOGGLE2     0x0C              
#define  VALID_ADDRESS     0x04
//-----------------------------------------------------------------------------

//-----------------------------Remote Key's Assignment-------------------------
#define  MENU_KEY          0xD1
#define  DISPLAY_TOTAL_KEY 0x0f
#define  AUDIO_300_KEY     0x4e
#define  _OK_KEY           0x5C
#define  F2_KEY            0xC8
#define  SETUP_KEY         0x82
#define  TITLE_F3_KEY      0x83
#define  SUBTITLE_200_KEY  0x4B
#define  UP_KEY            0x58
#define  DOWN_KEY          0x59
#define  LEFT_KEY          0x5A
#define  RIGHT_KEY         0x5B
//-----------------------------------------------------------------------------


//-------------------------------Timing Defines--------------------------------
                                                            //Timer3=100mS               
#define  _MENU_KEY_TIME          20 //100mS*value=Req Time  e.g 100mS*20=2 Sec
#define  _KEYPAD_TIME_OUT        60 //1 Sec*value=Req Time  e.g 1 Sec*5=5 Sec
#define  _MECH_COUNTER_TIME      2  //100mS*value=Req Time  e.g 100mS*1=100mS
#define  _REMAINING_TIMEOUT      5  //1 Sec*value=Req Time  e.g 1 Sec*5=5 Sec
#define  _TIME_TO_INACTIVE_PSENSE 5
//-----------------------------------------------------------------------------


//-----------------------------DEFAULT FACTORY SETTINGS VALUES-----------------

#define  _ML_THRESHOLD              2
#define  _MIN_LTRS                  1
#define  _FAST_SOL_TURNOFF_MARGIN   20       // unit : centi Liters (10e-2)
#define  _SLOW_SOL_TURNOFF_MARGIN   0        // unit : centi Liters (10e-2)
#define  _CALIBRATION_VALUE         1020
#define  _MOTOR_TIMEOUT             15
#define  _ML_SETTING                0
#define  _NO_FLOW_TIMEOUT           15
#define  _SIGN_FLAG                 0

#if _FAST_SOL_TURNOFF_MARGIN <= _SLOW_SOL_TURNOFF_MARGIN
   #error "High Solenoid must turnoff before low solenoid"
#endif
//-----------------------------------------------------------------------------               
               
//-----------------------EEPROM LOCATIONS FOR SAVING PARAMETERS----------------

//       Parameters                    Location    No of bytes
#define  _EE_FIRST_RUN                 0           //1
#define  _EE_ML_THRESHOLD              1           //1
//!#define  _EE_MIN_LTRS                  2           //1 
#define  _EE_FALSE_PULSES_TIMING       2           //1
#define  _EE_FAST_SOL_TURNOFF_MARGIN   3           //1
#define  _EE_SLOW_SOL_TURNOFF_MARGIN   4           //1
#define  _EE_CALIBRATION_VALUE         5           //2
#define  _EE_MOTOR_TIMEOUT             7           //1
#define  _EE_ML_SETTING                8           //1
#define  _EE_NO_FLOW_TIMEOUT           9           //1
#define  _EE_RATE_TENS                 10          //1           
#define  _EE_RATE_ONES                 11          //1       
#define  _EE_RATE_TENTH                12          //1
#define  _EE_RATE_HUNDREDTH            13          //1
#define  _EE_SHIFT_TOTALIZER           14          //4
#define  _EE_MASTER_TOTALIZER          18          //4
//!#define  _EE_MECH_COUNT                22          //1
#define  _EE_LITERS                    23          //4
#define  _EE_AMOUNT                    27          //4
#define  _EE_SIGN_FLAG                 31          //1
#define  _EE_MECH_COUNT                32          //4
#define  _EE_REMAIN_MECH_VALUE         36          //4
#define  _EE_LTR_10_HISTORY            40          //4  range form 40 to 79
// problem in eeprom locations 80 - 83 (not writing values)
#define  _EE_REMAIN_SHIFT_VALUE        84          //4
#define  _EE_REMAIN_MASTER_VALUE       88          //4
//-----------------------------------------------------------------------------


//---------------------------------Misc Defines--------------------------------
#define  _MAX_LENGTH  3
#define  _LTR_10_ARRAY 11
/* Following two macros are for restricting max and min values for calibration factor */
#define  _CALIB_MIN                 500      // 5.00 ml per pulse
#define  _CALIB_MAX                 1500     // 15.00 ml per pulse

//!#define DEBUG_ENABLED      // comment or uncomment to disable/enable printf

#ifdef  DEBUG_ENABLED
   #define DEBUG_DATA(msg,data) printf(msg,data)
#else
   #ignore_warnings 204
   #define DEBUG_DATA(msg,data) if(0)
#endif



//------------------------------Global Variables-------------------------------
int1  check_disp_pattern = FALSE, initialize_parameters = FALSE, any_key_pressed = FALSE,
      key_press_check = FALSE;
      
int   remaining_time=0;

int   state=0, leader=0, startbit=0, address=0, mode=0, toggle=0, prv_toggle=0, command=0,
      ir_key_press=0, ir_key_press2=0, sub_menu=0, ok_key=0,
      prv_command=0, enter=0;

volatile int1  new_filling = FALSE, pulses_ignore_flag = FALSE,
               totalizer_calculations = FALSE;
volatile int   msec_100 = 0, menu = 0, inc_var=0, numeric=0, milli_sec=0, time_start=0,
               kp_n=0, pulses_to_ignore = 0, no_flow_count = 0, motor_count = 0, 
               testing_variable = 0, mech_counter_time = 0;
volatile int   time_to_inactive_psense=0;

volatile int16 keypad_timeout=0;

int   pulse_threshold=0, ml_threshold=0, min_ltrs=0, fast_sol_turnoff_margin=0,
      slow_sol_turnoff_margin=0, motor_timeout=0, no_flow_timeout=0, 
      false_pulses_timing=0, false_p_time=0;

signed int ml_setting=0;

int1  slow_solenoid_only = FALSE, filling_start = FALSE, filling_performed = FALSE,
      two_sec_passed = FALSE, p_occured = FALSE,
      load_values_in_ram = FALSE, load_ram_flag = FALSE;
int16 min_ltrs_100x=0, calibration_value=0,power_loop_counter=0, ignored_pulses = 0;

int   blink=0, hundred=0, tens=0, ones=0;

int   ir_rate[4]={0,0,0,0};
int   rate_hundredth=0,rate_tenth=0,rate_ones=0,rate_tens=0;

int1  ltr_selection=FALSE, preset_key=TRUE, full_filling=0, 
      kp_pressed=FALSE, kp_value_flag=FALSE, sign_flag=0;

int   col=0,col_loop=0,clock_status=0,key=0,tenth=0,
      hundredth=0,thousand=0,lakh=0,tenthousand=0;
      
int   kp_data[6]={0,0,0,0,0,0};
int   col_data[4]={0x7F,0xbF,0xdF,0xEF};
int16 nozzle_counter=0;

int32 unit_price=0, liters=0, amount=0, kp_rupees=0, kp_liters=0, 
      temp32=0, required_pulses=0, FS_turn_off_pulses = 0, LS_turn_off_pulses = 0,
      calib_value=0,kp_value=0;

volatile int32 pulses_counter=0, shift_totalizer=0, master_totalizer=0,
               mech_count = 0, temp_mech_count = 0, remain_mech_value = 0,
               temp1_shift =0, temp1_master = 0, temp2_master = 0,
               remain_shift_value = 0, temp2_shift = 0;

int1  irs[58]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
              0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
                        
int lcd_data[16] ={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,
                   0xd0,0xe0,0xf0}; //lcd data
                   
int const  lcd_clear[16]={0x0a,0x1a,0x2a,0x3a,0x4a,0x5a,0x6a,0x7a,0x8a,0x9a,0xaa,0xba,
                          0xca,0xda,0xea,0xfa}; //clear lcd data

int const  lcd_888[18]={0x78,0x68,0x58,0xF8,0xC8,0xD8,0x48,0x38,0x18,0x88
                           ,0x98,0xE8,0x48,0x08,0x28,0xB8,0xA8,0xE8}; 
                           
int const lcd_blank[18]={0xEA,0xAA,0xBA,0x2A,0x0A,0x4A,0xEA,0x9A,0x8A,0x1A
                              ,0x3A,0x4A,0xDA,0xCA,0xFA,0x5A,0x6A,0x7A};                           

int1  show_liter_history_flag = FALSE;
int history_counter = 0, history_no = 0, k=0;
//!int32 ram_weight[]={0,0,0};
//!int32 ram_rupees[]={0,0,0};

int32 ltr_10_history[_LTR_10_ARRAY];

int32 ltr_history[_MAX_LENGTH];
int32 rs_history[_MAX_LENGTH];
int16 rate_history[_MAX_LENGTH];

typedef enum {
             _DISP_LAST_FILLING=1,
             _SUB_MENU_CASES=2,
             _SHIFT_TOTALIZER=3,
             _MASTER_TOTALIZER=4
             } main_menu_t;
main_menu_t main_menu;

typedef enum {
             _ML_TO_IGNORE=1,
             _FALSE_PULSES_IGNORE_TIMING=2,
//!             _LTRS_FOR_FAST_SOLENOID=2,
             _FAST_SOLENOID_OFF_MARGIN=3,
             _SLOW_SOLENOID_OFF_MARGIN=4,
             _CALIBRATION_FACTOR=5,
             _RESTORE_FACTORY_SETTING=6,
             _MOTOR_OFF_TIMING=7,
             _ML_SETTING_MENU=8,
             _TIMEOUT_FOR_NO_FLOW=9,
             _RATE_SETTING=15
             } sub_menu_no_t;
sub_menu_no_t menu_no;
//------------------------------Function Prototypes----------------------------
void io_init(void);   // Function for IO initialization (initial state and direction)
void delay_50(void);
void delay_250(void);
void clear_lcd(void);
void data_save(void);
void ir_routine(void);
void read_keypad(void);
void show_on_lcd(void);
void scan_keypad(void);
void format_data(void);
void power_check(void);
void check_nozzle(void);
void value_from_kp(void);
void peripheral_init(void);   // Function for peripheral initialization
void clear_888_pattern(void);
void show_liter_histroy(void);
void display_888_pattern(void);

void solenoid_and_motor_off(void);
void solenoid_and_motor_off_ISR(void);

void dataout_lcd(int data);
void dataout_kp(int kp_val);

void parameters_value_ml(int &value, int1 &flag, int menu_number);
void parameters_value(int &value, int menu_number, int1 fine_tuning);
void parameters_value(int16 &value, int menu_number, int1 fine_tuning);

int read_8_bit_values(int location);
void write_8_bit_values(int value,int location);


int16 read_16_bit_values(int location);
void write_16_bit_values(int16 value, int location);


int32 read_32_bit_values(int location);
void write_32_bit_values(int32 value, int location);

void totalizers_calculations(void);

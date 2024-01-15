#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);

const int B_SET =  7;     //button set pin
const int B_START =  8;   //button start / reset pin
const int B_SAFTY =  3;   //button interlock
const int UV_OUT =  10;   //output for relay control
int set_t = 120;           //after boot time is set to 34 seconds for my UV box, you can change it here for your need

//x+y dimensions of SET menu
int menu_1x = 20 ;  
int menu_1y = 32 ;

//x+y dimensions of START menu
int menu_2x = 90 ;
int menu_2y = 32 ;

void setup(void) {
  pinMode(B_SET,INPUT_PULLUP);
  pinMode(B_START,INPUT_PULLUP);
  pinMode(B_SAFTY,INPUT_PULLUP);
  pinMode(UV_OUT,OUTPUT);
   digitalWrite(UV_OUT, HIGH);
  pinMode(LED_BUILTIN,OUTPUT);
  
  //used for starting of a nano, also to stabilise power source voltage
  //mainly just for fun
  delay(100);
  u8g2.begin();
  u8g2.setFont(u8g2_font_calibration_gothic_nbp_tf);
   delay(200);
//  u8g2.setCursor(10,20);
 // u8g2.print("Booting.");   //just a joke... it is not needed here but gives a more sophisticated look
//  u8g2.sendBuffer();
  
//  for (int j=1; j<5; j++){
//    u8g2.print(".");
//    u8g2.sendBuffer();
//    delay(200);
//  }
}

void loop(void) {
  //clear screen
  u8g2.clearBuffer();

  //print menu SET
  u8g2.setFont(u8g2_font_t0_11_mf);
  u8g2.setCursor(menu_1x,menu_1y);
  u8g2.print("SET");

  //print menu START
  u8g2.setFont(u8g2_font_t0_11_mf);
  u8g2.setCursor(menu_2x,menu_2y);
  u8g2.print("START");
  
  //detect if button SET is active, make SET font bold
  while(digitalRead(B_SET) == LOW){
  u8g2.setFont(u8g2_font_t0_11b_mf);
  u8g2.setCursor(menu_1x,menu_1y);
  u8g2.print("SET");
  u8g2.sendBuffer();
  //increment timer 
 
    //if button is still pressed...
    while(digitalRead(B_SET) == LOW){
       set_t ++;
      //print incremented time
      printTime();
      delay(100);
      //print RESET menu
      for (int k = 1; k == 1 ;k++){
      u8g2.setFont(u8g2_font_t0_11_mf);
      u8g2.setCursor(menu_2x,menu_2y);
      u8g2.print("RESET");
      u8g2.sendBuffer();
      }
      //and if SET and START are pressed both, make RESET font bold
      if(digitalRead(B_START) == LOW){
      u8g2.setCursor(menu_2x,menu_2y);
      u8g2.setFont(u8g2_font_t0_11b_mf);
      u8g2.print("RESET");
      u8g2.sendBuffer();
        //and reset timer to 0
        set_t = 120;
        delay(500);
      }
      delay(10);
    }
  }
  
  //if button START is active, print START font as bold
  if(digitalRead(B_START) == LOW){
  u8g2.setFont(u8g2_font_t0_11b_mf);
  u8g2.setCursor(menu_2x,menu_2y);
  u8g2.print("START");
  u8g2.sendBuffer();
    while(digitalRead(B_START) == LOW){
     delay(10); 
    }
    //activate output for UV light relay
    digitalWrite(UV_OUT, LOW);
    //activate visual output using LED on arduino
    digitalWrite(LED_BUILTIN, HIGH);
    //if timer value is higher than zero, start countdown :...
    while(set_t != 0){
      //...show two warning thunderbolts...
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
      u8g2.setCursor(0, 35);
      u8g2.print((char)67);
//      u8g2.setCursor(100, 32);
//      u8g2.print((char)67);
      //...and timer countdown
      printTime();
      //each one milisecond test if safety interlock is activated
      //this takes roughly one second, so timer stay almost accurate
      for (int k=1; k<1000; k++){
        //if interlock is active, activate warning subroutine safety_lock
        while(digitalRead(B_SAFTY) == LOW){
          safety_lock();
        }
        //this activates UV and LED again after interlock was activated
        digitalWrite(UV_OUT, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1);  
      }
      //decrement the timer one second down
      set_t --;
    }
    //if here, set_t is zero, countdown ended
    //deactivate the outputs
    digitalWrite(UV_OUT, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    //clear the screen and write confirmation message
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_calibration_gothic_nbp_tf);
    u8g2.setCursor(30,20);
    u8g2.print("Done :)");
    u8g2.sendBuffer();
    //message stays on for 1.5second
    delay(1500);
  }

  //subroutine that shows set time value on main screen
  printTime();
  u8g2.sendBuffer();
  delay(10);  
}

//subroutine for printing a set / remaining time
void printTime(){
  u8g2.setFont(u8g2_font_calibration_gothic_nbp_tf);
  u8g2.setCursor(35,18);
  u8g2.print("Time: ");
  u8g2.print(set_t);
  u8g2.print("sec.");
  u8g2.sendBuffer();
}

//safety lock subroutine
//deactivates UV output, blinks arduino LED and shows warning message
void safety_lock(){
  digitalWrite(UV_OUT, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_calibration_gothic_nbp_tf);
  u8g2.setCursor(10,20);
  u8g2.print("SAFETY LOCK");
  u8g2.sendBuffer();
}

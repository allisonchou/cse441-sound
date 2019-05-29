/*
 * Aspectum -- Installation Arduino Routine
 * Written by Andrew Calimlim, CSE 441
 * With help from my teammates, especially Peter, and the internet
 */
 
#include <Adafruit_NeoPixel.h> //Adafruit NeoPixel LED library - for programming individual LED lights

#define LED_PIN    6
#define LED_COUNT 23

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // NeoPixel strip object

#include <LiquidCrystal_I2C.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int idle_LCD_reducer = 16;
int idle_LCD_cycler = 0;
int input_LCD_cycler = 0;



/*Global variables (because continuous loop programming is ridiculous)*/

int idle_lights_reducer = 22;
int idle_lights_cycler = 0;
int previous_cpi = 0;

int input_first_stable_input_time = -1;

void setup() {
  Serial.begin(9600);
  
  strip.begin();           // initialize NeoPixel strip object
  strip.show();            
  strip.setBrightness(255); // brightness (max = 255)

  lcd.init();                      // initialize the lcd 
  lcd.backlight();
}

void loop() {
  
  inputMode();
  return;
  
}

void inputLCD(){
  String nl[5] = {"Quiet", "Study", "Chat", "Talk", "Party"};
  int cur_pm_input = analogRead(A3);
  

}

void idleLCD(){
  String display_text_1 = "ASPECTUM";
  String display_text_2 = "Select desired noise level";
  int framerate = 750;
  
  lcd.setCursor(0,0);
  lcd.print(display_text_1); // first line never changes, ever in this mode

  if( floor(millis() / framerate) > idle_LCD_cycler){
    lcd.clear();
    idle_LCD_cycler = floor(millis() / framerate);
    idle_LCD_reducer--;
  }
  else{
    if (idle_LCD_reducer >= 0){
      lcd.setCursor(idle_LCD_reducer,1);
      lcd.print(display_text_2);
    }
    else if(idle_LCD_reducer >= -26){
      lcd.setCursor(0,1);
      lcd.print(display_text_2.substring(idle_LCD_reducer * -1));
    }
    else{
      idle_LCD_reducer = 14;
    }  
  }
}

void inputMode(){ 
  
  /*Detecting when to switch over*/
  int cur_pm_input = analogRead(A3); // ANALOG PIN 3

  int input_dif = abs(cur_pm_input - previous_cpi);
  int time_passed = millis() - input_first_stable_input_time;
  int cur_time = millis();
  delay(10);
  previous_cpi = cur_pm_input;
  
  if (input_dif <= 10){ //stability between loops detected
    if (input_first_stable_input_time < 0){ // stability is new
      //Serial.println("---STABILITY DETECTED!---");
      input_first_stable_input_time = millis(); //first time input was stable is this moment
    }
    else if (time_passed >= 10000){ //stability isn't new but has been stable for at least 10 seconds
      //Serial.println("IDLE MODE NOW");
      //return;
      idleMode();
      return;
    }
  }
  else{
    if (input_first_stable_input_time != -1){
      //Serial.println("---STABILITY LOST!---");
      input_first_stable_input_time = -1; //new stability
    }

  }
  
  /*LCD Input mode*/
  String nl_titles[3] = {"Quiet", "Chat", "Party"};
  int index = floor(cur_pm_input/341);
  String display_text_1 = nl_titles[index];

  String display_text_2 = "Placeholder text";

  /*Placeholder code*/
  
  String nl_data[3][4] = {
    {""},
    {""},
    {""},
  };

  // done for each incoming sensor data pin
  
  //int gates124_nl = analogRead(A0);
  int gates124_nl = 5;
  
  int index_2;
  if(gates124_nl > 3){
    index_2 = 2;
  }
  else{
    index_2 = 0;
  }
  nl_data[index_2][0] = "Gates 124";
  
  if (nl_data[index][0] == ""){
    display_text_2 = "No rooms a.t.m.";
  }
  else{
    display_text_2 = "";
    
    for (int i = 0; i < 4; i++){
      if(nl_data[index][i] != ""){
        display_text_2 += nl_data[index][i];
        display_text_2 += ", ";
      }

    }
  }
  
  int framerate_lcd = 1000;

  if( floor(millis() / framerate_lcd) > input_LCD_cycler){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(display_text_1);
      lcd.setCursor(0,1);
      lcd.print(display_text_2);
      input_LCD_cycler = floor(millis() / framerate_lcd);
   }

  /*LED Input mode*/
  strip.clear(); //clears all the lights (lights that were turned on and aren't turned on again then stay off)
  
  int first_pixel_not_lit = floor(cur_pm_input/44);
  for(int i = 0; i < first_pixel_not_lit; i++){
    int i_red_val = (i * 11); //0, 11, 22 ... 242 
    int i_blue_val = 255 - (i * 11); //255, 244, 233 ... 0
    strip.setPixelColor(i, i_red_val, 0, i_blue_val); // gradually goes from blue to red
  }
  strip.show();
}

void idleMode(){
    
    int cur_pm_input = analogRead(A3); // ANALOG PIN 3

    /*Preparing to switch to input mode*/
    int input_dif = abs(cur_pm_input - previous_cpi);
    delay(10);
    previous_cpi = cur_pm_input;
    
    if (input_dif > 20){
      //Serial.println("INPUT MODE");
      //inputMode();
      return;
    }
    
    /*LCD idle mode routine*/
    int framerate = 100;
    String display_text_1 = "ASPECTUM";
    String display_text_2 = "Select desired noise level";
    int framerate_lcd = 750;
    
    lcd.setCursor(0,0);
    lcd.print(display_text_1); // first line never changes, ever in this mode
  
    if( floor(millis() / framerate_lcd) > idle_LCD_cycler){
      lcd.clear();
      idle_LCD_cycler = floor(millis() / framerate_lcd);
      idle_LCD_reducer--;
    }
    else{
      if (idle_LCD_reducer >= 0){
        lcd.setCursor(idle_LCD_reducer,1);
        lcd.print(display_text_2);
      }
      else if(idle_LCD_reducer >= -26){
        lcd.setCursor(0,1);
        lcd.print(display_text_2.substring(idle_LCD_reducer * -1));
      }
      else{
        idle_LCD_reducer = 14;
      }  
    }

    /*Strip idle mode routine*/
    if(floor(millis() / framerate) > idle_lights_cycler){ //time for a new animation frame!
      strip.clear();
      idle_lights_cycler = floor(millis() / framerate);
      idle_lights_reducer--;
    }
    
    else{
      if (idle_lights_reducer >= 0){
        strip.setPixelColor(idle_lights_reducer, 255, 255, 255);
        strip.setPixelColor(idle_lights_reducer + 1, 255, 255, 255);
        strip.setPixelColor(idle_lights_reducer + 2, 255, 255, 255);
        strip.setPixelColor(idle_lights_reducer + 3, 255, 255, 255);
        strip.show();
      }

      /* Conditionals for smoothing the idle animation loop*/
      
      else if(idle_lights_reducer == -1){
        strip.setPixelColor(idle_lights_reducer + 1, 255, 255, 255);
        strip.setPixelColor(idle_lights_reducer + 2, 255, 255, 255);
        strip.setPixelColor(idle_lights_reducer + 3, 255, 255, 255);
        strip.show();
      }
      else if(idle_lights_reducer == -2){
        strip.setPixelColor(idle_lights_reducer + 2, 255, 255, 255);
        strip.setPixelColor(idle_lights_reducer + 3, 255, 255, 255);
        strip.show();
      }
      else if(idle_lights_reducer == -3){
        strip.setPixelColor(idle_lights_reducer + 3, 255, 255, 255);
        strip.show();
      }

      else {
        idle_lights_reducer = 22;
      }
    }
}

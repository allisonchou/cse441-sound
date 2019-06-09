/*
 * Aspectum -- Installation Arduino Routine
 * Written by Andrew Calimlim, CSE 441
 * With help from my teammates, especially Peter, and the internet
 */

#include <SoftwareSerial.h> 
SoftwareSerial ESPserial(2, 3); // RX | TX

#include <Adafruit_NeoPixel.h> //Adafruit NeoPixel LED library - for programming individual LED lights

#define LED_PIN    6
#define LED_COUNT 23

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // NeoPixel strip object

#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <SoftwareSerial.h>


LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

/*Global variables (because continuous loop programming is ridiculous)*/

int demo_nl;
int gates124_nl;

int idle_LCD_reducer = 16;
int idle_LCD_cycler = 0;

int idle_lights_reducer = 22;
int idle_lights_cycler = 0;

int input_LCD_cycler = 0;

int previous_cpi = 0;

int input_first_stable_input_time = -1;

void setup() {
  Serial.begin(115200);

  //Start the software serial for communication with the ESP8266 component
  ESPserial.begin(57600);  
  
  strip.begin();          
  strip.show();            
  strip.setBrightness(255); // brightness (max = 255)

  lcd.init();  
  lcd.backlight();

  //pinMode(A5, OUTPUT); for intializing any LED lights on the building drawings
  //pinMode(A4, OUTPUT);
}

void loop() {  
  inputMode();
  return;
  
}

void idleLCD(){
  String display_text_1 = "ASPECTUM";
  String display_text_2 = "Select desired noise level";
  int framerate = 750;
  
  lcd.setCursor(0,0);
  lcd.print(display_text_1); // first line of text never changes in input mode

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

  //digitalWrite(A5, HIGH); // any LED light

  //digitalWrite(A4, HIGH); // any LED light

  
  
  /*Detecting any inactivity*/
  int cur_pm_input = analogRead(A3); // ANALOG PIN 3

  int input_dif = abs(cur_pm_input - previous_cpi);
  int time_passed = millis() - input_first_stable_input_time;
  int cur_time = millis();
  delay(10);
  previous_cpi = cur_pm_input;
  
  if (input_dif <= 10){ //stability between loops detected
    if (input_first_stable_input_time < 0){ // stability is new
      input_first_stable_input_time = millis(); //first time input was stable is this moment
    }
    else if (time_passed >= 10000){ //stability isn't new but has been stable for at least 10 seconds

      idleMode();
      return;
    }
  }
  else{
    if (input_first_stable_input_time != -1){
      input_first_stable_input_time = -1; //new stability
    }

  }
  
  /*LCD Input mode*/
  
  String nl_titles[5] = {"Silence", "Murmur", "Chat", "Lively", "Bustle"};
    
  String display_text_1 = "";
  String display_text_2 = "";

  // same conditional as the lights so theoretically it should work the same
  int title = floor(cur_pm_input/42);

  if (0 <= title && title <= 4){
    display_text_1 = nl_titles[0];
  }
  else if (5 <= title && title <= 9){
    display_text_1 = nl_titles[1];
  }
  else if (10 <= title && title <= 14){
    display_text_1 = nl_titles[2];
  }
  else if (14 <= title && title <= 18){
    display_text_1 = nl_titles[3];
  }
  else if (19 <= title && title <= 23){
    display_text_1 = nl_titles[4];
  }

  String nl_data[5][4] = {
    {""},
    {""},
    {""},
    {""},
    {""},
  };

  int index = floor(cur_pm_input/205);

 
  // done for each incoming sensor data pin

  
  /*Updating noise level data array from Gates 124 sensor*/

  
  if (ESPserial.available()){
    Serial.println("ESP available!!!");
    // If sensor reads 1 through 5, the data is stored as 0 through 4
    gates124_nl = ESPserial.read() - 1;
  }
  
  // iterates through entire row of gates' current noise level
  
  for(int i = 0; i < 4; i++){
    // checks for the first empty space and places it there
    if ((nl_data[gates124_nl][i] == '\0')){
      nl_data[gates124_nl][i] = "Gates 124";
      break;
    }
  }


  /*Updating noise level data array from demo sensor*/


  
  if (ESPserial.available()){
    Serial.println("ESP available!!!");
    demo_nl = ESPserial.read() - 1;
  }
  // iterates through entire row of demo's current noise level
  for(int i = 0; i < 4; i++){
    if (nl_data[demo_nl][i] == '\0'){
      nl_data[demo_nl][i] = "Demo";
      break;
    }
  }

  /*Printing noise level data to LCD display*/
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

  display_text_2.remove(display_text_2.length() - 2 , 2);
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
  
  int first_pixel_not_lit = floor(cur_pm_input/42);
  for(int i = 0; i < first_pixel_not_lit; i++){
    if (0 <= i && i <= 4){
      strip.setPixelColor(i, 0, 255, 255); //blue
    }
    else if (5 <= i && i <= 9){
      strip.setPixelColor(i, 0, 255, 0); //green
    }
    else if (10 <= i && i <= 14){
      strip.setPixelColor(i, 255, 255, 0); //yellow
    }
    else if (14 <= i && i <= 18){
      strip.setPixelColor(i, 255, 128, 0); //orange
    }
    else if (19 <= i && i <= 23){
      strip.setPixelColor(i, 255, 0, 0); //red
    }
  
    /* 
     *  old color scheme blue to red
    int i_red_val = (i * 11); //0, 11, 22 ... 242 
    int i_blue_val = 255 - (i * 11); //255, 244, 233 ... 0
    strip.setPixelColor(i, i_red_val, 0, i_blue_val); // gradually goes from blue to red
    */
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

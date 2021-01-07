#define BAUDRATE 19200

#define ONSCREEN_STRING_LEN 17
#define DISPLAY_BUFFER_SIZE 380

#define SCREEN_ROTATION 0

/* Sensor Panel Stuffs */
char displayData[DISPLAY_BUFFER_SIZE];

#define CPU_NAME "XEON 2680 v2"
#define GPU_NAME "Radeon R9 Fury"
char hostname[ONSCREEN_STRING_LEN + 1];
char os_version[ONSCREEN_STRING_LEN + 1];
int num_cores = 0;
int cpu_usage = 0;
int package_temp = 0;
int edge_temp = 0;
int ram_amount = 0;
int ram_usage = 0;

/* Data Input Stuffs */
uint8_t cycle = 0;

char* cmd_array[100] = {(char*)"Test",
                        (char*)"Package_Temp", 
                        (char*)"Edge_Temp", 
                        (char*)"Cpu_Usage", 
                        (char*)"Num_Cores", 
                        (char*)"Ram_Usage",
                        (char*)"Ram_Amount",
                        (char*)"Hostname",
                        (char*)"OS_Version"};

int getIndex( const char * key ) {
  /**
   * Returns the index of the char* key in cmd_array.
   * Used for calling the corresponding command in the MakeblockSmartServo library using a switch statement.
   */
	for ( uint8_t i = 0; i < sizeof(cmd_array) / sizeof(char *); i++ ) {
    if (!strcmp(key, cmd_array[i])) return i;
  }	
	return -1;
}

// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <string.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

void setup(void) {
  Serial.begin(BAUDRATE);
  Serial.println(F("TFT LCD test"));

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
#endif

  tft.reset();

  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);

  tft.fillScreen(BLACK);
  tft.setRotation(SCREEN_ROTATION);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);

  updateDisplay();
}

void updateDisplay() {
  /* Format String of Display Data */
  char prevDisplayData[DISPLAY_BUFFER_SIZE];
  
  // Store Display State and Reset Displaydata
  strncpy(prevDisplayData, displayData, sizeof(prevDisplayData));
  memset(&displayData[0],0,sizeof(displayData));

  // Insert Strings
  sprintf(&displayData[0] + strlen(displayData), " %15s \n", hostname);
  sprintf(&displayData[0] + strlen(displayData), " %15s \n \n", os_version);
  sprintf(&displayData[0] + strlen(displayData), " -CPU-------------  \n");
  sprintf(&displayData[0] + strlen(displayData), "|                 |\n");
  sprintf(&displayData[0] + strlen(displayData), "|%3d Core CPU     |\n", num_cores);
  sprintf(&displayData[0] + strlen(displayData), "|%3d %% Usage      |\n", cpu_usage);
  sprintf(&displayData[0] + strlen(displayData), "|                 |\n");
  sprintf(&displayData[0] + strlen(displayData), "|Package |  Edge  |\n");
  sprintf(&displayData[0] + strlen(displayData), "| %3d    |  %3d   |\n", package_temp, edge_temp);
  sprintf(&displayData[0] + strlen(displayData), " ----------------- \n");
  sprintf(&displayData[0] + strlen(displayData), " -Memory---------- \n");
  sprintf(&displayData[0] + strlen(displayData), "|%3d Gb           |\n", ram_amount);
  sprintf(&displayData[0] + strlen(displayData), "|%3d %% Usage      |\n", ram_usage);
  sprintf(&displayData[0] + strlen(displayData), " ----------------- \n");
  sprintf(&displayData[0] + strlen(displayData), " -GPU------------- \n");
  sprintf(&displayData[0] + strlen(displayData), "|                 |\n");
  sprintf(&displayData[0] + strlen(displayData), "|                 |\n");
  sprintf(&displayData[0] + strlen(displayData), " ----------------- \n");

  /* Print Data to Display */
  uint16_t i = 0;
  int x = 0;
  int y = 0;
  tft.setCursor(0,0);

  for (i=0; i<strlen(displayData); i++) {
    if (displayData[i] == prevDisplayData[i]) {
      if (displayData[i] == '\n') {
        tft.print("\n");
      }
      else{
        tft.print(" ");
      }
    }
    else {
      x = tft.getCursorX();
      y = tft.getCursorY();
      tft.setTextColor(BLACK);
      tft.print(prevDisplayData[i]);
      tft.setTextColor(WHITE);
      tft.setCursor(x, y);
      tft.print(displayData[i]);
    }
  }
}

void loop() {

  #define BUFFER_LEN 20
  
  uint8_t i = 0;
  uint8_t s_idx = 0;

  char buf[BUFFER_LEN + 1];
  char cmd[BUFFER_LEN + 1];
  char arg[BUFFER_LEN + 1];
  String in_string;
  String split_string;
  
  if (Serial.available() > 0) {
    // Read available String from serial
    in_string = Serial.readString();
    
    if (sizeof(in_string) > 0) {
      
      // Split String by lines
      for (i=0; i<in_string.length(); i++) { 

        if (in_string[i] == '\n' || in_string[i] == '\n\r') {

          split_string = in_string.substring(s_idx, i);

          // Convert String to char* for sscanf
          split_string.toCharArray(buf, sizeof(buf));
      
          // Extract cmd string and parameters from the input
          sscanf(buf, "%s %s", cmd, arg);

          switch (getIndex(cmd)){
            default: break;

            case 0: //Test
              Serial.println("Test");
              break;
            
            case 1: // Package Temp
              sscanf(arg, "%d", &package_temp);
              break;

            case 2: // Edge Temp
              sscanf(arg, "%d", &edge_temp);
              break;

            case 3: // Cpu Usage
              sscanf(arg, "%d", &cpu_usage);
              break;

            case 4: // Num Cores
              sscanf(arg, "%d", &num_cores);
              break;

            case 5: // Ram Usage
              sscanf(arg, "%d", &ram_usage);
              break;

            case 6: // Ram Amount
              sscanf(arg, "%d", &ram_amount);
              break;

            case 7: // Hostname
              strncpy(hostname, arg, sizeof(hostname));
              break;

            case 8: // OS Version
              strncpy(os_version, arg, sizeof(os_version));
              break;

          };
        }
      }
    }
  }

  // Do not update Display every Cycle
  if (cycle >= 5) {
    updateDisplay();
    cycle = 0;
  }
  else {
    cycle += 1;
  }
  

  // Delay to circumvent busy loop
  //delay(10);

}

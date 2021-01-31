#define BAUDRATE 19200

#define ONSCREEN_STRING_LEN 20
#define DISPLAY_BUFFER_SIZE 250

#define SCREEN_ROTATION 2


/* Macros for various display elements */

/* Set Screen Resolution and number of rows and characters that fit onscreen based on screen rotation*/
#if (SCREEN_ROTATION == 0 || SCREEN_ROTATION == 2)
  /* Portrait mode */
  #define NUM_ROWS 20
  #define NUM_CHARS 15
  #define SCREEN_HEIGHT 320
  #define SCREEN_WIDTH 240
#elif (SCREEN_ROTATION == 1 || SCREEN_ROTATION == 3)
/* Horizontal mode */
  #define NUM_ROWS 14
  #define NUM_CHARS 20
  #define SCREEN_HEIGHT 240
  #define SCREEN_WIDTH 320
#endif

#define ROW_HEIGHT SCREEN_HEIGHT/NUM_ROWS
#define PADDING 5

/* Print string once or multiple times */
#define PRINT_STRING(string) sprintf(&displayData[0] + strlen(displayData), "%s", string);
#define MULTISTRING(string, num) for(int i=0;i<num;i++) PRINT_STRING(string)

/* Newline, spaces and separator once and multiple times */
#define NEWLINE PRINT_STRING("\n")
#define NEWLINES(num) MULTISTRING("\n",num)
#define SPACE PRINT_STRING(" ")
#define SPACES(num) MULTISTRING(" ",num)
#define SEPARATOR PRINT_STRING("|")

inline int count_occurence(const char *s, char c) {
  /* Returns the number of occurences of char c in string s */
  int i = 0;
  for (i=0; s[i]; s[i]==c ? i++ : *s++);
  return i;
}

/* Draw Box at height of current line of defined depht */
#define BOX(size, color) tft.drawRect(PADDING, count_occurence(displayData,'\n')*ROW_HEIGHT+PADDING, \
                                      SCREEN_WIDTH-2*PADDING, size*ROW_HEIGHT, color); //x, y, w, h color

/* Percent Bar */
#define BAR(percent, width) PRINT_STRING("[") \
                            MULTISTRING("#",round(percent*width/100)) \
                            MULTISTRING("-",width-round(percent*width/100)) \
                            PRINT_STRING("]")

/* Print statistics */
#define NUM_CORES sprintf(&displayData[0] + strlen(displayData), "%3d Core CPU", num_cores);
#define CPU_USAGE sprintf(&displayData[0] + strlen(displayData), "%3d%% Used", cpu_usage);
#define EDGE_TEMP sprintf(&displayData[0] + strlen(displayData), "%3dC Edge", edge_temp);
#define PACKAGE_TEMP sprintf(&displayData[0] + strlen(displayData), "%3dC Temp", package_temp);

#define HOSTNAME sprintf(&displayData[0] + strlen(displayData), "%15s", hostname);
#define OS_VERSION sprintf(&displayData[0] + strlen(displayData), "%15s", os_version);

#define RAM_AMOUNT sprintf(&displayData[0] + strlen(displayData), "%3dGb", ram_amount);
#define RAM_USAGE sprintf(&displayData[0] + strlen(displayData), "%3d%% Used", ram_usage);


/*
 * TODOs:
 * - Add fan control
 * - right-aligned print_string
 * - configurable UI? (SEMIDONE)
 * - fix weird text not fully displaying problem on os and hostname (TODO)
 * - add GPU (figure out how to poll in python)
 * 
 */

/* Sensor Panel Stuffs */
char displayData[DISPLAY_BUFFER_SIZE];
char hostname[ONSCREEN_STRING_LEN + 1];
char os_version[ONSCREEN_STRING_LEN + 1];
int num_cores = 0;
int cpu_usage = 0;
int package_temp = 0;
int edge_temp = 0;
int ram_amount = 0;
int ram_usage = 0;

// GPU currently not working through glances
int gpu_temp = 0;
int gpu_usage = 0;
int gpu_mem_usage = 0;
int gpu_mem_amount = 0;

/* Data Input Stuffs */
uint8_t cycle = 0;

char* cmd_array[150] = {(char*)"Test",
                        (char*)"Package_Temp", 
                        (char*)"Edge_Temp", 
                        (char*)"Cpu_Usage", 
                        (char*)"Num_Cores", 
                        (char*)"Ram_Usage",
                        (char*)"Ram_Amount",
                        (char*)"Hostname",
                        (char*)"OS_Version",
                        (char*)"GPU_Temp",
                        (char*)"GPU_Usage",
                        (char*)"GPU_Mem_Usage",
                        (char*)"GPU_Mem_Amount"
                        };

int getIndex( const char * key ) {
  /**
   * Returns the index of the char* key in cmd_array.
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
#define BLACK   0x0000
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

void updateDisplay() {
  /* Format String of Display Data */
  char prevDisplayData[DISPLAY_BUFFER_SIZE];
  
  // Store Display State and Reset Displaydata
  strncpy(prevDisplayData, displayData, sizeof(prevDisplayData));
  memset(&displayData[0],0,sizeof(displayData));

  /* NEW DISPLAY CONFIGURATION - Portrait Mode */
  BOX(12,WHITE)
  SPACE PRINT_STRING("   XEON E5-2690 V2") NEWLINE
  SPACES(7) NUM_CORES NEWLINES(2)
  SPACES(10) CPU_USAGE NEWLINE
  SPACE BAR(cpu_usage,16) NEWLINES(2)
  SPACES(10) PACKAGE_TEMP NEWLINE
  SPACE BAR(package_temp,16) NEWLINES(2)
  SPACES(10) EDGE_TEMP NEWLINE
  SPACE BAR(edge_temp,16) NEWLINES(3)
  BOX(5,WHITE)
  SPACE PRINT_STRING("       32Gb of RAM") NEWLINES(2)
  SPACES(10) RAM_USAGE NEWLINE
  SPACE BAR(ram_usage,16)

  /* NEW DISPLAY CONFIGURATION  - Horizontal Mode 
  BOX(8,WHITE)
  SPACE PRINT_STRING("          XEON E5-2690 V2") NEWLINE
  SPACES(14) NUM_CORES NEWLINES(2)
  SPACE BAR(cpu_usage,14) CPU_USAGE NEWLINES(2)
  SPACE BAR(package_temp,14) PACKAGE_TEMP NEWLINES(2)
  SPACE BAR(edge_temp, 14) EDGE_TEMP NEWLINES(2)
  BOX(4,WHITE) NEWLINE
  SPACE PRINT_STRING("                 32Gb RAM") NEWLINES(2)
  SPACE BAR(ram_usage, 14) RAM_USAGE */
  
  /* Print Data to Display */
  uint16_t i = 0;
  int x = 0;
  int y = 0;
  tft.setCursor(0,PADDING*2);

  /* Only Update Characters that have changed in order to avoid flickering */
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

        if (in_string[i] == '\n') {

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
            
            case 9: // GPU Temperature
              sscanf(arg, "%d", &gpu_temp);
              break;

            case 10: // GPU Usage
              sscanf(arg, "%d", &gpu_usage);
              break;

            case 11: // GPU Memory Usage
              sscanf(arg, "%d", &gpu_mem_usage);
              break;

            case 12: //GPU Memory Amount
              sscanf(arg, "%d", &gpu_mem_amount);
              break;

          };
        }
      }
    }
  }

  // Do not update Display every Cycle
  if (cycle >= 10) {
    updateDisplay();
    cycle = 0;
  }
  else {
    cycle += 1;
  }

}

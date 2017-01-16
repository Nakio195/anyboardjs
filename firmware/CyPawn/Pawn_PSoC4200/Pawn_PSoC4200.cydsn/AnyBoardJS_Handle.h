#ifndef ANYBOARDJS_HANDLE_H_PROTECTION
#define ANYBOARDJS_HANDLE_H_PROTECTION
   
#include <project.h>

    
void ProcessingRequest();

extern uint8_t BLE_Buffer[20];
extern uint8_t BLE_Buffer_Updated;

enum AnyBoard_Protocol{ANY_COMMAND = 0};


//Index of parameters of commands
enum AnyBoard_LED_ON{PARAM_LED_ON__R = 1, PARAM_LED_ON__G, PARAM_LED_ON__B};
enum AnyBoard_GET_COLOR{PARAM_GET_COLOR__R_H = 1, PARAM_GET_COLOR__R_L, PARAM_GET_COLOR__G_H, PARAM_GET_COLOR__G_L, PARAM_GET_COLOR__B_H, PARAM_GET_COLOR__B_L};

#define GET_NAME             32
#define GET_VERSION          33
#define GET_UUID             34
#define GET_TEMPERATURE      36
#define GET_COLOR            37
#define HAS_LED              64
#define HAS_LED_COLOR        65
#define HAS_VIBRATION        66
#define HAS_COLOR_DETECTION  67
#define HAS_LED_SCREEN       68
#define HAS_RFID             71
#define HAS_NFC              72
#define HAS_ACCELEROMETER    73
#define HAS_TEMPERATURE      74
#define HAS_PRINT            75
#define LED_OFF              128
#define LED_ON               129
#define LED_BLINK            130

#endif

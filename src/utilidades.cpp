#include "utilidades.h"
#include <Arduino.h>

#define BUZZER 18 // pin del buzzer
#define LEDC_CHANNEL 0 //canal ledc pwm

void pitido(char x) {

    ledcSetup(LEDC_CHANNEL, 1000, 8); // configurar canal ledc con frec y num de bits 
    ledcAttachPin(BUZZER, LEDC_CHANNEL); // añadir el pin del buzz al canal 

    if(x == '.'){
      ledcWriteTone(LEDC_CHANNEL, 1000);
      delay(100);
    } else{
      ledcWriteTone(LEDC_CHANNEL, 1000); //pin y frec del sonido
      delay(200);
    }
    ledcWriteTone(LEDC_CHANNEL, 0);//parar el pitido 
}

struct MorseLetra {
    const char* codigo;
    char letra;
};
  
MorseLetra diccionario[] = {
    {".-", 'A'},   {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'},  {".", 'E'},
    {"..-.", 'F'}, {"--.", 'G'},  {"....", 'H'}, {"..", 'I'},   {".---", 'J'},
    {"-.-", 'K'},  {".-..", 'L'}, {"--", 'M'},   {"-.", 'N'},   {"---", 'O'},
    {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'},  {"...", 'S'},  {"-", 'T'},
    {"..-", 'U'},  {"...-", 'V'}, {".--", 'W'},  {"-..-", 'X'}, {"-.--", 'Y'},
    {"--..", 'Z'}
};
  
const int numLetras = sizeof(diccionario) / sizeof(diccionario[0]);

  
char buscarLetra(String codigo) {
    for (int i = 0; i < numLetras; i++) {
      if (codigo.equals(diccionario[i].codigo)) {
        return diccionario[i].letra;
      }
    }
    return '?';//si mandas una secuencua que no corresponde a ninguna letra
}

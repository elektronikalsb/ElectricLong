                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                #include <SoftwareSerial.h> // para cambiar los pines RX y TX
#include <Servo.h> // para el motor
#include <Adafruit_NeoPixel.h> // para las luces
#include "FastLED.h" // para las luces
#include <avr/power.h> // para las luces

#define pedal 7 //numero pin del pedal
#define NUM_LEDS 21 // numero total de los leds
#define PIN_LED 3 // pin al que van conectadas las led
#define LED_COLOR CRGB::DarkOrchid // para cambiar de color

#define NUMPIXELS_1    1
#define NUMPIXELS_2    7
#define NUMPIXELS_3    14// como dividimos los led 
#define NUMPIXELS_4    21

SoftwareSerial h3(11,10); //RX,TX

Servo esc; // defines el esc del variador

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);
CRGB rgb_led[NUM_LEDS]; 

int value_slide=0; //deslizador app
int value_slide_comparacion=0;

int val = 1400; // variable motor le decimos que empieze en el minimo

int boton=0; //variable pedal y le indicamos la posicion en la que empieza

int periodo=1000; // para poder hacer millis
unsigned long tiempo; // para poder hacer millis


int periodo1=1000; // para poder hacer millis
unsigned long tiempo1; // para poder hacer millis

float divisor;
float voltios;
float porcentaje;
int lectura_porcentaje;

uint32_t rojo; 
uint32_t verde;
uint32_t azul;
uint32_t amarillo ; // metemos los colores en variables
uint32_t morado ;
uint32_t celeste;
uint32_t blanco;
uint32_t naranja;

void setup()
  {
      
      Serial.begin(38400); // velocidad de comunicacion puerto serie
      h3.begin(38400); //  velocidad de comunicacion bluetooth H3
      pixels.begin(); 

      analogReference(DEFAULT);

      esc.attach(9); // indicamos el pin del esc
      esc.writeMicroseconds(1000); // para mandar comandos al esc
  
      FastLED.addLeds<WS2812B, PIN_LED>(rgb_led, NUM_LEDS);   
  
      Serial.println("Estamos listos jefe:");
      pinMode(pedal, INPUT_PULLUP);

      rojo = pixels.Color(150,0,0);
      verde = pixels.Color(0,150,0);
      azul = pixels.Color(0,0,150);
      amarillo = pixels.Color(120,150,0); // en las variables que hemos creado antes, guardamos la mezcla de los colores
      morado = pixels.Color(150,0,150);
      celeste = pixels.Color(0,150,150);
      blanco = pixels.Color(150,150,150);
      naranja = pixels.Color(40,150,0);

      luces();          // funcion luces
  }

void loop() 
{
    nivel_bateria(); // entra en esta funcion que lo unico que hace es leer la bateria
    boton = digitalRead(pedal); //aqui dices que pedal vale 0.

  if (lectura_porcentaje<=0)       // si la bateria es menor o igual a 0
  {
    frenado();    // funcion de frenado
    Serial.println("bateria baja");   
    luces();     // funcion luces
  }
  else
  {
    if (h3.available()&&(boton==LOW))   // si hay algo en el puerto serie y el boton esta pulsado( aparece en LOW porque tiene imput_pullup y la logica es a la inversa)
      {
        value_slide = h3.parseInt(); // quitamos los ceros que llegan
        h3.read(); // leemos lo que hay en el buffer del H3
             
        Serial.println("value_slide=");
        Serial.println(value_slide);  // escribe en el puerto serie lo que vale el deslizador
    
        val=map(value_slide,0,255,1400,2200);  // hace una regla de 3 entre las escalas del motor y el deslizador
        esc.writeMicroseconds(val); //intruccion que manda los comandos al esc(motor)
        Serial.println("val=");
        Serial.print(val); // escribe el valor del motor
        luces(); // funcion luceS
      }
      
    else if(boton == HIGH)  // si el pedal esta sin pulsar 
    {
       
      frenado();  // funcion de frenado
      Serial.println("FRENA!!!");
      luces();    // funcion de las luces
      
    }
  }
}

void frenado()
{
if (val>=1400) // Si la variable val es mayor o igual a 1400
 {
  
  Serial.print("val_frenado=");
  Serial.println(val);
  if(millis() > (tiempo + periodo))  // Si millis es mayor que tiempo + periodo.
  {
        tiempo = millis();
        val = val-150;     // la velocidad ba bajando
        
        esc.writeMicroseconds(val);

  }
 }

}

void luces()
{
  int num_leds_switchedon = map(val, 1400, 2200, 0, NUM_LEDS); 
     
    for (int i = 0; i < num_leds_switchedon; i++) {
    rgb_led[i] = celeste;
  }  
    for (int i = 7; i < num_leds_switchedon; i++) {
    rgb_led[i] = morado;
  }
    for (int i = 14; i < num_leds_switchedon; i++) {
    rgb_led[i] = naranja;
  }
    for (int i = 21; i < num_leds_switchedon; i++) {
    rgb_led[i] = verde;
  }
  for (int i = num_leds_switchedon; i < NUM_LEDS; i++) {
    rgb_led[i] = CRGB::Black;
  }
    FastLED.show();
}

void nivel_bateria()   // Con una formula que es Y=mx+b hemos sacado los numeros por los que multiplicamos las variables.
{
  if(millis() > tiempo1 + periodo1)
  {
    tiempo1 = millis();
    divisor = leer();
    voltios = (5*divisor)/1023;     
    Serial.print("voltios=");
    Serial.println(voltios);

    lectura_porcentaje = (140.84*voltios)-601.40;
    Serial.print("lectura_porcentaje=");
  
  
  if (lectura_porcentaje >100)  //Si lectura es mayor que 100, en la aplicacion va a aparecer que es 100 no mayor que 100
   {
    lectura_porcentaje = 100;
   }
  if(lectura_porcentaje < 0)   //Si lectura es menor que 0, en la aplicacion va a aparecer que es 0 no menor que 0
  {
    lectura_porcentaje = 0;
  }
  
  Serial.println(lectura_porcentaje);
  h3.println(lectura_porcentaje);
  }
}

float leer (void)
{
int n;
float media;
float media_anterior;
float acu = 0;
  for (n = 0; n < 200; n++)   // va a hacer este for 200 veces
  {
      
      media_anterior = analogRead(A0);   //lee lo que entra por los pines analogicos
      acu = acu + media_anterior;     // Suma la nueva masla anterior
  }
      media = acu/n;    // divide el resultado de todas las sumas por el numero de numeros que a sumado
      Serial.print("media=");
      Serial.println(media);
      return (media);
}



#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h" 
#include "URTouch.h"       
#include "SPI.h" 
#include <AccelStepper.h>


#define step A0
#define dir A1
#define enbPin A2

#define limitPinUp A3
#define limitPinDown A6
#define stopPin A4
#define joystickPin A7

#define greenLED 2
#define redLED A5

#define TFT_SCK  13
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_CS   10     //SS

#define TFT_DC 8                  
#define TFT_RST 9 
 
#define t_SCK  7         //t_CLK    
#define t_CS 6                
#define t_MOSI 5         //t_DIN
#define t_MISO 4         //t_DO    
#define t_IRQ 3           



Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);
URTouch ts(t_SCK, t_CS, t_MOSI, t_MISO, t_IRQ);

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

const double pi = 3.141593; 

int syringe=50;
bool tog =0, syringe_mode=0, start=0;

//units in mm
double dia=29.1346, area = pi/4*dia*dia;   
double delayTime=0; 
float res =1, value[2];
long int count=0,steps=200;


int neutral_pos=0;
bool limitUpState, limitDownState;
unsigned int Time;

// int c/

void setup()
{

  pinMode(dir,    OUTPUT);
  pinMode(step,   OUTPUT); 
  pinMode(enbPin, OUTPUT); 
  
  pinMode(limitPinUp,     INPUT);
  pinMode(limitPinDown,     INPUT);
  pinMode(stopPin,      INPUT);
  pinMode(joystickPin,  INPUT);
  pinMode(greenLED,     OUTPUT);
  pinMode(redLED,       OUTPUT);

  digitalWrite(greenLED,LOW);
  digitalWrite(redLED,LOW);
  digitalWrite(enbPin, HIGH);

  digitalWrite(step, LOW);
  digitalWrite(dir, HIGH);

  neutral_pos = analogRead(joystickPin);

  tft.begin();                     
  tft.setRotation(3);
  ts.InitTouch();                   
  ts.setPrecision(PREC_EXTREME);

  
  tft_setup();

}


void loop()
{
  
  int x=0, y=0;
  while(ts.dataAvailable())
  {
    ts.read();
    x=ts.getX();
    y=ts.getY();

    if(!syringe_mode)
    {
      
        if(( x>=180 && x<=249 ) && ( y>=0 && y<=65))          //Flowrate button
        {
          tog=0;

          tft.fillRect(180, 0, 69, 65, 0x8d56);         //toggle flow rate
          tft.fillRect(250, 0, 70, 65, YELLOW);         //toggle volume

          tft.setTextSize(1); tft.setTextColor(BLACK);

          tft.setCursor(194, 14);   tft.print("TOGGLE");
          tft.setCursor(202, 29);   tft.print("FLOW");
          tft.setCursor(202, 44);   tft.print("RATE");

          tft.setCursor(265, 18);   tft.print("TOGGLE");
          tft.setCursor(265, 38);   tft.print("VOLUME");
          
          tft.fillRect(0, 0, 179, 65, WHITE); 
          tft.setTextSize(2); tft.setTextColor(BLACK);
          
          tft.setCursor(1,5);   tft.print("Flow Rate(ml/h)");
          tft.setCursor(20,35);   tft.print(value[tog],2);
          
        }
        else if(( x>=250 && x<=320 ) && ( y>=0 && y<=65))            //Volume button
        {
          tog =1;

          tft.fillRect(180, 0, 69, 65, YELLOW);                     //toggle flow rate
          tft.fillRect(250, 0, 70, 65, 0x8d56);                     //toggle volume

          tft.setTextSize(1); tft.setTextColor(BLACK);

          tft.setCursor(194, 14);   tft.print("TOGGLE");
          tft.setCursor(202, 29);   tft.print("FLOW");
          tft.setCursor(202, 44);   tft.print("RATE");

          tft.setCursor(265, 18);   tft.print("TOGGLE");
          tft.setCursor(265, 38);   tft.print("VOLUME");


          tft.fillRect(0, 0, 179, 65, WHITE); 
          tft.setTextSize(2); tft.setTextColor(BLACK);
          
          tft.setCursor(2,5);   tft.print("Volume (ml)");
          tft.setCursor(20,35);   tft.print(value[tog],2);
          
        }

        
        
        else if(( x>=10 && x<=40 ) && ( y>=70 && y<=100))        //increase
        {
          value[tog] += res;     

          tft.fillRect(15, 33, 164, 27, WHITE); 
          tft.setTextSize(2);     tft.setTextColor(BLACK);    
          tft.setCursor(20,35);   tft.print(value[tog],2);
        }                       
            
        else if(( x>=10 && x<=40 ) && ( y>=105 && y<=135))      //decrease
        {
          value[tog] -= res;     
          
          tft.fillRect(15, 33, 164, 32, WHITE); 
          tft.setTextSize(2);     tft.setTextColor(BLACK);    
          tft.setCursor(20,35);   tft.print(value[tog],2);
        }                       


        else if(( x>=5 && x<=45 ) && ( y>=170 && y<=198))
        {
          res=100;
          tft.setTextSize(2); tft.setTextColor(BLACK);
          tft.fillRect(5, 170, 40, 28, GREEN);                                        //Resolution - 100
          tft.setCursor(7,177); tft.print("100");

          tft.fillRect(51, 170, 37, 28, WHITE);   tft.setCursor(57,177);  tft.print("10");
          tft.fillRect(94, 170, 37, 28, WHITE);   tft.setCursor(106,177); tft.print("1");
          tft.fillRect(5, 205, 40, 28, WHITE);    tft.setCursor(9,212);   tft.print("0.1");
          tft.fillRect(51, 205, 54, 28, WHITE);   tft.setCursor(55,212);  tft.print("0.01");
        }
        else if(( x>=51 && x<=88 ) && ( y>=170 && y<=198))
        {
          res=10;
          tft.setTextSize(2); tft.setTextColor(BLACK);
          tft.fillRect(51, 170, 37, 28, GREEN);                                     //Resolution - 10
          tft.setCursor(57,177); tft.print("10");
          
          
          tft.fillRect(5, 170, 40, 28, WHITE);  tft.setCursor(7,177);   tft.print("100");
          tft.fillRect(94, 170, 37, 28, WHITE); tft.setCursor(106,177); tft.print("1");        
          tft.fillRect(5, 205, 40, 28, WHITE);  tft.setCursor(9,212);   tft.print("0.1");          
          tft.fillRect(51, 205, 54, 28, WHITE); tft.setCursor(55,212);  tft.print("0.01");
        }
        else if(( x>=94 && x<=131 ) && ( y>=170 && y<=198))
        {
          res=1;
          tft.setTextSize(2); tft.setTextColor(BLACK);
          tft.fillRect(94, 170, 37, 28, GREEN);                                    //Resolution - 1
          tft.setCursor(106,177); tft.print("1");

          tft.fillRect(5, 170, 40, 28, WHITE);  tft.setCursor(7,177);   tft.print("100");
          tft.fillRect(51, 170, 37, 28, WHITE); tft.setCursor(57,177);  tft.print("10");
          tft.fillRect(5, 205, 40, 28, WHITE);  tft.setCursor(9,212);   tft.print("0.1");
          tft.fillRect(51, 205, 54, 28, WHITE); tft.setCursor(55,212);  tft.print("0.01");
        }
        else if(( x>=5 && x<=45 ) && ( y>=205 && y<=233))
        {
          res=0.1;
          tft.setTextSize(2); tft.setTextColor(BLACK);
          tft.fillRect(5, 205, 40, 28, GREEN);                                    //Resolution - 0.1
          tft.setCursor(9,212);   tft.print("0.1");

          tft.fillRect(5, 170, 40, 28, WHITE);  tft.setCursor(7,177);   tft.print("100");
          tft.fillRect(51, 170, 37, 28, WHITE); tft.setCursor(57,177);  tft.print("10");
          tft.fillRect(94, 170, 37, 28, WHITE); tft.setCursor(106,177); tft.print("1");
          tft.fillRect(51, 205, 54, 28, WHITE); tft.setCursor(55,212);  tft.print("0.01");
        }
        else if(( x>=51 && x<=105 ) && ( y>=205 && y<=233))
        {
          res=0.01;
          tft.setTextSize(2); tft.setTextColor(BLACK);
          tft.fillRect(51, 205, 54, 28, GREEN);                                    //Resolution - 0.01
          tft.setCursor(55,212);  tft.print("0.01");

          tft.fillRect(5, 170, 40, 28, WHITE);  tft.setCursor(7,177);   tft.print("100");
          tft.fillRect(51, 170, 37, 28, WHITE); tft.setCursor(57,177);  tft.print("10");
          tft.fillRect(94, 170, 37, 28, WHITE); tft.setCursor(106,177); tft.print("1");
          tft.fillRect(5, 205, 40, 28, WHITE);  tft.setCursor(9,212);   tft.print("0.1");
        }


        
        else if(( x>=190 && x<=290 ) && ( y>=80 && y<=125))
        {
          start=1;
          digitalWrite(enbPin,LOW);
          start_steps();
        }           

        else if(( x>=170 && x<=300 ) && ( y>=189 && y<=237))
        {
          switch_syringe();
        }
    }    


    if(syringe_mode)
    {
      
      if(( x>=20 && x<=100 ) && ( y>=45 && y<=85))          //1
      {
        syringe = 1;
      }
      else if(( x>=20 && x<=100 ) && ( y>=95 && y<=135))    //2
      {
        syringe = 2;
      }
      else if(( x>=20 && x<=100 ) && ( y>=145 && y<=185))   //3
      {
        syringe = 3;
      }
      else if(( x>=20 && x<=100 ) && ( y>=195 && y<=235))   //5
      {
        syringe = 5;
      }

      else if(( x>=120 && x<=200 ) && ( y>=45 && y<=85))    //10
      {
        syringe = 10;
      }
      else if(( x>=120 && x<=200 ) && ( y>=95 && y<=135))   //15
      {
        syringe = 15;
      }
      else if(( x>=120 && x<=200 ) && ( y>=145 && y<=185))  //50
      {
        syringe = 50;
      }
      else if(( x>=120 && x<=200 ) && ( y>=195 && y<=235))  //100
      {
        syringe = 100;
      }


      else if(( x>=220 && x<=300 ) && ( y>=45 && y<=85))    //500
      {
        syringe = 500;
      }
      else if(( x>=220 && x<=300 ) && ( y>=95 && y<=135))   //1000
      {
        syringe = 1000;
      }
      else if(( x>=220 && x<=300 ) && ( y>=145 && y<=185))  //OK
      {
        tft_setup();
      }
       
        
        
    }
        

        


      
  }



//Calculation part================================================================


//Calculation for steps

  //d = 15.5 mm

  //steps = rev * 3200
  
  //rev = dist/(pitch * '4')   //4-start leadscrew

  //dist = vol/area

  //area = pi*d*d/4

  //finalllll steppsssss = ((val[1]/area)/8) * 6400

  steps= ceil(((value[1]*1000/area)/8) * 6400 ); 

//=================================================================================

//Calculation for flow rate

  //Flow rate given - value[1] - 'R' ml/hour
  // i.e. R/60 ml/min

  // (R/60) x syringe_conversion_factor_dist(1 ml= how much dist) - mm/min
  
  // 1 rev/min = 2 * '4' mm/min    =>    1 mm/min = 1/8 rev/min    
  //use of pitch & 4 - is no. of start of leadscrew

  //rpm reqd = (R/60) x syringe factor x (1/8) rev/min    ----- here syringe factor is 5 (i.e. 1 ml = 1.5mm) 
  //i.e rpm = value[0]/60 * syringe_dist_factor(1.5 here) * 1/8 
  
 //////// // delayTime(µs) = 4000*2.32709625212735/rpm;
 /////////i.e delay(us) = 


  delayTime= (4651.7037/value[0])*320; //unit - µs
  Time= round(delayTime);

  // if(joystick_pressed)
    if((digitalRead(limitPinUp)==HIGH) && (analogRead(limitPinDown)>512) && (digitalRead(stopPin)==LOW))
    {digitalWrite(redLED, LOW);}

  if(start==0 && (  analogRead(joystickPin)<(neutral_pos-5) || analogRead(joystickPin)>(neutral_pos+5) ))
  {
    digitalWrite(enbPin, LOW);
    digitalWrite(greenLED, HIGH);

    int jt=0, pos=analogRead(joystickPin);

    if(pos<neutral_pos)
    {
      jt=map(pos, 0, neutral_pos-5, 50, 400);   
      digitalWrite(dir, HIGH);
      
      digitalWrite(step,HIGH);
      delayMicroseconds(jt);
      digitalWrite(step,LOW);
      delayMicroseconds(jt);
    }
    else if(pos>neutral_pos)
    {
      jt=map(pos, neutral_pos+5, 1023, 400, 50);
      digitalWrite(dir,LOW);

      digitalWrite(step,HIGH);
      delayMicroseconds(jt);
      digitalWrite(step,LOW);
      delayMicroseconds(jt);
    }
  }
  else 
  {
      digitalWrite(enbPin, HIGH);
      
      digitalWrite(greenLED, LOW);

  }
  
}

void start_steps()
{ 
  
  if(start)
  {
    digitalWrite(greenLED, HIGH);


    digitalWrite(dir, LOW);
    if(delayTime <= 16000.00)
    {
      for(int i=0; i<steps; i++)
      {
        digitalWrite(step,HIGH);
        delayMicroseconds(delayTime);

        digitalWrite(step,LOW);
        delayMicroseconds(delayTime);
        
        if((digitalRead(limitPinUp)==LOW) || (analogRead(limitPinDown)<512) || (digitalRead(stopPin)==HIGH))
        {
          digitalWrite(redLED, HIGH);
          digitalWrite(greenLED, LOW);    
          break;
        }
        
        
      }
    }
    else
    {
      delayTime /=1000; 
      for(int i=0; i<steps; i++)
      {
        digitalWrite(step,HIGH);
        delay(delayTime);
        
        digitalWrite(step,LOW);
        delay(delayTime);
        
        if((digitalRead(limitPinUp)==LOW) || (analogRead(limitPinDown)<512) || (digitalRead(stopPin)==HIGH))
        {
          digitalWrite(redLED, HIGH);
          digitalWrite(greenLED, LOW);    
          break;
        }
      }
    }

    start=0;

    
  }
  digitalWrite(enbPin,HIGH);

}

void tft_setup()
{
  
  tft.fillRect(0,0,320,240, ILI9341_BLACK); 
 
//Interface Design begin=============================================
  tft.fillRect(0, 0, 179, 65, WHITE);             //display box button

  tft.setTextColor(BLACK); tft.setTextSize(2);
  tft.setCursor(1,5);     tft.print("Flow Rate(ml/h)");
  tft.setCursor(20,35);   tft.print(value[0],2);
  

//============================================================================
  //Increase-Decrease

  tft.fillRect(10,70,30,30,WHITE);                        //increase- button
  tft.fillTriangle(25,73, 12,97, 38,97, 0x0587);

  tft.fillRect(10,105,30,30,WHITE);                       //decrease- button
  tft.fillTriangle( 13,107, 37,107, 25,131, RED);        

  tft.setTextColor(WHITE); tft.setTextSize(2);

  tft.setCursor(45,79);   tft.print("Increase");
  tft.setCursor(45,112);  tft.print("Decrease");

  

//=============================================================================
  
  //Resolution text
  tft.setTextColor(WHITE);              //Resolution Text
  tft.setCursor(5, 145);
  tft.print("Resolution");
  tft.setCursor(122, 144 );
  tft.setTextSize(3);
  tft.print(":");  

//=========================================================================

  //Resolution buttons
  tft.setTextSize(2); tft.setTextColor(BLACK);    //Resolution Buttons

  tft.fillRect(5, 170, 40, 28, WHITE);            //Resolution - 100
  tft.setCursor(7,177); tft.print("100");

  tft.fillRect(51, 170, 37, 28, WHITE);           //Resolution - 10
  tft.setCursor(57,177); tft.print("10");

  tft.fillRect(94, 170, 37, 28, WHITE);           //Resolution - 1
  tft.setCursor(106,177); tft.print("1");
 
  tft.fillRect(5, 205, 40, 28, WHITE);            //Resolution - 0.1
  tft.setCursor(9,212); tft.print("0.1");
 
  tft.fillRect(51, 205, 54, 28, WHITE);           //Resolution - 0.01
  tft.setCursor(55,212); tft.print("0.01");

//==========================================================================


  
  tft.fillRect(190, 80, 100, 45, 0x6fed);              //start box
  tft.fillRoundRect(170, 189, 130, 48, 4, 0xFCA8);     //switch box

  tft.setCursor(210, 95);   tft.print("START");
  tft.setCursor(198, 195);  tft.print("SWITCH");
  tft.setCursor(191, 216);  tft.print("SYRINGE");
//========================================================================
  
  //Toggle buttons
  
  tft.fillRect(180, 0, 69, 65, 0x8d56);         //toggle flow rate
  tft.fillRect(250, 0, 70, 65, YELLOW);         //toggle volume

  tft.setTextSize(1); tft.setTextColor(BLACK);
  
  tft.setCursor(194, 14);   tft.print("TOGGLE");
  tft.setCursor(202, 29);   tft.print("FLOW");
  tft.setCursor(202, 44);   tft.print("RATE");

  tft.setCursor(265, 18);   tft.print("TOGGLE");
  tft.setCursor(265, 38);   tft.print("VOLUME");

//==============================================================================================
  syringe_mode = 0;
}


void switch_syringe()
{

  tft.fillScreen(0x0000);
  tft.setTextColor(WHITE); tft.setTextSize(2);
  tft.setCursor(10,10);
  tft.print("Choose Syringe Size");
  tft.setTextSize(3); tft.setCursor(237, 9); tft.print(":");
  
  tft.setTextSize(2);

  tft.drawRoundRect(20,45, 80,40, 5, WHITE);        tft.setCursor(36, 57);    tft.print("1 ml");
  tft.drawRoundRect(20,95, 80,40, 5, WHITE);        tft.setCursor(36, 107);   tft.print("2 ml");
  tft.drawRoundRect(20,145, 80,40, 5, WHITE);       tft.setCursor(36, 157);   tft.print("3 ml");  
  tft.drawRoundRect(20,195, 80,40, 5, WHITE);       tft.setCursor(36, 207);   tft.print("5 ml");  

  tft.drawRoundRect(120,45, 80,40, 5, WHITE);       tft.setCursor(136, 57);   tft.print("10 ml");     
  tft.drawRoundRect(120,95, 80,40, 5, WHITE);       tft.setCursor(136, 107);  tft.print("15 ml");     
  tft.drawRoundRect(120,145, 80,40, 5, WHITE);      tft.setCursor(136, 157);  tft.print("50 ml");     
  tft.drawRoundRect(120,195, 80,40, 5, WHITE);      tft.setCursor(127, 207);  tft.print("100 ml");     

  tft.drawRoundRect(220,45, 80,40, 5, WHITE);       tft.setCursor(227, 57);   tft.print("500 ml");
  tft.drawRoundRect(220,95, 80,40, 5, WHITE);       tft.setCursor(226, 107);  tft.print("1000ml");
  tft.drawRoundRect(220,145, 80,40, 5, WHITE);      tft.setCursor(250, 157);  tft.print("OK");
  syringe_mode=1;


}

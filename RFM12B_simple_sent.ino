#include <JeeLib.h>
#include <avr/sleep.h>


int vbat;

typedef struct {
int lp;
int bat;


} Payload;
Payload pomiar;

//#define rf12_sleep(x)

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

MilliTimer sendTimer;
void setup () {
    rf12_initialize(1, RF12_433MHZ, 210);
    pinMode(9, OUTPUT);
    pomiar.lp=0;
     rf12_control(0xC040); // bateria alarm
     rf12_control(0xC691); //bitrate 2.395kbps
//    rf12_control(0xC688); // bitrate 4.789kbps
     rf12_control(0x94C2); //BW 67kHz
    rf12_control(0x9820); // deviation 45kHz
    rf12_sleep(RF12_SLEEP);
    
}


void loop () {
   

pomiar.bat = getbat();
  rf12_sleep(RF12_WAKEUP);
    rf12_recvDone();
    if (rf12_canSend() && sendTimer.poll(4000)) {
      rf12_sendStart(0, &pomiar, sizeof pomiar,1);



  pomiar.lp += 1;
    

    }
  rf12_sleep(RF12_SLEEP);
  
Sleepy::loseSomeTime(2000);
  

}




int getbat(void){
  
  analogReference(INTERNAL);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  delay(20);
  vbat=map(analogRead(A0),0,1023,0,3416);
  digitalWrite(5, LOW);
  pinMode(5, INPUT);
  analogReference(DEFAULT);
  return vbat;
}


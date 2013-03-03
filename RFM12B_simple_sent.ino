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


void setup () {
  delay(150); // odczekanie na start RFM12B
  
  rf12_initialize(1, RF12_433MHZ, 210);
  pinMode(9, OUTPUT);
  pomiar.lp=0;
  rf12_control(0xC040); // bateria alarm
  rf12_control(0xC691); //bitrate 2.395kbps
//rf12_control(0xC688); // bitrate 4.789kbps
  rf12_control(0x94C2); //BW 67kHz
  rf12_control(0x9820); // deviation 45kHz
  rf12_sleep(RF12_SLEEP);
    
}


void loop () {
   

pomiar.bat = getbat();
  rf12_sleep(RF12_WAKEUP);

    

while (!rf12_canSend())
        rf12_recvDone();
        
      rf12_sendStart(0, &pomiar, sizeof pomiar,1);

    // set the sync mode to 2 if the fuses are still the Arduino default
    // mode 3 (full powerdown) can only be used with 258 CK startup fuses
    rf12_sendWait(2);
    rf12_sleep(RF12_SLEEP);
  
  pomiar.lp += 1;
  
Sleepy::loseSomeTime(2000); // czas uśpienia czujki do kolejnego pimiaru i wysyłki danych
  

}



// pomiar napięcia na zasilaniu
int getbat(void){
  
  analogReference(INTERNAL);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  delay(20);
  vbat=map(analogRead(A0),0,1023,0,3416); // potrzebny dzielnik z rezystorów 10k (D5-A0) oraz 4,7k (A0-GND), układ dobrany dla zasilania od2,0V do 3,3V
  digitalWrite(5, LOW);
  pinMode(5, INPUT);
  analogReference(DEFAULT);
  return vbat;
}


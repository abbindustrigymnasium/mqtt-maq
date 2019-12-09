#include "EspMQTTClient.h" //Tar med bibliotek och definerar pins
#define Rev_Get 4
#define pwm 5
#define D1 0
float rev , constant=3 , motor_power , error , const_i=0.001 , integral , kpe , real_speed; //Sätter variabler
int setspeed, Trigger, dtime,sas;
String Website_command, command;
unsigned long previoustime, currenttime = 0;

void setup() {
Serial.begin(9600);
attachInterrupt(digitalPinToInterrupt(Rev_Get), Fast, FALLING); //Sätter in en interupt
pinMode(pwm, OUTPUT),pinMode(D1, OUTPUT);
}

void onConnectionEstablished(); //Mqtt setup

EspMQTTClient client(
 "ABB_Indgym_Guest",           // Wifi ssid
  "Welcome2abb",           // Wifi password

  "maqiatto.com",  // MQTT broker ip
  1883,             // MQTT broker port
  "brian.nguyen@abbindustrigymnasium.se",            // MQTT username
  "7C8NXTmb&rsXBsnw5V",       // MQTT password
  "Mama",          // Client name
  onConnectionEstablished, // Connection established callback
  true,             // Enable web updater
  true              // Enable debug messages
);

void onConnectionEstablished(){ 
  client.subscribe("brian.nguyen@abbindustrigymnasium.se/SR", [] (const String &payload){ //Prenumerar till angiven adress
    Website_command = payload.charAt(0); //Filter för hastighet ex (h20). Ger möjlighet för att skicka andra värden
    if (Website_command== "h"){
      command = payload;
      Trigger = 1;
    }
 });
  client.publish("brian.nguyen@abbindustrigymnasium.se/SR", "Online"); //Skriver ut online till alla sina prenumeranter
}

void rpm(){ //Räknar tiden och räknar ut hur snabbt bilen åker i cm/s
  currenttime = millis();
if(currenttime - previoustime >= 200){
    dtime = (currenttime - previoustime)/1000;
    rev = rev/96;
    rev = rev*PI*3.7;
    real_speed = (rev/(currenttime-previoustime))*1000;
    previoustime = currenttime;
    rev=0;
    String car="b";
    car+=real_speed;
    client.publish("brian.nguyen@abbindustrigymnasium.se/SR", car); //Skickar värderna till mqtt. car lägger vi till för att märka vilken bil som är vilken
  }
}

void commands(){ //För att utmärka vad hemsidan vill göra
  if (Trigger ==1){
    command.remove(0,1);
    setspeed = command.toInt();
    Trigger = 0;
}
}
void integer(){ //Regler koden som anpassar så att motorn kör lika snabbt som given hastighet
  error = setspeed-real_speed;
  kpe=constant*error; //kp*e
  integral+=error*dtime*const_i;        
  motor_power += kpe+integral;
  if (motor_power >1023){
    motor_power = 1023;
  }
  else if (motor_power <0){
    motor_power = 0;
  }
}

void loop() { //loopen som kör hela tiden
  commands();
  Serial.println(" cm/s "+String(real_speed)+" PWM "+String(motor_power)+" Wanted "+setspeed+ "constant: "+constant);
  digitalWrite(D1, LOW);
  analogWrite(pwm, motor_power);
  client.loop(); 
  delay(150);
  rpm();
  integer(); 
}

ICACHE_RAM_ATTR void Fast() { //Varje gång den interupptas körs denna kod
  rev++;
}

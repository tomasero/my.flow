#include <RFduinoBLE.h>

const int LED_PIN = 1;
const int power1 = 5; //GPIO4
const int power2 = 4; //GPIO5
const int probe = 6; //GPIO6
const int r1 = 330;
const int vIn = 3;
int state = true;

// debounce time (in ms)
int debounce_time = 10;

// maximum debounce timeout (in ms)
int debounce_timeout = 100;

void setup () {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  digitalWrite(LED_PIN,HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  
  // configure the RFduino BLE properties
  RFduinoBLE.advertisementData = "fullness";
  RFduinoBLE.advertisementInterval = 500;
  RFduinoBLE.deviceName = "tampon";
  RFduinoBLE.txPowerLevel = -20;
  Serial.println("RFduino BLE Advertising interval is 500ms");
  Serial.println("RFduino BLE DeviceName: RFduino");
  Serial.println("RFduino BLE Tx Power Level: -20dBm");
  
  // start the BLE stack
  RFduinoBLE.begin();
  Serial.println("RFduino BLE stack started");
  
  initPower();
}

void loop () {
  float w = getWaterResistance();
  int waterResistance = (int)w;
  if (sendData(waterResistance)){
    RFduinoBLE.sendInt(waterResistance);
  } else {
    RFduino_ULPDelay(INFINITE);
  }
  Serial.println("------------------");
  delay(1000);
  switchPolarity();
  delay(1000);
}

boolean sendData(int res){
  //state = tampon and is over a certain threshold?
  return true;
}

void switchPolarity () {
  if (!state) {
    digitalWrite(power1, HIGH);
    digitalWrite(power2, LOW);
  } else {
    digitalWrite(power1, LOW);
    digitalWrite(power2, HIGH);
  }
  state = !state;
}

float getWaterResistance () {
  Serial.print("State: ");
  Serial.println(state);
  int value = analogRead(probe);
  float vOut = value * (vIn / 1023.0);
  Serial.print("Value: ");
  Serial.println(value);
  Serial.print("Voltage: ");
  Serial.println(vOut);
  int output = 0;
  if (!state) {
    output = (vOut * r1) / (vIn - vOut);
//    output = (40*voltage)/(1 - (voltage/5));
//    output = (voltage * resistorValue)/(5 - voltage);
  } else {
    output = (r1 * vIn / vOut) - r1;
//    output = (5 * resistorValue)/voltage - resistorValue;
//    output = (1000/vOut) - 200;
  }
  Serial.print("Output: ");
  Serial.println(output);
  //if output reaches a threshold, turn on bluetooth and send the data.
  return (int) output;
}

void initPower () {
   pinMode(power1, OUTPUT);
   pinMode(power2, OUTPUT); 
  digitalWrite(power1, HIGH);
  digitalWrite(power2, LOW);
}

void RFduinoBLE_onDisconnect()
{
  // don't leave the led on if they disconnect
  digitalWrite(LED_PIN, LOW);
}

void RFduinoBLE_onReceive(char *data, int len)
{
  //we should be able to turn electronics off from the app if need be.
  
  // if the first byte is 0x01 / on / true
  if (data[0])
    digitalWrite(LED_PIN, HIGH);
  else
    digitalWrite(LED_PIN, LOW);
}

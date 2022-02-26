// Refrenenced https://roboticsbackend.com/raspberry-pi-arduino-serial-communication/

void setup() {
  Serial.begin(9600);

}

void loop() {
 if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    Serial.print("You sent me: ");
    Serial.println(data);
  }
  
  /*
   * For Inital Test
   * Arduino sends out "Hello From Arduino" through serial 
   * for the raspberry pi to read and print
   * 
  Serial.println("Hello From Arduino");
  delay(1000);
  */
}

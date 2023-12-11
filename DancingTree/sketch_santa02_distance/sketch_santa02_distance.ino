#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
#define sigPin 8 //attach pin D3 Arduino to pin Signal of Relay
#define ledPin 7 //attach pin D3 Arduino to pin Trig of HC-SR04
#define MINCOUNT 5 //minium count before trigger
#define MAX_DIST 25 //max distance in cm
#define PLAYTIME 35500 //time to play
#define WAITTIME 60000 //time to wait 

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int count = 0;

void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(sigPin, OUTPUT); 
  pinMode(ledPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
}
void loop() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm");
  Serial.print(" - Count: ");
  Serial.print(count);
  Serial.println(" ");

  if (distance > MAX_DIST) {
    count++;
    if (count > MINCOUNT) {
      Serial.print("Playing!");
      digitalWrite(sigPin, HIGH);//play
      delay(PLAYTIME);
      Serial.print("Stop");
      digitalWrite(sigPin, LOW);
      digitalWrite(ledPin, HIGH);
      delay(WAITTIME); //wait for 3 mins before next check 
      digitalWrite(ledPin, LOW);
      Serial.print("Finished");
      count = 0;
    }
  }
  else {
    count = 0;
  }
  delay(200);  
}

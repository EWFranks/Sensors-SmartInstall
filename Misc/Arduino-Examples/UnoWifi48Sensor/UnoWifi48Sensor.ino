int inputPin=4; // define ultrasonic signal receiver pin ECHO to D4 
int outputPin=5; // define ultrasonic signal transmitter pin TRIG to D5
int waterPin = 1; //connect water sensor to analog interface 0
int gasPin = 2; //connect water sensor to analog interface 0
int vibPin = 3; //connect water sensor to analog interface 0
const int flamePin = 3;     // the number of the flame pin (D3)
int State = 0;         // variable for reading status
int prevState = 0;    // variable for switching flame reading

void setup()

{

  Serial.begin(9600); 
  pinMode(inputPin, INPUT); 
  pinMode(outputPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(flamePin, INPUT);

}

void loop()

{

  Serial.println("Distance: "+String(ultrasonic())); //Output distance
  Serial.println("Sound: "+String(sound()));
  Serial.println("Water Level: "+String(water()));
  Serial.println("Gas: "+String(gas()));
  Serial.println("Vibration: "+String(vibration()));
  delay(500);
}
int ultrasonic()
{
  digitalWrite(outputPin, LOW); 
  delayMicroseconds(2);

  digitalWrite(outputPin, HIGH); // Pulse for 10μ s to trigger ultrasonic detection

  delayMicroseconds(10); 
  digitalWrite(outputPin, LOW);

  int distance = pulseIn(inputPin, HIGH); // Read receiver pulse time 
  distance= distance/58; // Transform pulse time to distance 
  return distance;
}

int flame(int prev)
{
  State = digitalRead(flamePin);
  if (State == HIGH && State != prev) {     
    // Send Flame on message   
    Serial.println("Flame"); 
    prevState = HIGH; 
  } 
  else if(State == LOW && State != prev) {
    // Send flame off message
    Serial.println("No Flame"); 
    prevState = LOW;
  }
  return State;
}

int sound()
{
  int val;
  val=analogRead(0);   //connect mic sensor to Analog 0 
  return val;
}

int water()
{
  int level;
  level = analogRead(waterPin); 
  return level;
}

int gas()
{
  int gasReading;
  gasReading = analogRead(gasPin);
  return gasReading;
}

int vibration()
{
  int vib;
  vib = analogRead(vibPin);
  return vib;
}
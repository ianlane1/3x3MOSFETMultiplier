bool startTesting = false;
bool passFail = true;

#define Abits *((volatile uint8_t *)(&PORTA)) // A1 - A3 LSB will be A1
#define Bbits *((volatile uint8_t *)(&PORTD)) // B1 - B3 LSB will be B1
#define Dbits *((volatile uint8_t *)(&PINC)) // Answer bits LSB will be D1
#define Cbits *((volatile uint8_t *)(&PINL)) // may not be used

// If one of multiplications in the test fails it will increase this counter
int numFailed = 0;
// Create two lists, one for the correct answer and one for the failed answer, these have 64 spaces in case every multiplication fails
short int actualAnswer[64];
short int dAnswer[64];

bool testing(){
  // Double for loop interates through each possible combination of multiplication from 0-7.
  // Even if 3 * 2 gives the right answer of 6 since multiplication is commutative we must also test that 2 * 3 is also 6
  for (short int a = 0; a < 8; a++){
    for (short int b = 0; b < 8; b++){
      // Set the ports 
      Abits = a; 
      Bbits = b; 

      short int actual = a * b;
      delayMicroseconds(1); // A 1 microsecond delay is more than 5 times the needed delay for the multiplier to give out correct information
      short int d = Dbits;

      // If the numbers dont match, then the whole test fails, however we finish the entire test such that we know every number that failed
      if (d != actual){
        passFail = false;
        actualAnswer[numFailed] = actual;
        dAnswer[numFailed] = d;
        numFailed++;
      }
    }
  }
  // Will always return true unless the test fails at some point
  return passFail;
}

void setup() {
  DDRA = 0;
  DDRD = 0;
  DDRC = 0b11111111;
  DDRL = 0b11111111; 

  Serial.begin(9600);
  Serial.println("Enter anything on the serial monitor to begin testing");
}

void loop() {
  // One user gives an input to indicate they want to start the test, run through the test
  if (Serial.available()){
    startTesting = true;
    passFail = testing();
  }

  // Clear the serial buffer so that it will not infinitely run through the test and we can see where we failed
  while (Serial.available() > 0){
    Serial.read();
  }
  
  // If we have run through the test, indicate whether we passed or failed
  if (startTesting){
    if (passFail){
      Serial.println("The test passed!");
    }
    else {
      Serial.println("The test failed!");

      /* If the test fails, this will first print out the list of what the answer should have been
        Then it will print out the list of the answer our test gave us
        [ a, b, c, ...]
        [ x, y, z, ...]
        a is what the answer should have been, x is the answer our multiplier gave us, b is the next answer it should have been and so on
      */
      Serial.println("What the answer should have been: ");
      Serial.print("[ ");
      for (int i = 0; i < numFailed; i++){
        Serial.print(actualAnswer[i]);
        Serial.print(", ");
      }
      Serial.println(" ]");


      Serial.println("The answer the multiplier gave: ");
      Serial.print("[ ");
      for (int i = 0; i < numFailed; i++){
        Serial.print(dAnswer[i]);
        Serial.print(", ");
      }
      Serial.println(" ]");

      // Reset for next run through
      numFailed = 0;
      passFail = true;
    }
    Serial.println("Enter anything on the serial monitor to start again");
    startTesting = false;
  }
}

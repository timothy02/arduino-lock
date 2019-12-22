#include <Keypad.h>

#include <IRremote.h>
#include <IRremoteInt.h>

/* KEYPAD INIT */
const byte COLS = 4;
const byte ROWS = 4;

// A - CLEAR INPUT; B - LOCK; C - NEW PASSWORD
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {6, 7,  8, 9};
byte colPins[COLS] = {2, 3, 4, 5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


/* ACCESS CODE */
boolean isOpen = false;
boolean isTypingNewCode = false;

const int GREEN_LIGHT = 12;
const int YELLOW_LIGHT = 11;
const int RED_LIGHT = 10;

const int STATE_OPEN = 1;
const int STATE_NEW = 2;
const int STATE_CLOSED = 0;

char CODE[8];
char input[8];
int inputCounter = 0;


/* RC CODE */
const int RC_PIN = A0;
IRrecv irrecv(RC_PIN);
decode_results results;

void setup() {
  pinMode(GREEN_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT, OUTPUT);
  pinMode(RED_LIGHT, OUTPUT);

  CODE[0] = '1';
  CODE[1] = '2';
  CODE[2] = '3';
  CODE[3] = '4';
  CODE[4] = '5';
  CODE[5] = '6';
  CODE[6] = '7';
  CODE[7] = '0';

  irrecv.enableIRIn();
  
 Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();

   if (irrecv.decode(&results)) 
    {
         irrecv.resume();   // Receive the next value
    }
    if(results.value != 4294967295 && results.value != 0){
      Serial.println(results.value);
    }

    if(results.value == 16738455){
      key = '0';
    }

    if(results.value == 16724175){
      key = '1';
    }

    if(results.value == 15669479){
      key = '2';
    }

    if(results.value == 16743045){
      key = '3';
    }

    if(results.value == 16716015){
      key = '4';
    }

    if(results.value == 16726215){
      key = '5';
    }

    if(results.value == 16734885){
      key = '6';
    }

    if(results.value == 16728765){
      key = '7';
    }

    if(results.value == 16730805){
      key = '8';
    }

    if(results.value == 16732845){
      key = '9';
    }

    if(results.value == 16750695){
      key = 'A';
    }

    if(results.value == 16756815){
      key = 'B';
    }
 

    if(results.value == 16769055){
      key = 'C';
    }

    if(results.value == 16754775){
      key = 'D';
    }
 

  if(key != NO_KEY){
    if(!isTypingNewCode){
     blinkYellow();
    } else {
      if(readInputForNewCode(key)){ //gets fired when new code is entered
        isTypingNewCode = false;
        resetInput();
      }
    }

    if(!isTypingNewCode){
      if(key == 'A'){
        Serial.println("-- CLEAR INPUT --");
        resetInput();
      } else if(key == 'B'){
        Serial.println("-- LOCK --");
        resetInput();
        isOpen = false;
      } else if (key == 'C' && isOpen){
        Serial.println("-- TYPING NEW INPUT --");
        resetInput();
        isTypingNewCode = true;
        setLights(STATE_NEW);
      } else{
        readInput(key);
      }
    }
  }

  if(!isOpen && checkCode()){
    Serial.println("-- UNLOCK --");
    resetInput();
    isOpen = true;
  }

  if(isOpen && !isTypingNewCode){
     setLights(STATE_OPEN);
  } else if(!isTypingNewCode){
    setLights(STATE_CLOSED);
  }
}


void setLights(int state){
  switch(state){
    case STATE_OPEN:
      digitalWrite(GREEN_LIGHT, HIGH);
      digitalWrite(YELLOW_LIGHT, LOW);
      digitalWrite(RED_LIGHT, LOW);
    break;
    
    case STATE_NEW:
      digitalWrite(GREEN_LIGHT, HIGH);
      digitalWrite(YELLOW_LIGHT, HIGH);
      digitalWrite(RED_LIGHT, LOW);
    break;
    
    case STATE_CLOSED:
      digitalWrite(GREEN_LIGHT, LOW);
      digitalWrite(YELLOW_LIGHT, LOW);
      digitalWrite(RED_LIGHT, HIGH);
    break;
  }
}

void blinkYellow(){
  digitalWrite(YELLOW_LIGHT, HIGH);
  delay(100);
  digitalWrite(YELLOW_LIGHT, LOW);
}

void readInput(char key){
  if(inputCounter == 8){
    resetInput();
  }
  input[inputCounter] = key;
  inputCounter++;
  Serial.print("INPUT: ");
  Serial.println(input);
}

bool readInputForNewCode(char key){
  if(key == 'A' || key == 'B' || key == 'C'){
    return false;
  }
  Serial.println(inputCounter);
  CODE[inputCounter] = key;
  inputCounter++;
  Serial.print("CODE: ");
  Serial.println(CODE);
  
  if(inputCounter == 8){
    resetInput();
    return true;
  }
  
  return false;
}

void resetInput(){
    for(int i=0; i<8; i++){
      input[i] = 0x00;  
    }
    
    inputCounter = 0;
}

bool checkCode(){
  bool result = true;
  
  for(int i=0; i<8; i++){
    if(input[i] != CODE[i]){
      result = false;
    }
  }

  return result;
}

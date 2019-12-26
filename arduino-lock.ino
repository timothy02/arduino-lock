#include <Keypad.h>
#include <IRremote.h>
#include <IRremoteInt.h>

/* GLOBAL VARIABLES */
const int GREEN_LIGHT = 12;
const int YELLOW_LIGHT = 11;
const int RED_LIGHT = 10;

const int STATE_OPEN = 1;
const int STATE_NEW = 2;
const int STATE_CLOSED = 0;

const int DEFAULT_CODE_LENGTH = 8;
char DEFAULT_CODE[DEFAULT_CODE_LENGTH] = {'1', '2', '3', '4', '5', '6', '7', '8'};


int appState = STATE_CLOSED;
char CODE[DEFAULT_CODE_LENGTH];
char input[DEFAULT_CODE_LENGTH];
int inputCounter = 0;

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


/* RC INIT */
const int RC_PIN = A0;
IRrecv irrecv(RC_PIN);
decode_results results;

void setup() {
  pinMode(GREEN_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT, OUTPUT);
  pinMode(RED_LIGHT, OUTPUT);

  irrecv.enableIRIn();
  
  Serial.begin(9600);

  setCode(DEFAULT_CODE);
  setState(STATE_CLOSED);
}

void loop() {
  char key = resolveInput();

  performAction(key);
  setLights(appState);
}

char resolveInput(){
  char key = keypad.getKey();
  if (irrecv.decode(&results)){
       irrecv.resume();   // Receive the next value
  }
  
  if(decodeIrValue(results.value) != NO_KEY){
    key = decodeIrValue(results.value);
  }

  return key;
}

void performAction(char key){
  if(appState == STATE_OPEN){
      switch(key){
        case 'A':
          resetInput();
        break;
        
        case 'B':
          Serial.println("-- TYPING NEW CODE --");
          setState(STATE_NEW);
        break;

        case 'C':
          Serial.println("-- LOCKED --");
          setState(STATE_CLOSED);
        break;

        case NO_KEY:
        break;
        
        default:
          blinkYellow();
        break;
    }
  }

  if(appState == STATE_NEW){
    switch(key){
      case 'A':
        resetInput();
      break;

      case 'B': //Ignore those control keys
      case 'C':
      case 'D':
      case NO_KEY:
      break;

      default:
        if(readInputForNewCode(key)){ //gets fired when new code is entered
          resetInput();
          setState(STATE_OPEN);
        }
      break;
    }
  }

  if(appState == STATE_CLOSED){
    switch(key){
      case 'A':
        resetInput();
        blinkYellow();
      break;
  
      case 'B': //Ignore those control keys
      case 'C':
      case 'D':
      case NO_KEY:
      break;
  
      default:
        readInput(key);
        blinkYellow();
      break;
    }

    if(checkCode()){
      setState(STATE_OPEN);
      Serial.println("-- UNLOCK --");
      resetInput();
   }
  }
}

void setState(int state){
  appState = state;

  setLights(state);
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
  Serial.println("-- CLEAR INPUT --");
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

void setCode(char newCode[]){
  for(int i=0; i<8; i++){
    CODE[i] = newCode[i];
  }
}

char decodeIrValue(unsigned long irCode){
  switch(irCode){
    case 16738455:
      return '0';

    case 16724175:
      return '1';

    case 15669479:
      return '2';

    case 16743045:
      return '3';

    case 16716015:
      return '4';

    case 16726215:
      return '5';

    case 16734885:
      return '6';

    case 16728765:
      return '7';

    case 16730805:
      return '8';

    case 16732845:
      return '9';

    case 16750695:
      return 'A';

    case 16756815:
      return 'B';

    case 16769055:
      return 'C';

    case 16754775:
      return 'D';

    default:
      return NO_KEY;
  }
}

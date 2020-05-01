#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

#define direction_ 10
#define turn 11
#define buzzer 22
#define redBulb 30
#define blueBulb 32


//*********** LCD DISPLAY *****************
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);



const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {12, 2, 3, 4};
byte colPins[COLS] = {5, 6, 8, 9};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);

  // ---------Stepper motor ------------------------
  pinMode(direction_, OUTPUT);
  pinMode(turn, OUTPUT);

  // ---------Bulbs ------------------------
  pinMode(redBulb, OUTPUT);
  pinMode(blueBulb, OUTPUT);

  // ---------Buzzer ------------------------

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);

  // ---------Stepper motor ------------------------
  digitalWrite(direction_, HIGH);
  digitalWrite(turn, LOW);




  //---DISPLAY-------------
  lcd.begin(16, 2);
  lcd.home ();
  lcd.setCursor ( 2, 0 );
  lcd.print("DRUG INJECTOR");
  delay(2000);
  lcd.clear();


  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);



  sei();//allow interrupts
}


int flag = 0;
double currentDrugInjected, currentVolumeInjected, remainingDrugtobeInjected, remainingTime;


int cursor = 0, clearCursor = 0;  //for backspace function in display
String readFromDisplay = "";      //to put input readings temporarily

double bodyWeight, requiredDosage, drugConcentration, drugDrawnInSyringe, syringeVol, dilusionVolume; //inputs taken from the user
double dilutionDosage;  //this is the volume of the dilution that should be injected per minute


long microsteps;    // No of microsteps to empty the total volume of the syringe(m1/m2). This changes based on the syringe volume.
// Value to the variable is assinged in syringePosition() function (see readData tab)

long stepsForward;  // No of microsteps to move forward the thread bar for adjusting the initial position or the starting point before injecting the drug
long injectSteps;   // No of microsteps to move forward the thread bar for injecting the drug

double microstepsPermL; // No of microsteps to push 1mL
// Value to the variable is assinged in syringePosition() function (see readData tab)

double d_time;    // This delay in milliseconds will decide the speed of the stepper motor ( see void loop for the equation)

long  injectCounter = 0, backwardCounter = 0, fowardCounter = 0 ; //just a counter for counting microsteps

void loop() {

  lcd.setCursor ( 0, 0 );
  lcd.print("press * to ");
  lcd.setCursor ( 0, 1 );
  lcd.print("start ");

  //get input from the keypad
  char customKey = customKeypad.getKey();

  if (customKey && customKey == '*') { // start * button is for start the process
    delay(2000);
    readData();                        //calling readData function to get user inputs

    lcd.clear();
    lcd.setCursor ( 0, 0 );

    lcd.print("Adjusting......");
    syringePosition();                // This will adjust the stepper motor to a starting postion
    //based on the drug volume in the syringe ( see readData tab for function implementation)

    //equation for calculating dosage for the dilution
    dilutionDosage = (requiredDosage * bodyWeight * dilusionVolume) / (1000 * drugConcentration * drugDrawnInSyringe);

    d_time = (((unsigned long)500 * 60) * syringeVol) / (dilutionDosage * microsteps);  //delay in milliseconds which decides the speed of the stepper motor

    Serial.println(d_time);
    Serial.println(injectSteps);
    lcd.clear();
    lcd.setCursor ( 0, 0 );
    lcd.print("Injecting......");
    delay(2000);

    //**************  Injecting drug  *********************************
    inject();   //(see inject tab)

    lcd.clear();


    // When D is pressed, thread bar will go back.
    while (1) {
      char customKey = customKeypad.getKey();
      if (customKey && customKey == 'D') {
        back();
        break;
      }
    }
  }
}


ISR(TIMER1_COMPA_vect) { //timer1 interrupt 1Hz

  if (injectCounter > 0 && flag == 0) {
    flag = 1;
    currentVolumeInjected = injectCounter / microstepsPermL;    // upto 2 decimals in mL
    currentDrugInjected = (1000 * drugConcentration * drugDrawnInSyringe * currentVolumeInjected) / dilusionVolume; // upto 2 decimals in ug
    remainingDrugtobeInjected = (1000 * drugConcentration * drugDrawnInSyringe) - currentDrugInjected; // upto 2 decimals in ug
    remainingTime = ( injectSteps - injectCounter) / (microstepsPermL * dilutionDosage); // minutes

    String drug = String(currentDrugInjected);
    drug.remove(drug.indexOf(".")+ 2);
    
    String volume = String(round(currentVolumeInjected));
    String minutes = String(round(remainingTime));

    
    
    lcd.clear();
    lcd.setCursor ( 0, 0 );
    lcd.print("Injected " + drug + "ug" + "mL");



  }
}

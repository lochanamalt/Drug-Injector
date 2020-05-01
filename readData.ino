
// ---------Reading data from display -------------------
void readData() {

  // ****************   taking body weight of the patient  *********************************
  lcd.setCursor ( 0, 0 );
  lcd.print("Body weight?(kg)");

  collectData();

  bodyWeight = readFromDisplay.toDouble();
  lcd.clear();
  Serial.println(bodyWeight);


  // *******************************     taking prescibed dosage for the patient    **************************************
  lcd.setCursor ( 0, 0 );
  lcd.print("Dosage?ug/kg/min");

  collectData();

  requiredDosage = readFromDisplay.toDouble();
  lcd.clear();
  Serial.println(requiredDosage);



  // ****************   taking drug concentration (from the drug bottle) *********************************
  lcd.setCursor ( 0, 0 );
  lcd.print("Drug con?(mg/mL)");

  collectData();

  drugConcentration = readFromDisplay.toDouble();
  lcd.clear();
  Serial.println(drugConcentration);


  // ****************   taking amount of drug drawn in the syringe  *********************************
  lcd.setCursor ( 0, 0 );
  lcd.print("Drug drawn?(mL)");

  collectData();

  drugDrawnInSyringe = readFromDisplay.toDouble();
  lcd.clear();
  Serial.println(drugDrawnInSyringe);



  // ****************   taking syringe volume   *********************************
  lcd.setCursor ( 0, 0 );
  lcd.print("Syringe vol?(mL)");

  collectData();

  syringeVol = readFromDisplay.toDouble();
  lcd.clear();
  Serial.println(syringeVol);

  // *************************  taking total volume of the dilution  *************************************************

  //This is to input drug volume inside the syringe.

  lcd.setCursor ( 0, 0 );
  lcd.print("Dilution vol?(mL)");

  collectData();

  dilusionVolume = readFromDisplay.toDouble();
  lcd.clear();
  Serial.println(dilusionVolume);

}


void collectData() {

  readFromDisplay = "";
  cursor = 0;

  while (1)
  {
    char customKey = customKeypad.getKey();
    if (customKey) {
      if (customKey == '#') {    // "#" is considered as the enter button. When pressed, go to next input
        break;
      }
      else if (customKey == 'C') {    // C is the backspace key
        lcd.setCursor (clearCursor, 1 );
        lcd.print(" ");
        cursor = clearCursor;
        clearCursor = clearCursor - 1;
        readFromDisplay.remove(readFromDisplay.length() - 1);
      }
      else {
        lcd.setCursor ( cursor, 1 );
        lcd.print(customKey);
        cursor = cursor + 1;
        clearCursor = cursor - 1;
        readFromDisplay = readFromDisplay + customKey;
      }
    }
  }


}



//------------ Doing adjustments -------------------------------------

void syringePosition() {


  switch ((int)syringeVol) {                         //switch case based on the syringe volume

    case 20: microsteps = 62693;                  // microsteps to empty a 20mL syringe (m1)
      microstepsPermL = microsteps / syringeVol;  // No of microsteps to push 1mL

      switch ((int)dilusionVolume) {
        case 10: stepsForward = microstepsPermL * 10;
          injectSteps = microstepsPermL * 10 ;
          break;
        default : stepsForward = 0;
          injectSteps = microstepsPermL * 20 ;
          break;
      }
      break;

    default: microsteps = 54856;                     // microsteps to empty a 50mL syringe (m1)
      microstepsPermL = microsteps / syringeVol;    // No of microsteps to push 1mL

      switch ((int)dilusionVolume) {                   //switch case based on the drug volume
        case 10: stepsForward = microstepsPermL * 40;  //No of microsteps to push 4mL length in syringe
          //( because drug volume is 1mL. And thread bar should go forward 4mL length to start injecting)
          injectSteps = microstepsPermL * 10 ;        //No of microsteps to inject 1mL (drug volume is 1mL in this case)
          break;
        case 20: stepsForward = microstepsPermL * 30;
          injectSteps = microstepsPermL * 20 ;
          break;
        case 30: stepsForward = microstepsPermL * 20;
          injectSteps = microstepsPermL * 30 ;
          break;
        case 40: stepsForward = microstepsPermL * 10;
          injectSteps = microstepsPermL * 40 ;
          break;
        default : stepsForward = 0;
          injectSteps = microstepsPermL * 50 ;
          break;
      }
      break;

  }

  fowardCounter = 0 ;         //for counting the no of microsteps

  //stepper motor will rotate to adjust the initial position before injecting the drug
  while (fowardCounter != stepsForward) {

    digitalWrite(turn, HIGH);
    delayMicroseconds(100);
    digitalWrite(turn, LOW);
    delayMicroseconds(100);
    fowardCounter = fowardCounter + 1;
  }
  fowardCounter = 0;
  delay(1000);
}

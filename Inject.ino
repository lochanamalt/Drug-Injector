
//**************  Injecting drug  *********************************

void inject() {
  injectCounter = 0;
  while (injectCounter != injectSteps) {
    digitalWrite(blueBulb, HIGH);
    digitalWrite(turn, HIGH);
    delay(d_time);
    digitalWrite(turn, LOW);
    delay(d_time);
    digitalWrite(blueBulb, LOW);
    injectCounter = injectCounter + 1;
  }
  
  injectCounter = 0;

  // Buzzer will ring after finishing the injecting process. And red bulb is switched on.
  digitalWrite(buzzer, LOW);
  digitalWrite(redBulb, HIGH);
  delay(1000);
}


//**************  Move backward the thread bar after injecting process   *********************************

void back() {
  digitalWrite(redBulb, LOW);   //Red bulb and Buzeer are switched off.
  digitalWrite(buzzer, HIGH);
  digitalWrite(direction_, LOW);
  digitalWrite(turn, LOW);
  backwardCounter = 0;

  while (backwardCounter != (injectSteps + stepsForward)) { // backward total microsteps
    digitalWrite(turn, HIGH);
    delayMicroseconds(100);
    digitalWrite(turn, LOW);
    delayMicroseconds(100);
    backwardCounter = backwardCounter + 1;
  }
  backwardCounter = 0;
  delay(1000);
  digitalWrite(direction_, HIGH);
}

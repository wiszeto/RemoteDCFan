// Wilson Szeto, Remote control fan, 24 November 2020, EE laboratory final project 

#include <Servo.h> 

#include <LiquidCrystal.h> 

#include <IRremote.h> 

  

// servo declarations 

Servo serv; 

int servoPin = 9; 

  

//remote declarations 

char KeyName[17]; 

char KeyName1[17]; 

  

//ir reciver declarations 

IRrecv irrecv(21); 

decode_results results; 

  

//LCD display declarations 

const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4; 

LiquidCrystal lcd(rs, en, d4, d5, d6, d7); 

  

//Rotary declarations 

const int A = 3; 

const int B = 2; 

int counter = 0; 

int aState; 

int aLastState; 

volatile int switchPin = 20; 

int switchState = HIGH; 

int p = 0; 

int re; 

  

void setup() { 

  //servo setup 

  serv.attach(servoPin); 

  serv.write(0); 

  

  //lcd setup 

  lcd.begin(16, 2); 

  

  //Serial monitor setup 

  Serial.begin(9600); 

  lcd.setCursor(0, 0); 

  lcd.print("angle:    "); 

  

  

  //rotary setup 

  pinMode(A, INPUT); 

  pinMode(B, INPUT); 

  aLastState = digitalRead(A); 

  pinMode(switchPin, INPUT_PULLUP); 

  attachInterrupt(digitalPinToInterrupt(switchPin), autorotate, FALLING); 

  

  // ir setup 

  irrecv.enableIRIn(); 

} 

  

void loop() { 

  //clear display 

  lcd.setCursor(0, 1); 

  lcd.print("             "); 

  lcd.setCursor(15, 0); 

  lcd.print(" "); 

  

  //auto rotate 

  while (p % 2 == 1) { 

    Serial.print(p); 

    //indicate that auto rotate is on by printing A on lcd display 

    lcd.setCursor(15, 0); 

    lcd.print("A"); 

    for (int i = 0; i <= 180; i++) { 

      //pan left 

      serv.write(i); 

      lcd.setCursor(7, 0); 

      lcd.print(i); 

      rotationconditions(i); 

      delay(10); 

    } 

    delay(1000); 

    for (int i = 180; i >= 0; i--) { 

      //pan right 

      serv.write(i); 

      lcd.setCursor(7, 0); 

      lcd.print(i); 

      rotationconditions(i); 

      delay(10); 

    } 

    delay(1000); 

  } 

  

  //remote pressed 

  if (irrecv.decode(&results)) { 

    Serial.println(results.value, HEX); 

    Serial.println(results.value); 

    translateIR(); 

    rotationconditions(counter); 

    if (results.value == 16720605) { 

      //when left button is clicked 

      turningLeft(20); 

      re = 1; 

    } 

    if (results.value == 16761405) { 

      //when right button is clicked 

      turningRight(20); 

      re = 0; 

    } 

    if (results.value == 4294967295) { 

      //when repeat is activated  

      if (re == 0) { 

        turningRight(20); 

      } 

      if (re == 1) { 

        turningLeft(20); 

      } 

    } 

    irrecv.resume(); 

  } 

  

  //rotation 

  rotationconditions(counter); 

  if (aState != aLastState) { 

    if (digitalRead(B) != aState) { 

      turningLeft(3); 

    } else { 

      turningRight(3); 

    } 

    Serial.print("Position: "); 

    Serial.println(counter); 

  } 

  aLastState = aState; 

} 

  

void turningLeft(int speeds) { 

  //turns servo to the left and updates lcd screen 

  counter = counter + speeds; 

  lcd.setCursor(0, 1); 

  lcd.print("TURNING LEFT"); 

  serv.write(counter); 

  lcd.setCursor(7, 0); 

  lcd.print(counter); 

  rotationconditions(counter); 

} 

  

void turningRight(int speeds) { 

  //turns servo to the right and updates lcd screen 

  counter = counter - speeds; 

  lcd.setCursor(0, 1); 

  lcd.print("TURNING RIGHT"); 

  serv.write(counter); 

  lcd.setCursor(7, 0); 

  lcd.print(counter); 

  rotationconditions(counter); 

} 

  

void autorotate() { 

  p++; 

} 

  

void rotationconditions(int type) { 

  aState = digitalRead(A); 

  // if angle is above 180, it will set the angle back to 180 

  if (type > 180) { 

    counter = 180; 

    lcd.setCursor(7, 0); 

    lcd.print(counter); 

  } 

  //if the angle is under 0, it will set the angle back to 0 

  if (type < 0) { 

    counter = 0; 

    lcd.setCursor(7, 0); 

    lcd.print(counter); 

  } 

  /* 

  if the number is below 100, it will set the third digit blank.  

  if this is not implemented, it will print 960 instead of 96 on the lcd display.  

  This is because the 0 is left over from the 100.  

  Same thing if it is under 10. */ 

  if (type < 100) { 

    lcd.setCursor(9, 0); 

    lcd.print(" "); 

  } 

  if (type < 10) { 

    lcd.setCursor(8, 0); 

    lcd.print(" "); 

  } 

  type = 0; 

} 

  

void  LCDisplayKey() { 

  lcd.setCursor(0, 0); 

  lcd.print(KeyName); 

  lcd.setCursor(0, 1); 

  lcd.print(KeyName1); 

} 

  

void translateIR() // takes action based on IR code received 

// describing Remote IR codes 

{ 

  switch (results.value) 

  { 

    case 0xFF22DD: strcpy(KeyName, " Turning Left"); Serial.println(" LEFT"); break; 

    case 0xFFC23D: strcpy(KeyName, " Turning Right"); Serial.println(" RIGHT"); break; 

    case 0xFFFFFFFF: strcpy(KeyName, " REPEAT"); Serial.println(" REPEAT"); break; 

    default:   strcpy(KeyName, " Invalid Code"); Serial.println(" Invalid Code"); break; 

  }// End Case 

  

  delay(500); // Do not get immediate repeat 

} //END translateIR function 

#include <SimbleeForMobile.h>
#include <SimbleeBLE.h>
#include <SimbleeForMobileClient.h>
#include <SimbleeCloud.h>
#include <Simblee_ENS210.h>

Simblee_ENS210 ENS210;

//SimbleeCloud ESNs
unsigned int userId = 0xe42532dd;
unsigned int destESN = 0x60606060;

//Simblee Cloud has to pass through a SimbleeForMobile Client instance
SimbleeForMobileClient client;
SimbleeCloud cloud(&client);

//Arrays sent to Cloud
char lightValue[6];
char temperatureValue[7];
char humidityValue[7];
char CO2Value[6];

//Character Arrays storing values received from the sensors
char valueA[5]; //Light Values
char valueD[5]; //ECO2 Values

//SimbleeForMobile Things

//Buttons
int AtoB;
int BtoA;
int otherPlant;

//Text
int iPlant, forMobile;
int homeText, loginText;
int welcomeMessage, ESNText;
int enterPlant, enterName;
int lightText, tempText, humText, CO2Text;
int lightVal, tempVal, humVal, CO2Val;
int unitLight, unitTemp, unitHum, unitCO2;
int displayText, nameAloe, nameOrchid, nameTulip, nameFern, nameCactus, nameFicus, unrecognizedText;
int keyText, redText, greenText;

//Text Fields
int userName;
int plantField;

//'Pointer' Variables
int screenValue = 1;
int whatPlant = 0;

//Plants
String aloeVera = String("Aloe Vera");
String orchid = String("Orchid");
String tulip = String("Tulip");
String fern = String("Fern");
String cactus = String("Cactus");
String ficus = String("Ficus");

//Variables storing character arrays into integers/floats for compare functions
uint16_t lightCheck;
float tempCheck;
float humCheck;
uint16_t CO2Check;

void setup() {
  Serial.begin(115200);
  delay(100);

  printf("\nRFDigital ENS210 Temperature & Humidity sensor simple example\n\n");
  if (!ENS210.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }

  valueA[4] = '\0';
  valueD[4] = '\0';

  SimbleeForMobile.deviceName = "Compact";
  SimbleeForMobile.advertisementData = "App"; 

  Serial.print("The ESN of my Simblee is 0x");
  Serial.println(cloud.myESN, HEX);
  cloud.userID = userId;

  SimbleeForMobile.begin();
}

void loop() {
  
  //Storing Temperature Data into float variable
  float temperature;
  temperature = ENS210.readTemp();

  //Storing Humidity Data into float variable
  float humidity;
  humidity = ENS210.readHumidity();

  //Converting Temperature float into Character Array
  String stringVal = "";
  stringVal+=String(int(temperature))+ "."+String(getDecimal(temperature));
  char valueB[stringVal.length()+1];
  stringVal.toCharArray(valueB,stringVal.length()+1);

  //Converting Humidity Float into Character Array
  String stringVal2 = "";
  stringVal2+=String(int(humidity))+ "."+String(getDecimal(humidity));
  char valueC[stringVal2.length()+1];
  stringVal2.toCharArray(valueC,stringVal2.length()+1);

  //Storing Light Character Array into new array with pointer to be sent to cloud
  lightValue[0] = '5';
  lightValue[1] = valueA[0];
  lightValue[2] = valueA[1];
  lightValue[3] = valueA[2];
  lightValue[4] = valueA[3];
  lightValue[5] = valueA[4];
  cloud.send(destESN, lightValue, sizeof(lightValue));

  //Storing Temperature Character Array into new array with a pointer to be sent to cloud
  temperatureValue[0] = '6';
  temperatureValue[1] = valueB[0];
  temperatureValue[2] = valueB[1];
  temperatureValue[3] = valueB[2];
  temperatureValue[4] = valueB[3];
  temperatureValue[5] = valueB[4];
  cloud.send(destESN, temperatureValue, sizeof(temperatureValue));

  //Storing Humidity Array into new array with a pointer to be sent to cloud
  humidityValue[0] = '7';
  humidityValue[1] = valueC[0];
  humidityValue[2] = valueC[1];
  humidityValue[3] = valueC[2];
  humidityValue[4] = valueC[3];
  humidityValue[5] = valueC[4];
  cloud.send(destESN, humidityValue, sizeof(humidityValue));

  //Storing CO2 Array into new array with a pointer to be sent to cloud
  CO2Value[0] = '8';
  CO2Value[1] = valueD[0];
  CO2Value[2] = valueD[1];
  CO2Value[3] = valueD[2];
  CO2Value[4] = valueD[3];
  CO2Value[5] = valueD[4];
  cloud.send(destESN, CO2Value, sizeof(CO2Value));

  //Compress character arrays from sensors into integers/floats for compare functions
  lightCheck = atoi(valueA);
  tempCheck = atof(valueB);
  humCheck = atof(valueC);
  CO2Check = atoi(valueD);

  //Update sensor readings on the Simblee Mobile App
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.updateText(lightVal, valueA);
    SimbleeForMobile.updateText(tempVal, valueB);
    SimbleeForMobile.updateText(humVal, valueC);
    SimbleeForMobile.updateText(CO2Val, valueD);
  }

  //Code to determine what plant has been selected in the Mobile App to update colors of sensor readings in the App to determine whether conditions are desirable or not
  if (whatPlant == 0 && screenValue == 2){
    whatHappens0();
  }
  if (whatPlant == 1 && screenValue == 2){
    whatHappens1();
  }
  if (whatPlant == 2 && screenValue == 2){
    whatHappens2();
  }
  if (whatPlant == 3 && screenValue == 2){
    whatHappens3();
  }
  if (whatPlant == 4 && screenValue == 2){
    whatHappens4();
  }
  if (whatPlant == 5 && screenValue == 2){
    whatHappens5();
  }
  if (whatPlant == 6 && screenValue == 2){
    whatHappens6();
  }
  if (whatPlant == 7 && screenValue == 2){
    whatHappens7();
  }
  
  //Process must be called in loop function
  SimbleeForMobile.process(); 
  cloud.process(); 
}

void SimbleeForMobile_onConnect()
{
  SimbleeBLE.dualModeBegin();
  Serial.println("Simblee For Mobile Connected");
}

void SimbleeForMobile_onDisconnect () {
  SimbleeBLE.dualModeEnd();
  Serial.println("SimbleeForMobile Disconnected");
}

void ui(){
  if (cloud.connect()){
    Serial.println("Cloud Connected");
  }else{
    Serial.println("Cloud Disconnected");
  }
  
  SimbleeForMobile.beginScreen(WHITE, PORTRAIT);

  //Login Screen
  iPlant = SimbleeForMobile.drawText(105, 30, "iPlant", BLACK, 50);
  forMobile = SimbleeForMobile.drawText(120, 80, "For Mobile", BLACK, 20);
  loginText = SimbleeForMobile.drawText(145, 510, "Login", BLACK, 10);
  welcomeMessage = SimbleeForMobile.drawText(100, 350, "Welcome back!", BLACK, 20);
  enterName = SimbleeForMobile.drawText(60, 180, "Enter your username here: ", BLACK, 15);
  userName = SimbleeForMobile.drawTextField(60, 200, 200, "", "Username: ", BLACK, WHITE);
  AtoB = SimbleeForMobile.drawButton(200, 500, 100, "NEXT", BOX_TYPE);
  SimbleeForMobile.setVisible(AtoB, false);
  SimbleeForMobile.setVisible(welcomeMessage, false);

  //Home-Data Screen
  homeText = SimbleeForMobile.drawText(140, 40, "Home", BLACK, 20);
  BtoA = SimbleeForMobile.drawButton(20, 500, 100, "Logout", BOX_TYPE);
  enterPlant = SimbleeForMobile.drawText(60, 120, "Enter your plant name here: ", BLACK, 15);
  plantField = SimbleeForMobile.drawTextField(60, 140, 200, "", "Plant: ", BLACK, WHITE);
  lightText = SimbleeForMobile.drawText(50, 220, "Light: ", BLACK, 20);
  tempText = SimbleeForMobile.drawText(50, 270, "Temperature: ", BLACK, 20);
  humText = SimbleeForMobile.drawText(50, 320, "Humidity: ", BLACK, 20);
  CO2Text = SimbleeForMobile.drawText(50, 370, "CO2: ", BLACK, 20);
  lightVal = SimbleeForMobile.drawText(190, 220, "", BLACK, 20);
  tempVal = SimbleeForMobile.drawText(190, 270, "", BLACK, 20);
  humVal = SimbleeForMobile.drawText(190, 320, "", BLACK, 20);
  CO2Val = SimbleeForMobile.drawText(190, 370, "", BLACK, 20);
  unitLight = SimbleeForMobile.drawText(240, 220, "cd", BLACK, 20);
  unitTemp = SimbleeForMobile.drawText(240, 270, "\xb0" "C", BLACK, 20);
  unitHum = SimbleeForMobile.drawText(240, 320, "%", BLACK, 20);
  unitCO2 = SimbleeForMobile.drawText(240, 370, "ppm", BLACK, 20);
  displayText = SimbleeForMobile.drawText(90, 120, "Displaying: ", BLACK, 15);
  nameAloe = SimbleeForMobile.drawText(170, 120, "Aloe Vera", BLACK, 15);
  nameOrchid = SimbleeForMobile.drawText(170, 120, "Orchid", BLACK, 15);
  nameTulip = SimbleeForMobile.drawText(170, 120, "Tulip", BLACK, 15);
  nameFern = SimbleeForMobile.drawText(170, 120, "Fern", BLACK, 15);
  nameCactus = SimbleeForMobile.drawText(170, 120, "Cactus", BLACK, 15);
  nameFicus = SimbleeForMobile.drawText(170, 120, "Ficus", BLACK, 15);
  unrecognizedText = SimbleeForMobile.drawText(75, 120, "Unrecognized Plant Selected", BLACK, 15);
  otherPlant = SimbleeForMobile.drawButton(85, 160, 150, "Select another plant", BLUE, TEXT_TYPE);
  keyText = SimbleeForMobile.drawText(80,430, "Key:", BLACK, 10);
  greenText = SimbleeForMobile.drawText(110, 420, "Green: Desirable Conditions", GREEN, 10);
  redText = SimbleeForMobile.drawText(110, 440, "Red: Undesirable Conditions", RED, 10);

  //Initialise Home-Data Screen Modules to invisible
  SimbleeForMobile.setVisible(homeText, false);
  SimbleeForMobile.setVisible(BtoA, false);
  SimbleeForMobile.setVisible(enterPlant, false);
  SimbleeForMobile.setVisible(plantField, false);
  SimbleeForMobile.setVisible(lightText, false);
  SimbleeForMobile.setVisible(tempText, false);
  SimbleeForMobile.setVisible(humText, false);
  SimbleeForMobile.setVisible(CO2Text, false);
  SimbleeForMobile.setVisible(lightVal, false);
  SimbleeForMobile.setVisible(tempVal, false);
  SimbleeForMobile.setVisible(humVal, false);
  SimbleeForMobile.setVisible(CO2Val, false);
  SimbleeForMobile.setVisible(unitLight, false);
  SimbleeForMobile.setVisible(unitTemp, false);
  SimbleeForMobile.setVisible(unitHum, false);
  SimbleeForMobile.setVisible(unitCO2, false);
  SimbleeForMobile.setVisible(displayText, false);
  SimbleeForMobile.setVisible(nameAloe, false);
  SimbleeForMobile.setVisible(nameOrchid, false);
  SimbleeForMobile.setVisible(nameTulip, false);
  SimbleeForMobile.setVisible(nameFern, false);
  SimbleeForMobile.setVisible(nameCactus, false);
  SimbleeForMobile.setVisible(nameFicus, false);
  SimbleeForMobile.setVisible(otherPlant, false);
  SimbleeForMobile.setVisible(unrecognizedText, false);
  SimbleeForMobile.setVisible(keyText, false);
  SimbleeForMobile.setVisible(greenText, false);
  SimbleeForMobile.setVisible(redText, false);

  //Set Button Press Events
  SimbleeForMobile.setEvents(AtoB, EVENT_PRESS);
  SimbleeForMobile.setEvents(BtoA, EVENT_PRESS);
  SimbleeForMobile.setEvents(otherPlant, EVENT_PRESS);

  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event){
  //If Button AtoB is pressed, show Home Screen. If Button BtoA is pressed, show Login Screen.
  if (event.id == AtoB){
    showScreenB();
  }
  else if (event.id == BtoA){
    showScreenA();
  }

  //Code to set Next Button and Welcome Message visible or not
  if (event.id == userName){
    if (event.text != NULL){
      SimbleeForMobile.setVisible(AtoB, true);
      SimbleeForMobile.setVisible(welcomeMessage, true);
    }
  }else if (screenValue == 2 || event.text == NULL || event.text == ""){
    if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(AtoB, false);
    SimbleeForMobile.setVisible(welcomeMessage, false);
    SimbleeForMobile.updateText(userName, "");
    }
  }

  //Code to set Plant Text Field visible/invisible + set Plant name visible/invisible
  if (event.id == plantField){
   String userPlant(event.text);
   
    if (userPlant.equals(aloeVera)){ 
            whatPlant = 2;
            SimbleeForMobile.updateText(plantField, "");
            SimbleeForMobile.setVisible(plantField, false);
            SimbleeForMobile.setVisible(enterPlant, false);  
            SimbleeForMobile.setVisible(displayText, true);
            SimbleeForMobile.setVisible(nameAloe, true);
            SimbleeForMobile.setVisible(otherPlant, true);
        }
     if (userPlant.equals(orchid)){
      whatPlant = 3;
      SimbleeForMobile.updateText(plantField, "");
      SimbleeForMobile.setVisible(plantField, false);
      SimbleeForMobile.setVisible(enterPlant, false);  
      SimbleeForMobile.setVisible(displayText, true);
      SimbleeForMobile.setVisible(nameOrchid, true);
      SimbleeForMobile.setVisible(otherPlant, true);
      
     }
     if (userPlant.equals(tulip)){
      whatPlant = 4;
      SimbleeForMobile.updateText(plantField, "");
      SimbleeForMobile.setVisible(plantField, false);
      SimbleeForMobile.setVisible(enterPlant, false);  
      SimbleeForMobile.setVisible(displayText, true);
      SimbleeForMobile.setVisible(nameTulip, true);
      SimbleeForMobile.setVisible(otherPlant, true);  
     }
     if (userPlant.equals(fern)){
      whatPlant = 5;
      SimbleeForMobile.updateText(plantField, "");
      SimbleeForMobile.setVisible(plantField, false);
      SimbleeForMobile.setVisible(enterPlant, false);  
      SimbleeForMobile.setVisible(displayText, true);
      SimbleeForMobile.setVisible(nameFern, true);
      SimbleeForMobile.setVisible(otherPlant, true);
     }
     if (userPlant.equals(cactus)){
      whatPlant = 6;
      SimbleeForMobile.updateText(plantField, "");
      SimbleeForMobile.setVisible(plantField, false);
      SimbleeForMobile.setVisible(enterPlant, false);  
      SimbleeForMobile.setVisible(displayText, true);
      SimbleeForMobile.setVisible(nameCactus, true);
      SimbleeForMobile.setVisible(otherPlant, true);
     }
     if (userPlant.equals(ficus)){
      whatPlant = 7;
      SimbleeForMobile.updateText(plantField, "");
      SimbleeForMobile.setVisible(plantField, false);
      SimbleeForMobile.setVisible(enterPlant, false);  
      SimbleeForMobile.setVisible(displayText, true);
      SimbleeForMobile.setVisible(nameFicus, true);
      SimbleeForMobile.setVisible(otherPlant, true);
     }
     if (userPlant != NULL && whatPlant == 0){
      whatPlant = 1;
      SimbleeForMobile.updateText(plantField, "");
      SimbleeForMobile.setVisible(plantField, false);
      SimbleeForMobile.setVisible(enterPlant, false);  
      SimbleeForMobile.setVisible(unrecognizedText, true);
      SimbleeForMobile.setVisible(otherPlant, true);
     }
  }

  if (event.id == otherPlant){
    whatPlant = 0;
    if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(plantField, true);
    SimbleeForMobile.setVisible(enterPlant, true);
    SimbleeForMobile.setVisible(displayText, false);
    SimbleeForMobile.setVisible(nameAloe, false);
    SimbleeForMobile.setVisible(nameOrchid, false);
    SimbleeForMobile.setVisible(nameTulip, false);
    SimbleeForMobile.setVisible(nameFern, false);
    SimbleeForMobile.setVisible(nameCactus, false);
    SimbleeForMobile.setVisible(nameFicus, false);
    SimbleeForMobile.setVisible(unrecognizedText, false);
    SimbleeForMobile.setVisible(otherPlant, false);
    }
  }
}

void showScreenB(){
  screenValue = 2;
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(homeText, true);
    SimbleeForMobile.setVisible(BtoA, true);
    SimbleeForMobile.setVisible(enterPlant, true);
    SimbleeForMobile.setVisible(plantField, true);
    SimbleeForMobile.setVisible(lightText, true);
    SimbleeForMobile.setVisible(tempText, true);
    SimbleeForMobile.setVisible(humText, true);
    SimbleeForMobile.setVisible(CO2Text, true);
    SimbleeForMobile.setVisible(lightVal, true);
    SimbleeForMobile.setVisible(tempVal, true);
    SimbleeForMobile.setVisible(humVal, true);
    SimbleeForMobile.setVisible(CO2Val, true);
    SimbleeForMobile.setVisible(unitLight, true);
    SimbleeForMobile.setVisible(unitTemp, true);
    SimbleeForMobile.setVisible(unitHum, true);
    SimbleeForMobile.setVisible(unitCO2, true);
    SimbleeForMobile.setVisible(iPlant, false);
    SimbleeForMobile.setVisible(forMobile, false);
    SimbleeForMobile.setVisible(loginText, false);
    SimbleeForMobile.setVisible(welcomeMessage, false);
    SimbleeForMobile.setVisible(enterName, false);
    SimbleeForMobile.setVisible(userName, false);
    SimbleeForMobile.setVisible(AtoB, false);
  }
}

void showScreenA(){
  screenValue = 1;
  whatPlant = 0;
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(homeText, false);
    SimbleeForMobile.setVisible(BtoA, false);
    SimbleeForMobile.setVisible(enterPlant, false);
    SimbleeForMobile.setVisible(plantField, false);
    SimbleeForMobile.setVisible(lightText, false);
    SimbleeForMobile.setVisible(tempText, false);
    SimbleeForMobile.setVisible(humText, false);
    SimbleeForMobile.setVisible(CO2Text, false);
    SimbleeForMobile.setVisible(lightVal, false);
    SimbleeForMobile.setVisible(tempVal, false);
    SimbleeForMobile.setVisible(humVal, false);
    SimbleeForMobile.setVisible(CO2Val, false);
    SimbleeForMobile.setVisible(unitLight, false);
    SimbleeForMobile.setVisible(unitTemp, false);
    SimbleeForMobile.setVisible(unitHum, false);
    SimbleeForMobile.setVisible(unitCO2, false);
    SimbleeForMobile.setVisible(iPlant, true);
    SimbleeForMobile.setVisible(forMobile, true);
    SimbleeForMobile.setVisible(loginText, true);
    SimbleeForMobile.setVisible(enterName, true);
    SimbleeForMobile.setVisible(userName, true);
    SimbleeForMobile.setVisible(nameAloe, false);
    SimbleeForMobile.setVisible(nameOrchid, false);
    SimbleeForMobile.setVisible(nameFern, false);
    SimbleeForMobile.setVisible(nameCactus, false);
    SimbleeForMobile.setVisible(nameFicus, false);
    SimbleeForMobile.setVisible(nameTulip, false);
    SimbleeForMobile.setVisible(plantField, false);
    SimbleeForMobile.setVisible(displayText, false);
    SimbleeForMobile.setVisible(unrecognizedText, false);
    SimbleeForMobile.setVisible(otherPlant, false);
    SimbleeForMobile.setVisible(keyText, false);
    SimbleeForMobile.setVisible(greenText, false);
    SimbleeForMobile.setVisible(redText, false);
  }
}

void whatHappens0(){
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(keyText, false);
    SimbleeForMobile.setVisible(greenText, false);
    SimbleeForMobile.setVisible(redText, false);
    SimbleeForMobile.updateColor(lightVal, BLACK);
    SimbleeForMobile.updateColor(tempVal, BLACK);
    SimbleeForMobile.updateColor(humVal, BLACK);
    SimbleeForMobile.updateColor(CO2Val, BLACK);
  }  
}

void whatHappens1(){
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(keyText, false);
    SimbleeForMobile.setVisible(greenText, false);
    SimbleeForMobile.setVisible(redText, false);
    SimbleeForMobile.updateColor(lightVal, BLACK);
    SimbleeForMobile.updateColor(tempVal, BLACK);
    SimbleeForMobile.updateColor(humVal, BLACK);
    SimbleeForMobile.updateColor(CO2Val, BLACK);    
  }  
}

void whatHappens2(){
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(keyText, true);
    SimbleeForMobile.setVisible(greenText, true);
    SimbleeForMobile.setVisible(redText, true);

    if (lightCheck <= 5000){
      SimbleeForMobile.updateColor(lightVal, RED);
    }
    else if (lightCheck > 5000){
      SimbleeForMobile.updateColor(lightVal, GREEN);
    }

    if (tempCheck <= 10 || tempCheck >= 40){
      SimbleeForMobile.updateColor(tempVal, RED);
    }
    else if (tempCheck > 10 || tempCheck < 40){
      SimbleeForMobile.updateColor(tempVal, GREEN);
    }

    if (humCheck <= 10 || humCheck >= 70){
      SimbleeForMobile.updateColor(humVal, RED);
    }
    else if (humCheck > 10 || humCheck < 70){
      SimbleeForMobile.updateColor(humVal, GREEN);
    }

    if (CO2Check <= 400 || CO2Check >= 1500){
      SimbleeForMobile.updateColor(CO2Val, RED);
    }
    else if (CO2Check > 400 || CO2Check < 1500){
      SimbleeForMobile.updateColor(CO2Val, GREEN);
    }
  }
}

void whatHappens3(){
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(keyText, true);
    SimbleeForMobile.setVisible(greenText, true);
    SimbleeForMobile.setVisible(redText, true);

    if (lightCheck >= 3000){
      SimbleeForMobile.updateColor(lightVal, RED);
    }
    else if (lightCheck < 3000){
      SimbleeForMobile.updateColor(lightVal, GREEN);
    }
    
    if (tempCheck <= 7 || tempCheck >= 30){
      SimbleeForMobile.updateColor(tempVal, RED);
    }
    else if (tempCheck > 7 || tempCheck < 30){
      SimbleeForMobile.updateColor(tempVal, GREEN);
    }

    if (humCheck <= 50 || humCheck >= 70){
      SimbleeForMobile.updateColor(humVal, RED);
    }
    else if (humCheck > 50 || humCheck < 70){
      SimbleeForMobile.updateColor(humVal, GREEN);
    }

    if (CO2Check <= 200 || CO2Check >= 1200){
      SimbleeForMobile.updateColor(CO2Val, RED);
    }
    else if (CO2Check > 200 || CO2Check < 1200){
      SimbleeForMobile.updateColor(CO2Val, GREEN);
    }        
  }  
}

void whatHappens4(){
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(keyText, true);
    SimbleeForMobile.setVisible(greenText, true);
    SimbleeForMobile.setVisible(redText, true);

    if (lightCheck <= 5000){
      SimbleeForMobile.updateColor(lightVal, RED);
    }
    else if (lightCheck > 5000){
      SimbleeForMobile.updateColor(lightVal, GREEN);
    }

    if (tempCheck <= 13 || tempCheck >= 18){
      SimbleeForMobile.updateColor(tempVal, RED);
    }
    else if (tempCheck > 13 || tempCheck < 18){
      SimbleeForMobile.updateColor(tempVal, GREEN);
    }

    if (humCheck <= 65 || humCheck >= 90){
      SimbleeForMobile.updateColor(humVal, RED);
    }
    else if (humCheck > 65 || humCheck < 90){
      SimbleeForMobile.updateColor(humVal, GREEN);
    }

    if (CO2Check <= 300 || CO2Check >= 1750){
      SimbleeForMobile.updateColor(CO2Val, RED);
    }
    else if (CO2Check > 300 || CO2Check < 1750){
      SimbleeForMobile.updateColor(CO2Val, GREEN);
    }        
  }  
}

void whatHappens5(){
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(keyText, true);
    SimbleeForMobile.setVisible(greenText, true);
    SimbleeForMobile.setVisible(redText, true);

    if (lightCheck >= 3000){
      SimbleeForMobile.updateColor(lightVal, RED);
    }
    else if (lightCheck < 3000){
      SimbleeForMobile.updateColor(lightVal, GREEN);
    }

    if (tempCheck <= 20 || tempCheck >= 27){
      SimbleeForMobile.updateColor(tempVal, RED);
    }
    else if (tempCheck > 20 || tempCheck < 27){
      SimbleeForMobile.updateColor(tempVal, GREEN);
    }

    if (humCheck <= 70 || humCheck >= 90){
      SimbleeForMobile.updateColor(humVal, RED);
    }
    else if (humCheck > 70 || humCheck < 90){
      SimbleeForMobile.updateColor(humVal, GREEN);
    }

    if (CO2Check <= 400 || CO2Check >= 1500){
      SimbleeForMobile.updateColor(CO2Val, RED);
    }
    else if (CO2Check > 400 || CO2Check < 1500){
      SimbleeForMobile.updateColor(CO2Val, GREEN);
    }            
  }  
}

void whatHappens6(){
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(keyText, true);
    SimbleeForMobile.setVisible(greenText, true);
    SimbleeForMobile.setVisible(redText, true);

    if (lightCheck <= 5500){
      SimbleeForMobile.updateColor(lightVal, RED);
    }
    else if (lightCheck > 5500){
      SimbleeForMobile.updateColor(lightVal, GREEN);
    }

    if (tempCheck <= 7 || tempCheck >= 30){
      SimbleeForMobile.updateColor(tempVal, RED);
    }
    else if (tempCheck > 7 || tempCheck < 30){
      SimbleeForMobile.updateColor(tempVal, GREEN);
    }

    if (humCheck <= 10 || humCheck >= 60){
      SimbleeForMobile.updateColor(humVal, RED);
    }
    else if (humCheck > 10 || humCheck < 60){
      SimbleeForMobile.updateColor(humVal, GREEN);
    }

    if (CO2Check <= 400 || CO2Check >= 1500){
      SimbleeForMobile.updateColor(CO2Val, RED);
    }
    else if (CO2Check > 400 || CO2Check < 1500){
      SimbleeForMobile.updateColor(CO2Val, GREEN);
    }            
  }  
}

void whatHappens7(){
  if (SimbleeForMobile.updatable){
    SimbleeForMobile.setVisible(keyText, true);
    SimbleeForMobile.setVisible(greenText, true);
    SimbleeForMobile.setVisible(redText, true);

    if (lightCheck >= 3500 || lightCheck <= 2500){
      SimbleeForMobile.updateColor(lightVal, RED);
    }
    else if (lightCheck < 3500 || lightCheck > 2500){
      SimbleeForMobile.updateColor(lightVal, GREEN);
    }

    if (tempCheck <= 21 || tempCheck >= 29){
      SimbleeForMobile.updateColor(tempVal, RED);
    }
    else if (tempCheck > 21 || tempCheck < 29){
      SimbleeForMobile.updateColor(tempVal, GREEN);
    }

    if (humCheck <= 70 || humCheck >= 90){
      SimbleeForMobile.updateColor(humVal, RED);
    }
    else if (humCheck > 70 || humCheck < 90){
      SimbleeForMobile.updateColor(humVal, GREEN);
    }

    if (CO2Check <= 400 || CO2Check >= 1500){
      SimbleeForMobile.updateColor(CO2Val, RED);
    }
    else if (CO2Check > 400 || CO2Check < 1500){
      SimbleeForMobile.updateColor(CO2Val, GREEN);
    }        
  }  
}

void SimbleeCloud_onReceive(unsigned int originESN, const unsigned char *payload, int len){ 
}

void SimbleeCOM_onReceive(unsigned int esn, const char* payload, int len, int rssi) {
  //First read Light pointer '1' before storing sensor values into valueA
  if (payload[0] == '1'){
  valueA[0] = payload[1];
  valueA[1] = payload[2];
  valueA[2] = payload[3];
  valueA[3] = payload[4];
  }
  
  //First read ECO2 pointer '4' before storing sensor values into valueD
  if (payload[0] == '4'){
    valueD[0] = payload[1];
    valueD[1] = payload[2];
    valueD[2] = payload[3];
    valueD[3] = payload[4];
  }
}

//Allows code to read the decimal values when converting a float into a character array
long getDecimal(float val){
  int intPart = int(val);
  long decPart = 100*(val-intPart);

   if(decPart>0)return(decPart);
   else if(decPart<0)return((-1)*decPart);
   else if(decPart=0)return(00);
}

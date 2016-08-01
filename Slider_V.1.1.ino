////
////
//// Stepper Slider with two directions buttons, one blue and one green
//// Stepper driver is an easydriver 
//// Made by Haroun Hajem
////




//#include <SoftI2CMaster.h>
#include <SI2CIO.h>
#include <LiquidCrystal_SR3W.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR1W.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SI2C.h>
#include <LiquidCrystal.h>
#include <LCD.h>
#include <I2CIO.h>
#include <FastIO.h>
#include <LiquidCrystal_I2C_ByVac.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <MultiStepper.h>
#include <AccelStepper.h>


#pragma region Pin Declaration
#define btn_Blue 7
#define btn_Green 8
//Buttons
#define btnBlue A12 
#define btnYellow A13
#define btnBlack A14
#define btnGreen A15
#define btnCalibrationStop A8
#define btnJoy A0

//Joystick
#define joyX A10
#define joyY A9



//Stepper Motor
#define stepPin 6
#define dirPin 7
#define enablePin 2
AccelStepper stepper(1, stepPin, dirPin);
#pragma endregion
#pragma region LCD Declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

#pragma endregion
#pragma region Global Variables Declaration
short int _stepPosition = 1;
long maxStepPosition = 50000;
short int minStepPosition = 0;
String lastMessage = "Last";
long fastSpeed = 50000.0;
int valueJoyX, valueJoyY;
short const
buffer = 350;
short const
origoX = 499,
origoY = 497;
const short joyMenuDelay = 800;
short const accSpeed = 50;
long timerJoy;
long timerAcc;
long timerMov;
long timerReading;
int accelerationValue = 900;
int movement = 100;
short speedValue = 500;
#pragma endregion

void setup()
{
	Serial.begin(115200);


	//pinMode()
	pinMode(btnBlue, INPUT);
	pinMode(btnYellow, INPUT);
	pinMode(btnBlack, INPUT);
	pinMode(btnGreen, INPUT);
	pinMode(btnCalibrationStop, INPUT);
	pinMode(joyX, INPUT);
	pinMode(joyY, INPUT);
	pinMode(enablePin, OUTPUT);

	//Stepper Setup
	stepper.setMaxSpeed(2000);
	stepper.setAcceleration(10000);
	stepper.setEnablePin(enablePin);
	//Turn off stepper
	digitalWrite(enablePin, HIGH);
	//LCD declaration
	lcd.begin(20, 4);

}

long timerTimeLapseMove2;
short movementDelay2 = 1500;
void loop()
{
	//ManualDrive();
	//IntroWelcomeSequance();
	//CallibrationSequance();
	MenuMain();
	//TimeLapseBegin();



	/*if (!stepper.isRunning())
	{
		digitalWrite(enablePin, HIGH);
	}


	if (millis() - timerTimeLapseMove2 > movementDelay2 &&
		!stepper.isRunning() && SmoothingBtnJoy() == 0)
	{
		stepper.moveTo(stepper.currentPosition() + 25);
		timerTimeLapseMove2 = millis();
		digitalWrite(enablePin, LOW);

	}
	stepper.run();*/






}


// Intro & Calibration
void IntroWelcomeSequance()
{
	//Print to LCD

	lcd.setCursor(2, 1);

	lcd.print("VIDEO SLIDER PRO");
	lcd.setCursor(3, 2);
	lcd.print("By Haron Hajem");
	delay(1900);

	lcd.clear();
	lcd.home();
	lcd.setCursor(3, 1);
	lcd.print("Begin calibrate");
	lcd.setCursor(4, 2);
	lcd.print("Press any key");
	bool btnResponse = false;

	do
	{
		if (digitalRead(btnBlue) ||
			digitalRead(btnYellow) ||
			digitalRead(btnBlack) ||
			digitalRead(btnGreen) || SmoothingBtnJoy() == 0)
		{
			btnResponse = true;
		}
	} while (!btnResponse);


	lcd.clear();
	lcd.setCursor(0, 2);
	lcd.print("Don't  obstruct  the");
	lcd.setCursor(0, 3);
	lcd.print("rails while  running");
	for (size_t i = 0; i < 10; i++)
	{
		lcd.setCursor(6, 0);
		lcd.print("WARNING!");
		delay(500);
		lcd.setCursor(6, 0);
		lcd.print("         ");
		delay(500);

	}


	lcd.clear();

}
void CallibrationSequance()
{
	lcd.clear();
	lcd.setCursor(4, 1);
	lcd.print("Calibrating");
	lcd.blink();

	// !!!
	stepper.setAcceleration(2500);
	stepper.setSpeed(500);
	digitalWrite(enablePin, LOW);

	// !!!
	stepper.moveTo(stepper.currentPosition() - 50000);
	while (!digitalRead(btnCalibrationStop)) {
		/*	if (stepper.currentPosition()  == 50000)
		{
		stepper.moveTo()
		}
		stepper.run();*/

		//TODO Utveckla en smooth stepper calibration movement.
	}


	MoveFromCalibrationButton();
	stepper.setCurrentPosition(0);
	lcd.setCursor(7, 2);
	lcd.noBlink();
	lcd.print("Done");

	// !!!
	stepper.setAcceleration(10000);
	digitalWrite(enablePin, HIGH);


	delay(5000);
	lcd.clear();

}
void MoveFromCalibrationButton() {
	const short calibrationBufferDistance = 100;
	stepper.setAcceleration(8000);
	digitalWrite(enablePin, LOW);
	stepper.moveTo(stepper.currentPosition() + calibrationBufferDistance);
	do
	{
		stepper.run();
	} while (!digitalRead(btnCalibrationStop));
	digitalWrite(enablePin, HIGH);

}


// Menu
#pragma region Menu var

short cursorPos = 0;
short cursorLastPos = cursorPos;
const short btnPause = 200;
#pragma endregion
void MenuMain()
{
	PrintMenuHome();
	cursorPos = 0;

	switch (NavigateReadJoystick(4))
	{
	case 0:
		//Manual Drive
		cursorPos = 0;
		PrintMenuDrive();
		ManualDrive();
		break;
	case 1:
		//Auto Drive
		cursorPos = 0;
		MenuAutoDrive();
		////Reset values
		//cursorPos = 0;
		break;
	case 2:
		//TimeLapse
		MenuTimeLapse();
		////Reset values
		//cursorPos = 0;
		break;
	case 3:
		break;
	case 10:
		MenuMain();
		break;
	default:
		break;
	}
	delay(800);
	MenuMain();

	//Menu print Menu
	//1. Auto Drive
	//1.1 Drive
	//1.2 Select position
	//2. Manual Drive
	//3. Setting
	//3.1 ReCalibrate
	//3.2 SetTime? Acceleration
}


//Auto Drive
#pragma region MenuAutoVar
short posA = 0, posB = 0;
long timerDisplay;
int lastPos;
int lastSpeedValue = 0;
#pragma endregion
void MenuAutoDrive()
{
	PrintMenuAutoDrive();
	cursorPos = 0;
	switch (NavigateReadJoystick(4))
	{
	case 0:
		//	Drive
		AutoDriveComence();
		break;

	case 1:
		// Set positions
		AutoDriveSetPosition();
		break;

	case 2:
		// Set acceleration speed
		AutoDriveSetSpeed();
		break;
	case 10:
		//Return to MainManu
		MenuMain();
		break;
	default:
		break;
	};
	delay(750);
	MenuAutoDrive();
}
void AutoDriveSetPosition()
{
	lcd.clear();
	lcd.noBlink();
	PrintPosition("A", 0);
	lcd.print(stepper.currentPosition());

	stepper.setAcceleration(10000);
	stepper.setMaxSpeed(10000);
	do
	{
		if (!stepper.isRunning()

			&& lastPos != stepper.currentPosition()
			) //millis() - timerDisplay > 600UL
		{
			PrintPosition("A", 0);

			/*int currPos = ;*/
			lastPos = stepper.currentPosition();
			lcd.print(lastPos);
			timerDisplay = millis();
		}
		valueJoyX = analogRead(joyX);
		valueJoyY = analogRead(joyY);
		Reading_Movement();
		StepperRun();
		posA = stepper.currentPosition();
	} while (SmoothingBtnJoy() != 0);

	delay(600);
	PrintPosition("B", 1);
	lcd.print(stepper.currentPosition());
	do
	{
		if (!stepper.isRunning() && lastPos != stepper.currentPosition())
		{
			PrintPosition("B", 1);
			lcd.print(stepper.currentPosition());
			lastPos = stepper.currentPosition();
		}
		valueJoyX = analogRead(joyX);
		valueJoyY = analogRead(joyY);
		Reading_Movement();
		StepperRun();
		posB = stepper.currentPosition();
	} while (SmoothingBtnJoy() != 0);
	delay(600);
}
void AutoDriveSetSpeed()
{
	lcd.clear();
	lcd.noBlink();
	PrintSpeedValue();
	AutoDriveCheckAllowedLimitAcceleration();
	do
	{
		if (speedValue != lastSpeedValue &&
			millis() - timerDisplay > 150UL)
		{
			PrintSpeedValue();
			lastSpeedValue = speedValue;
			timerDisplay = millis();
		}
		valueJoyX = analogRead(joyX);
		valueJoyY = analogRead(joyY);
		ReadingAndSettingSpeedValue(100, 100, 3600, 25);


	} while (SmoothingBtnJoy() != 0);

}
void AutoDriveToPositionX(int position, int maxSpeed) {
	stepper.setAcceleration(900);
	stepper.setMaxSpeed(maxSpeed);

	// Move to position
	digitalWrite(enablePin, LOW);
	if (stepper.currentPosition() != position)
	{
		//stepper.moveTo(posA);
		stepper.moveTo(position);
	}
	do
	{
		stepper.run();
	} while (stepper.currentPosition() != position);
	digitalWrite(enablePin, HIGH);
}
void AutoDriveComence()
{
	if ((posA == 0 && posB == 0) ||
		(posA == posB))
	{
		//Check if value is correct
		lcd.clear();
		lcd.noBlink();
		lcd.setCursor(0, 1);
		lcd.print("Please set  position");
		delay(2000);
		MenuAutoDrive();
	}
	else
	{

		//Drive
		//Print Auto Drive
		lcd.clear();
		lcd.noBlink();
		lcd.setCursor(0, 1);
		lcd.print("    Auto Drive");
		AutoDriveToPositionX(posA, 2000);
		delay(1500);
		AutoDriveToPositionX(posB, speedValue);
		delay(2500);
		AutoDriveToPositionX(posA, 2000);

	 }
}
void AutoDriveCheckAllowedLimitAcceleration() {
	if (accelerationValue > 900)
	{
		accelerationValue = 900;
	}
}
void ReadingAndSettingSpeedValue(int intervalTimer, int minSpeed, int maxSpeed, int addValue)
{
	// delay for reading 
	if (millis() - timerReading <= intervalTimer) {
		return;
	}

	// Setting value ++
	if (valueJoyY < origoY - buffer)
	{
		if (speedValue <
			minSpeed)
		{
			speedValue = minSpeed;
			return;
		}
		speedValue -= addValue;
		timerReading = millis();
	}
	// Setting value --
	else if (valueJoyY > origoY + buffer)
	{
		if (speedValue > maxSpeed)
		{
			speedValue = maxSpeed;
			return;
		}
		speedValue += addValue;
		timerReading = millis();
	}


}



//Print Menus
void PrintMenuAutoDrive()
{
	lcd.clear();
	lcd.blink();
	lcd.setCursor(0, 0);
	lcd.print("Auto Drive      [ ]");
	lcd.setCursor(0, 1);
	lcd.print("Set positions   [ ]");
	lcd.setCursor(0, 2);
	lcd.print("Set speed       [ ]");
	lcd.setCursor(0, 3);
	lcd.print("Exit            [ ]");

	lcd.setCursor(17, 0);


}
void PrintMenuTimeLapse()
{
	lcd.clear();
	lcd.blink();
	lcd.setCursor(0, 0);
	lcd.print("Begin           [ ]");
	lcd.setCursor(0, 1);
	lcd.print("Set timer       [ ]");
	lcd.setCursor(0, 2);
	lcd.print("Exit            [ ]");
	lcd.setCursor(17, 0);

}
void PrintMenuSettings() {}
void PrintMenuDrive()
{
	lcd.clear();
	lcd.noBlink();
	lcd.setCursor(1, 1);
	lcd.print("MANUAL DRIVE MODE");
	lcd.setCursor(1, 2);
	lcd.print("press key to exit");
}
void PrintMenuHome()
{
	lcd.clear();
	lcd.blink();
	lcd.setCursor(0, 0);
	lcd.print("Manual Drive    [ ]");
	lcd.setCursor(0, 1);
	lcd.print("Auto Drive      [ ]");
	lcd.setCursor(0, 2);
	lcd.print("Timelapse       [ ]");
	lcd.setCursor(0, 3);
	lcd.print("Settings        [ ]");
	lcd.setCursor(17, 0);

}
void PrintPosition(String posChar, int lcdPrintRowNr) {

	lcd.setCursor(0, lcdPrintRowNr);
	lcd.print("                ");
	lcd.setCursor(0, lcdPrintRowNr);
	lcd.print("Position " + posChar + ": ");
}
void PrintSpeedValue() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Speed value is :");
	lcd.setCursor(0, 1);
	lcd.print(speedValue);
}



//Navigation mechanism
#pragma region Smooting Var
const int numReadings = 10;		// the number of readings
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int val;
long timerBtnJoy;
#pragma endregion
int SmoothingBtnJoy() {
	if (millis() - timerBtnJoy > 1UL)
	{

		val = analogRead(A0);


		////////////////////////////////////////
		total = total - readings[readIndex];
		// read from the sensor:
		readings[readIndex] = analogRead(A0);
		// add the reading to the total:
		total = total + readings[readIndex];
		// advance to the next position in the array:
		readIndex = readIndex + 1;

		// if we're at the end of the array...
		if (readIndex >= numReadings) {
			// ...wrap around to the beginning:
			readIndex = 0;
		}

		// calculate the average:
		// send it to the computer as ascii digits

		//delay(1);        // delay in between reads for stability
		timerBtnJoy = millis();
		return average = total / numReadings;
	}
	else
	{
		return 500;
	}






}
int ButtonReadNavigate(int menu_length)
{

	//Go back
	if (digitalRead(btnBlue))
	{
		delay(250);
		return 10;
	}

	//Selection is made
	if (digitalRead(btnGreen))
	{

		delay(250);
		return cursorPos;
	}


	//Move up
	if (digitalRead(btnYellow))
	{
		if (cursorPos - 1 >= 0)
		{
			cursorPos -= 1;

		}

	}


	//Move down
	if (digitalRead(btnBlack))
	{
		if (cursorPos + 1 < menu_length)
		{
			cursorPos += 1;

		}

	}

	//Print out new selection on LCD
	if (cursorPos != cursorLastPos)
	{
		lcd.setCursor(17, cursorPos);
		cursorLastPos = cursorPos;
		delay(btnPause);
	}

	ButtonReadNavigate(menu_length);
}
int NavigateReadJoystick(int menu_length)
{

	//Go back
	//if (digitalRead(btnBlue))
	//{
	//	delay(250);
	//	//Reset values
	//	cursorPos = 0;
	//	return 10;
	//}

	//Selection is made
	if (digitalRead(btnGreen) || SmoothingBtnJoy() == 0)
	{

		delay(250);
		if (cursorPos + 1 == menu_length)
		{
			return 10;
		}
		return cursorPos;
	}
	valueJoyY = analogRead(joyY);


	//Move up
	if (valueJoyY < origoY - buffer)
	{
		if (cursorPos - 1 > -1 &&
			millis() - timerJoy >= joyMenuDelay)
		{
			cursorPos -= 1;
			timerJoy = millis();
		}
	}
	else if (valueJoyY > origoY + buffer)
	{
		//Move down
		if (cursorPos + 1 < menu_length &&
			millis() - timerJoy >= joyMenuDelay)
		{
			cursorPos += 1;
			timerJoy = millis();
		}
	}

	//Print out new selection on LCD
	if (cursorPos != cursorLastPos)
	{
		lcd.setCursor(17, cursorPos);
		cursorLastPos = cursorPos;
		delay(btnPause);
	}

	NavigateReadJoystick(menu_length);
}



// Manual Drive
void ManualDrive()
{
	valueJoyX = analogRead(joyX);
	//XXX
	valueJoyY = analogRead(joyY);


	Reading_Acceleration(2, 1000, 11000, 50);
	Reading_Movement();
	StepperRun();


	//!digitalRead(btnJoy)
	if (digitalRead(btnBlue) ||
		SmoothingBtnJoy() == 0)
	{
		return;
	}
	ManualDrive();
}
void Reading_Acceleration(int timer, int minAccu, int maxAccu, int addValue)
{

	if (millis() - timerAcc <= timer) {
		return;
	}

	if (valueJoyY < origoY - buffer)
	{
		if (accelerationValue <
			minAccu)
		{
			accelerationValue = minAccu;
			return;
		}
		accelerationValue -= addValue;
		stepper.setAcceleration(accelerationValue);
		timerAcc = millis();
	}
	else if (valueJoyY > origoY + buffer)
	{
		if (accelerationValue > maxAccu)
		{
			accelerationValue = maxAccu;
			return;
		}
		accelerationValue += addValue;
		stepper.setAcceleration(accelerationValue);
		timerAcc = millis();
	}


}
//void Reading_Speed()
//{
//	if (millis() - timerAcc <= 25UL) {
//		return;
//	}
//
//	if (valueJoyY < origoY - buffer)
//	{
//		if (accelerationValue <
//			1000)
//		{
//			accelerationValue = 1000;
//			return;
//		}
//		accelerationValue -= accSpeed + 500;
//		stepper.setSpeed(accelerationValue);
//
//		timerAcc = millis();
//	}
//	else if (valueJoyY > origoY + buffer)
//	{
//		if (accelerationValue > 30000)
//		{
//			accelerationValue = 30000;
//			return;
//		}
//		accelerationValue += accSpeed + 500;
//		stepper.setSpeed(accelerationValue);
//		timerAcc = millis();
//	}
//
//
//}
void Reading_Movement()
{
	//Set stepper to MoveTo position in between MAX & MIN
	int currentPos = stepper.currentPosition();
	const int maxPos = 8050;
	const int minPos = 0;

	//Decrease value, not under min
	if (valueJoyX > origoX + buffer)
	{
		if (currentPos - movement < minPos)
		{
			return;
		}
		stepper.moveTo(currentPos - movement);
	}

	//Increase value, not over max
	if (valueJoyX < origoX - buffer)
	{
		if (currentPos + movement > maxPos)
		{
			return;
		}

		stepper.moveTo(currentPos + movement);


	}

}
void StepperRun()
{
	if (
		!stepper.isRunning()) //millis() - timer1 > 150UL && 
	{

		digitalWrite(enablePin, HIGH);
	}
	else
	{
		digitalWrite(enablePin, LOW);

	}
	stepper.run();
}


#pragma region TimeLapse VAR
long timeLapseTime, lasttimeLapseTime, timerAnalogRead;

#pragma endregion

// Time Lapse
void MenuTimeLapse()
{
	PrintMenuTimeLapse();
	cursorPos = 0;
	switch (NavigateReadJoystick(3))
	{
	case 0:
		//Begin Time Lapse 
		TimeLapseBegin();
		break;
	case 1:
		//Set timer
		TimeLapseSetTimer();
		break;
	case 10:
		MenuMain();
		break;
	default:
		break;
	}
	MenuTimeLapse();
}

void TimeLapseBegin()
{
	//if (timeLapseTime == 0)		//Is timer set?
	//{
	//	lcd.clear();
	//	lcd.noBlink();
	//	lcd.setCursor(2, 1);
	//	lcd.print("Please set timer");
	//	delay(2000);
	//	return;
	//}


	// Time Lapse sequance begin
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Begin");

	//stepper.setSpeed(30000);
	digitalWrite(enablePin, LOW);
	stepper.setAcceleration(900);
	//stepper.setCurrentPosition(8050);
	stepper.setMaxSpeed(1600);
	//Move to start position, before begining
	if (stepper.currentPosition() > 0)
	{
		stepper.moveTo(0);
		do
		{
			stepper.run();
		} while (stepper.currentPosition() != 0);
	}

	//stepper.moveTo(5000);
	const long movementDelay = 186.3354037266965 * 2;



	//////
	//////
	////// Problem - Den missar vissa steps. Kan bero på att biten är för kort
	////// Accelerationen för låg, eller att hastigheten är för låg. 
	//////


	long timerTimeLapseMove;
	do
	{
		//stepper.runSpeedToPosition()
		//stepper.run();
		//delay(1000);

		if (millis() - timerTimeLapseMove > movementDelay)  //   && !stepper.isRunning()

		{
			stepper.moveTo(stepper.currentPosition() + 25);
			timerTimeLapseMove = millis();
			digitalWrite(enablePin, LOW);

		}


		stepper.run();
		if (!stepper.isRunning())
		{
			digitalWrite(enablePin, HIGH);
		}

		//if (millis() - timerStepPin > 500UL)
		//{
		//	digitalWrite(stepPin, HIGH);
		//}
		//else
		//{
		//	digitalWrite(stepPin, LOW);
		//}

	} while (stepper.currentPosition() != 8050); //stepper.currentPosition() != 5000
	digitalWrite(enablePin, HIGH);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Done");

	delay(5000);

}


// Convert microseconds to Min, Hour, Days
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  


void TimeLapseSetTimer()
{
	PrintTimeLapseSetTimer();
	lasttimeLapseTime = timeLapseTime + 1;
	int inputJoyX;
	do
	{

		//Read Joystick and change value
		inputJoyX = analogRead(joyX);
		if (millis() - timerAnalogRead > 150UL)
		{
			if (inputJoyX > origoX + buffer)
			{
				timeLapseTime += 60000;
			}

			if (inputJoyX < origoX - buffer)
			{
				if (timeLapseTime - 60000 > -1)
				{
					timeLapseTime -= 60000;
				}
			}
			timerAnalogRead = millis();
		}

		//Update Display
		if (timeLapseTime != lasttimeLapseTime &&
			millis() - timerDisplay > 500UL)
		{
			lcd.setCursor(7, 2);
			lcd.print("                ");
			lcd.setCursor(7, 2);

			/*long minutes = ((timeLapseTime / (1000 * 60)) % 60);
			long hours = ((timeLapseTime / (1000 * 60 * 60)) % 24);*/


			lcd.print(numberOfHours(timeLapseTime / 1000));
			lcd.print("H ");
			lcd.print(numberOfMinutes(timeLapseTime / 1000));
			lcd.print("M");
			lasttimeLapseTime = timeLapseTime;
			timerDisplay = millis();

		}




	} while (SmoothingBtnJoy() != 0);
	delay(500);

}

void PrintTimeLapseSetTimer()
{
	lcd.clear();
	lcd.noBlink();
	lcd.setCursor(3, 1);
	lcd.print("Set Time Lapse");


}


//TODO Testa ifall motorn har en stillastående statisk metod? 
//TODO Testa att göra sända egna signaler till steppern, med millis() timer för att sända till step kanalen.
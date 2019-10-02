/*
 * NOTAS
 * falta fazer a implementa��o de SetPoint (quando ligar e desligar as coisas, conforme o Kp)
 * falta construir na protoboard
 * falta fazer a documenta��o
 * @autor - Samuel Antunes Vieira
*/

//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>

//Define os pinos que ser�o utilizados para liga��o ao display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Defini��o de valores de PWM m�ximos e de incremento
#define MAXresPWM 153 //máximo aquecedor
#define percHeat 1.53 //porcentagem aquecedor
#define quartHeat 38.25 //quartil aquecedor
#define MAXfanPWN 255 //máximo ventilador
#define quartFan 63.75 //quartil ventilador
#define percFan 2.55 //porcentagem ventilador

//mensagens constantes da interface
const char on[] = "ON";
const char off[] = "OFF";
const char enabled[] = "Enabled";
const char disabled[] = "Disabled";
const char sp[] = "SP:";
const char KP[] = "KP:";
const char manual[] = "  Manual Mode";
const char fan[] = "Fan:";
const char heat[] = "Heat:";
const char pv[] = "PV:";
const char driver[] = "  Driver Status";
const char npg[] = "New Prop. Gain";
const char nsp[] = "New Set Point";
const char stat1h[] = "C   Heat";
const char stat1f[] = "C    Fan";
const char stat1n[] = "C";
const char stat2[] = "PV:     C";
const char percent[] = "%";

//define pinos e vari�veis globais
const int btnUp = 7; //+ button pin
const int btnDown = 8; //- button pin
const int btnSet = 9;  //S button pin
const int btnP = 10; //P button pin
const int PwLED = 0; //led sinalizador Enable
const int DriveEN = 6; //Driver On/Off pin
//const int pinFet = 1; //OVEN on/off

const int period = 500;
unsigned long time_now = 0;

int SensorPin = A0; //analog lm35/OVEN
int FanPin = A1; //analog ventilador
int HeatPin = A2; //analog aquecedor

float HeatVal = 0; //valor de temperatura atual

// vari�veis de ganho manual
int pFAN = 100;
int pHEAT = 100;
float vFAN = 255;
float vHEAT = 153;

float setPoint = 30.0; // SetPoint c/ valor inicial
float tempSP = setPoint; //vari�vel temp para ajuste de setpoint

float Kp = 1.0; //ganho, iniciado como 1
float tempKP = Kp; //vari�vel temp para ajuste de ganho

// bools para flags
bool onoff = 1;
bool toogleled = 0;
bool heaton = 0;
bool fanon = 0;

typedef enum
{
	menu = 1,
	spconf,
	KPconf,
	dvrconf,
	manHeat,
	manFan
}Estados;

int StateAtual = menu;
//dps fazer switch case de StateAtual no loop!

void setup()
{
  //define entradas e sa�das
  pinMode(btnUp, INPUT);
  pinMode(btnDown, INPUT);
  pinMode(btnSet, INPUT);
  pinMode(btnP, INPUT);
  pinMode(DriveEN, OUTPUT);
  //pinMode(pinFet, OUTPUT);
  pinMode(PwLED, OUTPUT);
  digitalWrite(DriveEN, HIGH);
  //digitalWrite(pinFet, HIGH);
  digitalWrite(PwLED, HIGH);
  //Serial.begin(9600);

  //Define o n�mero de colunas e linhas do LCD
  lcd.begin(16, 2);
}

void loop()
{
  //realiza verificação a cada 500ms
  if(millis() >= (time_now + period))
  {
    //se onoff=1, driver ligado: blink led 2Hz
    if(toogleled==1 && onoff==1)
    {
      digitalWrite(PwLED, HIGH);
      toogleled = 0;
    }else if(toogleled==0 && onoff==1)
    {
      digitalWrite(PwLED, LOW);
      toogleled = 1;
    }else{
      digitalWrite(PwLED, HIGH);
    }
    time_now = millis();
  }

  //faz leitura sensor, convertendo dados para temperatura
  HeatVal = (float(analogRead(SensorPin))*5/(1023))/0.01;

  //se não estiver em modo manual, configura ventilador e aquecedor conforme ganho e setPoint
  if(StateAtual != manHeat && StateAtual != manFan)
  {
    if(HeatVal < setPoint)
    {
      //Está frio! liga aquecedor e desliga ventilador...
      analogWrite(HeatPin, (Kp*quartHeat));
      analogWrite(FanPin, 0);
      heaton = 1;
      fanon = 0;
    }else if(HeatVal > setPoint)
    {
      //Está quente! liga ventilador e desliga aquecedor...
      analogWrite(FanPin, (Kp*quartFan));
      analogWrite(HeatPin, 0);
      heaton = 0;
      fanon = 1;
    }else
    {
      //temperatura ideal, desliga componentes
      analogWrite(FanPin, 0);
      analogWrite(HeatPin, 0);
      heaton = 0;
      fanon = 0;
    }
  }

  //INTERFACE - troca de estados
  switch(StateAtual)
  {
  	  case menu:
  		  showstatus();
	  break;

  	  case spconf:
  		  SPconfig();
  		  if(digitalRead(btnUp)) tempSP++;
  		  if(digitalRead(btnDown)) tempSP--;
  		  if(digitalRead(btnSet)) setPoint = tempSP;
	  break;

  	  case KPconf:
  		  KPconfig();
  		  if(digitalRead(btnUp))
  		  {
  			  tempKP++;
         //variação de 25% de ganho
  			  if(tempKP > 4) tempKP = 4;
  		  }
		  if(digitalRead(btnDown))
		  {
			  tempKP--;
			  if(tempKP < 1) tempKP = 1;
		  }
		  if(digitalRead(btnSet))
		  {
			  Kp = tempKP;
		  }
	  break;

  	  case dvrconf:
  		  DVRconfig();
  		  if(digitalRead(btnUp) || digitalRead(btnDown)) onoff=!onoff;
  		  if(onoff)
  		  {
  			digitalWrite(DriveEN, HIGH);
  		  }else{
  			digitalWrite(DriveEN, LOW);
  		  }
	  break;

  	  case manHeat:
  		  manualHeatConfig();
		  if(digitalRead(btnUp))
		  {
			  pHEAT++;
			  if(pHEAT > 100) pHEAT = 100;
		  }
		  if(digitalRead(btnDown))
		  {
			  pHEAT--;
			  if(pHEAT < 1) pHEAT = 1;
		  }
		  if(digitalRead(btnSet))
		  {
			  if(pHEAT==0)
			  {
				  pHEAT = 100;
			  }else
			  {
				  pHEAT = 0;
			  }
		  }
  		  vHEAT = pHEAT * percHeat;
  		  analogWrite(HeatPin, vHEAT);
	  break;

  	  case manFan:
  		  manualFanConfig();
		  if(digitalRead(btnUp))
		  {
			  pFAN++;
			  if(pFAN > 100) pFAN = 100;
		  }
		  if(digitalRead(btnDown))
		  {
			  pFAN--;
			  if(pFAN < 1) pFAN = 1;
		  }
		  if(digitalRead(btnSet))
		  {
			  if(pFAN==0)
			  {
				  pFAN=100;
			  }else{
				  pFAN=0;
			  }
		  }
  		  vFAN = pFAN * percFan;
  		  analogWrite(FanPin, vFAN);
	  break;
  }
  delay(200);	//atualiza tela a cada delay

  //ao pressionar botão, altera modo
  if(digitalRead(btnP))
  {
	  StateAtual++;
	  if(StateAtual==7) StateAtual=1;
  }

}

//SET POINT, PROCESS VALUE, HEAT
void showstatus()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(sp);
  lcd.setCursor(4, 0);
  lcd.print(setPoint);
  lcd.setCursor(0,1);
  lcd.print(stat2);
  lcd.setCursor(4, 1);
  lcd.print(HeatVal);
  if(heaton)
  {
	  lcd.setCursor(8,0);
	  lcd.print(stat1h);
	  lcd.setCursor(13, 1);
	  lcd.print(on);
  }else if(fanon)
  {
	  lcd.setCursor(8,0);
	  lcd.print(stat1f);
	  lcd.setCursor(13, 1);
	  lcd.print(on);
  }else{
	  lcd.setCursor(8,0);
	  lcd.print(stat1n);
	  lcd.setCursor(13, 1);
	  lcd.print(off);
  }
}

//tela de ajuste do Set Point
void SPconfig()
{
	lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(nsp);
    lcd.setCursor(0, 1);
    lcd.print(sp);
    lcd.setCursor(4, 1);
    lcd.print(tempSP);
}

//tela de ajuste do ganho
void KPconfig()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(npg);
	lcd.setCursor(0, 1);
	lcd.print(KP);
	lcd.setCursor(4, 1);
	lcd.print(tempKP);
}

//tela habilita/desabitlita driver
void DVRconfig()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(driver);
	lcd.setCursor(4, 1);
	if(onoff==1)
	{
		lcd.print(enabled);
	}else
	{
		lcd.print(disabled);
	}
}

void manualHeatConfig()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(manual);
	lcd.setCursor(0, 1);
	lcd.print(heat);
	lcd.setCursor(6, 1);
	if(pHEAT > 0)
	{
	  lcd.print(on);
	  lcd.setCursor(10 , 1);
	  lcd.print(pHEAT);
	  lcd.setCursor(13 , 1);
	  lcd.print(percent);
	}else
	{
	  lcd.print(off);
	}
}

void manualFanConfig()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(manual);
	lcd.setCursor(0, 1);
	lcd.print(fan);
	lcd.setCursor(5, 1);
	if(pFAN > 0)
	{
	  lcd.print(on);
	  lcd.setCursor(10 , 1);
	  lcd.print(pFAN);
	  lcd.setCursor(13 , 1);
	  lcd.print(percent);
	}else
	{
	  lcd.print(off);
	}
}

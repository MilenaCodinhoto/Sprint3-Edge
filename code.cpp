#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define trigPin 18
#define echoPin 19

long duration;
float distance1, distance2;
unsigned long time1, time2;

// Substitua 0x27 pelo endereço correto identificado pelo scanner
LiquidCrystal_PCF8574 lcd(0x27); 

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.print("Inicializando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  distance1 = getAverageDistance();
  time1 = millis();
  delay(900); // Intervalo de tempo maior entre as medições
  distance2 = getAverageDistance();
  time2 = millis();
  
  Serial.print("Distance 1: ");
  Serial.print(distance1);
  Serial.print(" cm, Time 1: ");
  Serial.print(time1);
  Serial.println(" ms");

  Serial.print("Distance 2: ");
  Serial.print(distance2);
  Serial.print(" cm, Time 2: ");
  Serial.print(time2);
  Serial.println(" ms");
  
  lcd.clear();
  lcd.setCursor(0, 0); // posicionamento
  lcd.print("D1:");
  lcd.print(distance1);
  lcd.print(" cm");

  lcd.setCursor(0, 1); // posicionamento
  lcd.print("D2:");
  lcd.print(distance2);
  lcd.print(" cm");
  
  // Verifica se as distâncias são válidas antes de calcular a velocidade
  if (distance1 >= 0 && distance2 >= 0) {
    float speed = calculateSpeed(distance1, distance2, time1, time2);
    Serial.print("Velocidade: ");
    Serial.print(speed);
    Serial.println(" cm/s");

    lcd.setCursor(8, 1); // posicionamento
    lcd.print("V:");
    lcd.print(speed);
    lcd.print(" cm/s");
  } else {
    Serial.println("Leitura inválida, não foi possível calcular a velocidade");
    lcd.setCursor(0, 1); // posicionamento
    lcd.print("Leitura Invalida");
  }
  
  delay(1000); // Intervalo de tempo para a próxima medição
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);  // Timeout ajustado para 30ms
  
  // Ignorar leituras inválidas ou fora do alcance do sensor 
  if (duration > 0 && duration < 23200) {
    return (duration * 0.034) / 2;
  } else {
    return -1.0; // Valor inválido
  }
}

float getAverageDistance() {
  float sum = 0;
  int validReadings = 0;
  int samples = 5; // Número de amostras para a média
  
  for (int i = 0; i < samples; i++) {
    float distance = getDistance();
    if (distance != -1.0) {
      sum += distance;
      validReadings++;
    }
    delay(10); // Pequeno intervalo entre as amostras
  }
  
  if (validReadings > 0) {
    return sum / validReadings;
  } else {
    return -1.0; // Retorna -1.0 para indicar que todas as leituras foram inválidas
  }
}

float calculateSpeed(float d1, float d2, unsigned long t1, unsigned long t2) {
  float deltaTime = (t2 - t1) / 1000.0; // Converter tempo para segundos
  float deltaDistance = d2 - d1;
  return deltaDistance / deltaTime;
}
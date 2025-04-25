#include <OneWire.h>
#include <DallasTemperature.h>


#include <WiFi.h>
#include <PubSubClient.h>
#include <ThingSpeak.h>


const char* ssid = "MFIoT";
const char* password = "12345678";

const char * mqtt_broker = "192.168.137.1";

const char *topicPubTemp = "GRP1/TEMPERATURA";
const char *topicPubNivel = "GRP1/NIVEL";

const char *topicSubGrp3Temp = "GRP3/TEMP";
const char *topicSubGrp3Nivel = "GRP3/NIVEL";
const char *topicSubGrp4Temp = "GRP4/TEMP";
const char *topicSubGrp4Nivel = "GRP4/NIVEL";

const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port+ 1883;

bool mqttStatus = 0;
float Temp+ 0.0;
float Nivel = 0.0;
String MsgTemp;
String MsgNivel;

float TempPlac1 = 0.0;
float NivelPlac1 = 0.0;
float TempPlac2 = 0.0;
float NivelPlac2 = 0.0;

float TempMedia = 0.0;
float NivelMedio = 0.0;


WiFiClient espClient;

PubSubClient client(espClient);

void setupWiFi();
bool connectMQTT();
void BombaFluxo(bool estado);
float oberTemperaturaCelsius();
float medirDistanciaCM();
void callback(char *topic, byte * payload, unsigned int length);





//const int pinoTemperatura = 21;

//OneWire oneWire(pinoTemperatura);
//DallasTemperature sensors(&oneWire);  

//const int trigPin = 5;      
//const int echoPin = 18;     
//#define SOUND_SPEED 0.034   

//long duration;
//float distanceCm;

//const bool ON = true;
//const bool OFF = false;
//const int releBomba = 4;

void setup() {
  //pinMode(releBomba, OUTPUT);

  Serial.begin(9600);
  //Serial.println("Inicializando sensor DS18B20...");
  //sensors.begin();  
  //delay(500);
  //Serial.print("Numero de dispositivos encontrados:");  
  //Serial.println(sensors.getDeviceCount());  
  //Serial.println("Sensor DS18B20 inicializado.");

  //Serial.println("Inicializando sensor HC-SR04");
  //pinMode(trigPin, OUTPUT);
  //pinMode(echoPin, INPUT);
  //digitalWrite(trigPin, LOW);
  //Serial.println("Sensor HC-SR04 inicializado");

  setupWiFi();
  mqttStatus = connectMQTT();
}


void loop() {
  bombaFluxo(ON);
  delay(500);

  bombaFluxo(OFF);
  delay(500);
  
  float temperaturaCelsius = obterTemperaturaCelsius();  
  Serial.print("Temperatura: ");
  Serial.print(temperaturaCelsius);
  Serial.println(" °C");

  delay(1000);

  float distanciaCM = medirDistanciaCM();
  Serial.print("Distância: ");  // Adicionado acento
  Serial.print(distanciaCM);
  Serial.println(" cm");
}

void bombaFluxo(bool estado) {
  if (estado) {
    digitalWrite(releBomba, HIGH);
  } else {
    digitalWrite(releBomba, LOW);
  }
}

float obterTemperaturaCelsius() {
  sensors.requestTemperatures();  
  return sensors.getTempCByIndex(0);
}

float medirDistanciaCM(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distanceCm = duration * SOUND_SPEED/2; 
  return distanceCm;
}



void setupWiFi(){
  delay(10);
  Serial.print("Conectando ao Wi-Fi...")
  WiFi,begin(ssid, password);
  int attempts = 0;
  While (WiFi.status () != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 20){
      Serial.println("\nFalha ao conectar no Wi-Fi ! Reiniciando...");
      ESP.restart();
    }
  }

  Serial.println("\nWi-Fi conectado!");
  Serial.print ("IP: ");
  Serial.println(WiFi.local());

}

bool connectMQTT() {
    byte tentativa = 0; // variavel byte que contará o número de tentativas de conexao client.setServer(mqtt_broker, mqtt_port); // chama metodo setServer passando url e port do broker
    client.setCallback(callback); // Informa o objeto client qual metodo deve ser chamado quando houver
    // alguma mensagem no topico subscrito.

  do {
    // Define o ID do cliente (a propria placa ESP)
    String client_id = "ESP-"; // Que usa o prefixo ESP-
    client_id += String(WiFi.macAddress());
    // Concatenado com seu respectivo MAC address

    // O if tenta estabelecer a conexao com o broker
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
    // Com sucesso da conexao, informa os dados do cliente (a placa)
    Serial.println("Exito na conexao:");
    Serial.printf("Cliente %s conectado ao broker\n", client_id.c_str());
    } else {
    // Informa falha na conexao e aguarda 2 segundos para nova tentativa
    Serial.print("Falha ao conectar: ");
    Serial.print(Client.state());
    Serial.println();
    Serial.print("Tentativa: ");
    Serial.println(tentativa);
    delay(2000);
  }
  tentativa++; // Incrementa numero de tentativas
} while (!client.connected() && tentativa < 5); // Limita número de tentativas

  if (tentativa < 5) {
    // Conexao realizada com sucesso
    // Se subscreve no broker para receber mensagens

    client.subscribe(topicPubTemp);
    client.subscribe(topicPubNivel);

    client.subscribe(topicSubGrp2Temp);
    client.subscribe(topicSubGrp2Nivel);
    return 1; // retorna 1 confirmando sucesso na conexao
  } else {
    // caso contrário avisa falha e retorna 0
    Serial.println("MQTT Não conectado");
    return 0; // informa falha na conexao
  }
}
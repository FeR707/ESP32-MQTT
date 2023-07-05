#include <WiFi.h>
#include <PubSubClient.h>

#define LED1 4
#define LDR 26
const int pinLDR = 36;

const char* ssid = "H1";
const char* pass = "20262026";

const char* mqtt_servidor = "iothall.sytes.net";
const int mqtt_puerto = 1883;
const char* mqtt_usuario = "test";
const char* mqtt_pass = "test";
const char* mqtt_topico_raiz = "UPT";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String datosRecibidos = "";

  Serial.println(topic);
  for (int i = 0; i < length; i++) {
    datosRecibidos += (char)payload[i];
  }

  Serial.println("Payload: " + datosRecibidos);

  if (datosRecibidos == "Prendeme")
    digitalWrite(LED1, HIGH);

  if (datosRecibidos == "Apagame")
    digitalWrite(LED1, LOW);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");

    if (client.connect("UPT", mqtt_usuario, mqtt_pass)) {
      Serial.println("Conectado");
      String topico = "UPT";
      String mensaje = "Conectado al broker";
      client.publish(topico.c_str(), mensaje.c_str());
      //Tópicos a suscribir.
      String topicoSuscribir = "UPT/uC";
      client.subscribe(topicoSuscribir.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" volver a intentar en 1 segundos.");
      delay(1000);
    }
  }
}

void setup() {
  pinMode(LED1, OUTPUT);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");

  client.setServer(mqtt_servidor, mqtt_puerto);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int valorLDR = analogRead(pinLDR);
  String topico = "UPT/APP";
  String mensaje = "Luz: " + (String)valorLDR;
  client.publish(topico.c_str(), mensaje.c_str());
  delay(500);

  delay(50);
}

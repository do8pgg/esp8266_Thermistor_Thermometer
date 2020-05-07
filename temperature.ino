#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> //OTA
#include <WiFiUdp.h> //OTA
#include <ArduinoOTA.h> //OTA

const char* ssid = "KISTE";
const char* password = "*****************";

int tempRes = 5000.0;
int tempPin = 0;
int tempPlus = 5;

int mint = 100;
int maxt = 0;

WiFiServer server(80);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  ArduinoOTA.begin(); //OTA
  server.begin();
  Serial.println(" ");
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("ESP8266 IP: ");
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(WiFi.localIP());
  Serial.println(" ");
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println(" ");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("Neuer Client: ");
  Serial.print(client.remoteIP());
  Serial.print(" - ");
  while (!client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  //client.flush();
  digitalWrite(LED_BUILTIN, LOW);
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  digitalWrite(LED_BUILTIN, HIGH);
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<meta http-equiv=\"refresh\" content=\"60\">");
  client.println("");

  int tempReading = analogRead(tempPin);
  double tempK = log(tempRes * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  float tempC = tempK - 273.15;
  float tempF = (tempC * 9.0) / 5; // + 32.0;
  float tempG = (tempF + tempPlus);


  if (tempG < mint) {
    mint = tempG;
  }
  if (tempG > maxt) {
    maxt = tempG;
  }
  digitalWrite(LED_BUILTIN, LOW);
  client.print("<title>ESP8266-Tiefk&uuml;hler</title>");
  client.print("<h1>");
  client.print("<center>");
  client.print("Temperatur Tiefk&uuml;hler");
  client.print("</h1>");
  client.print("</center>");
  client.print("<br />");
  client.print("<p><font size='7'>");
  client.print("Aktuelle Temperatur: ");
  client.print(tempG, 1);
  client.print("&#176;C");
  client.print("<br />");
  client.print("<br />");
  client.print("Minimale Temperatur: ");
  client.print(mint, 1);
  client.print("&#176;C");
  client.print("<br />");
  client.print("Maximale Temperatur: ");
  client.print(maxt, 1);
  client.print("&#176;C");
  client.print("<br />");
  client.print("<br />");
  client.print("Laufzeit:&nbsp; ");
  client.print(millis() / 60 / 1000, 1);
  client.print(" Minuten ");
  client.print("</font></p>");
  client.print("</html>");
  delay(2);
  Serial.println("Client getrennt");
  //  ESP.restart();
  Serial.println(" ");
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  ArduinoOTA.handle();
}

/*************************************
 * Program : ROBOT BNU 4.0
 * ***********************************/
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

String Topic;
String Payload;

const char* ssid = "wifi_name";
const char* password = "wifi_password";

#define IN_1 D3 // Maju
#define IN_2 D4 // Mundur
#define IN_3 D7 // Belok Kanan
#define IN_4 D8 // Belok Kiri

#define mqttServer "tailor.cloudmqtt.com"
#define mqttPort 13718
#define mqttUser "user"
#define mqttPassword "password"

WiFiServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);


void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  /* we got '1' -> Maju */
  if ((char)payload[0] == '1') {
    digitalWrite(IN_1, HIGH);
    delay(100);  
    digitalWrite(IN_1, LOW);
  } 
  /* we got '2' -> Kiri */
  if ((char)payload[0] == '2') {
    digitalWrite(IN_4, HIGH);   
    delay(300);  
    digitalWrite(IN_1, HIGH);
    delay(100);  
    digitalWrite(IN_4, LOW);
    digitalWrite(IN_1, LOW);
  } 
  /* we got '3' -> Kanan */
  if ((char)payload[0] == '3') {
    digitalWrite(IN_3, HIGH);   
    delay(300);  
    digitalWrite(IN_1, HIGH);
    delay(100);  
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_1, LOW);
  } 
  /* we got '4' -> Mundur */
  if ((char)payload[0] == '4') {
    digitalWrite(IN_2, HIGH);   
    delay(100);  
    digitalWrite(IN_2, LOW);
  } 
}
  
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
   
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  server.begin();
  Serial.println("Server started");
 
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/"); 

  // Connect to Server IoT (CloudMQTT)

  client.setServer(mqttServer, mqttPort);
  client.setCallback(receivedCallback);
 
  while (!client.connected()) {
    Serial.println("Connecting to CLoudMQTT...");
 
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");
 
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.subscribe("Maju");
  client.subscribe("Kiri");
  client.subscribe("Kanan");
  client.subscribe("Mundur");
}
 
void loop() { 

  client.loop();
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  } 
  
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  } 
 
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  if (request.indexOf("/IN_1on") > 0)  {
    digitalWrite(IN_1, HIGH);
    delay(100);  
    digitalWrite(IN_1, LOW);
  }
  if (request.indexOf("/IN_1off") >0)  {
    digitalWrite(IN_1, LOW);   
  }
   if (request.indexOf("/IN_2on") > 0)  {
    digitalWrite(IN_2, HIGH);   
    delay(100);  
    digitalWrite(IN_2, LOW);
  }
  if (request.indexOf("/IN_2off") >0)  {
    digitalWrite(IN_2, LOW);   
  }
   if (request.indexOf("/IN_3on") > 0)  {
    digitalWrite(IN_3, HIGH);   
    delay(300);  
    digitalWrite(IN_1, HIGH);
    delay(100);  
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_1, LOW);
  }
  if (request.indexOf("/IN_3off") >0)  {
    digitalWrite(IN_3, LOW);   
  }
   if (request.indexOf("/IN_4on") > 0)  {
    digitalWrite(IN_4, HIGH);   
    delay(300);  
    digitalWrite(IN_1, HIGH);
    delay(100);  
    digitalWrite(IN_4, LOW);
    digitalWrite(IN_1, LOW);
  }
  if (request.indexOf("/IN_4off") >0)  {
    digitalWrite(IN_4, LOW);   
  }
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
  client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
  client.println("</head>");
  client.println("<body bgcolor = \"#f7e6ec\">"); 
  client.println("<hr/><hr>");
  client.println("<h4><center> Robot BNU 4.0  </center></h4>");
  client.println("<hr/><hr>");
  client.println("<br><br>");
  client.println("<br><br>");
  client.println("<center>");
  client.println("ROBOT");
  client.println("<a href=\"/IN_1on\"\"><button>Maju </button></a>");
  client.println("<a href=\"/IN_2on\"\"><button>Mundur </button></a><br />");
  client.println("</center>"); 
  client.println("<br><br>");
  client.println("<center>"); 
  client.println("ROBOT");
  client.println("<a href=\"/IN_3on\"\"><button>Belok Kanan </button></a>");
  client.println("<a href=\"/IN_4on\"\"><button>Belok Kiri </button></a><br />");
  client.println("</center>"); 
  client.println("<br><br>");
  client.println("<center>"); 
  client.println("<table border=\"5\">");
  client.println("<tr>");
  //=====================
  if (digitalRead(IN_1))
         { 
           client.print("<td>Maju = ON</td>");        
         }
  else
          {
            client.print("<td>Maju = OFF</td>");
          }     
  client.println("<br />");
  //======================           
  if (digitalRead(IN_2))
          { 
           client.print("<td>Mundur = ON</td>");
          }
  else
          {
            client.print("<td>Mundur = OFF</td>");
          }
  client.println("</tr>");
  //=======================
  if (digitalRead(IN_3))
          { 
           client.print("<td>Belok Kanan = ON</td>");
          }
  else
          {
            client.print("<td>Belok Kanan = OFF</td>");
          }
  client.println("<br />");
  //=======================
  if (digitalRead(IN_4))
          { 
           client.print("<td>Belok Kiri = ON</td>");
          }
  else
          {
            client.print("<td>Belok Kiri = OFF</td>");
          }
  client.println("</tr>");
  //=======================

  client.println("<tr>"); 
  client.println("</table>");
  client.println("</center>");
  client.println("</html>"); 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println(""); 

}

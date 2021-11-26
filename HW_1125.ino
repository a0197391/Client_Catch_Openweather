#include <ArduinoJson.h>
#include <WiFi.h>

const char* ssid = "ASUS_X01BDA";
const char* password = "835e9500c858";
const char* host1 = "maker.ifttt.com"; //IFTTT server網址
const char* event = "Weather";  //IFTTT事件名稱
const char* apiKey = "cD6vkUnQ9LReE0JjnmvawE";  //IFTTT Applet key
const char* host2 = "api.openweathermap.org";
const char* resource = "/data/2.5/weather?id=1668341&appid=8b48222ab3b3913829db4ce7c93c7cee"; 

char jsonRead[600];

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
}

void loop() {
  char* json = Getdata();
 //Client傳送資料
  Serial.println(json);
 
  StaticJsonDocument<600> doc;
  deserializeJson(doc, json);
  
  const char* location = doc["name"];
  String weather = doc["weather"][0]["description"];
  double temp = doc["main"]["temp"];
  double humidity = doc["main"]["humidity"];  

 
  String weatherfix="";
  
  for(int i=0;i<weather.length();i++){
  if(weather[i]!=' ') weatherfix+=weather[i];
  else if(weather[i]==' ')weatherfix+="%20";
 }
 
  String value1 = weather;
  double value2 = temp-273;
  double value3 = humidity;
  
  Sendtoline(String(weatherfix),String(value2),String (value3));  //Client傳送資料
  
    Serial.print("***");
    Serial.print(location);
    Serial.println("***");
    Serial.print("Type: ");
    Serial.println(weather);
    Serial.print("Temp: ");
    Serial.print(temp - 273);
    Serial.println("C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.println("--------------------"); 
    
    delay(3000); // the OWM free plan API does NOT allow more then 60 calls per minute
}  

void Sendtoline(String value1, String value2,String value3) {
  WiFiClient client;  //建立Client物件
  const int httpPort = 80;  //預設通訊阜80
//  String url="maker.ifttt.com/trigger/{event}/with/key/cD6vkUnQ9LReE0JjnmvawE";
  String JsonString = "";  //此範例不會用到

  //Client連結Server
  if (client.connect(host1, httpPort)) {

//    Webhook API
    String url = "/trigger/Weather/with/key/cD6vkUnQ9LReE0JjnmvawE";
//    Query String
     url += "?value1=" + value1 + "&value2=" + value2+ "&value3=" + value3;
    Serial.println(url);
    //Client傳送
    client.println(String("POST ") + url + " HTTP/1.1");
    client.println(String("Host: ") + host1);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JsonString.length());
    client.println();
    client.println(JsonString);

    //等待5秒，每0.1秒偵測一次是否有接收到response資料
    int timeout = 0;
    while (!client.available() && (timeout++ <= 50)) {
      delay(100);
    }

    //如果無回應
    if (!client.available()) {
      Serial.println("No response...");
    }
    //用while迴圈一字一字讀取Response
    while (client.available()) {
      Serial.write(client.read());
    }

    //停止Client
    client.stop();
  }
}


char* Getdata(){
  WiFiClient client;  //建立Client物件
  const int httpPort = 80;  //預設通訊阜80
  String JsonString = "";  //此範例不會用到

  //Client連結Server
  if (client.connect(host2, httpPort)) {

    //Client傳送
    client.println(String("POST ") + resource + " HTTP/1.1");
    client.println(String("Host: ") + host2); 
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JsonString.length());
    client.println();
    client.println(JsonString);

    //等待5秒，每0.1秒偵測一次是否有接收到response資料  
    int timeout = 0;
    while(!client.available() && (timeout++ <= 50)){
      delay(100);
    }
  
     //如果無回應
    if(!client.available()) {
      Serial.println("No response...");
    }

    //Checking for the end of HTTP stream
    while(!client.find("\r\n\r\n")){
      // wait for finishing header stream reading ...
    }

    //讀取資料並儲存在jsonRead中
    client.readBytes(jsonRead, 600);

    //停止Client
    client.stop(); 

    //回傳
    return jsonRead;
  } 
  
}

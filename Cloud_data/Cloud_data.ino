// Libraries
/*OBDII-UART-Serial version 9
* This program will talk to vehicle using the OBDII-UART board, 
*and upload to freeboard IoT platform
* 
* 
* updates:
*   v3: modified the getResponse() function so that the buffer receives the correct response.
*       add the getRPM() to get the engine RPM from the vehicle.
*   v4: add the getSpeed() function to get the speed of the vehicle
*   v5: is the wifi version
*   v6: is the non-wifi, non-serial version. Remove serial initialization,
*       so that the board can work without a computer.
*   v7: is the non-wifi, non-serial version. Add fuel level and coolant temperature.
*       rearrange the display location.
*   v8: is the wifi, non-serial version. Upolad speed, RPM, fuel level and coolant temperture
* 
* 10 K potentialmeter:
* ends to +5V and ground

*/

////////////////////////////////////////////////////////
//
// WiFi related //
///////////////////////////////////////////////////////
// ket noi board voi wifi
#include <SPI.h>
#include <WiFi101.h>
//Thư viện WiFi101 giúp việc sử dụng chip Wi-Fi onboard của board MKR1000 trở nên dễ dàng, và dễ kết nối board vào Internet.
// Credentials
char ssid[] = "Happy Family";     //  your network SSID (name)// ssid: tên WiFi của điểm truy cập mà chúng ta muốn kết nối đến, có thể có tối đa lên đến 32 ký tự.
char pass[] = "02022001";  // your network password, nhập pass mạng truy cập
char server[] = "dweet.io";  // freeboard and dweet Settings
unsigned long lastConnectionTime = 0; // track the last connection time, theo dõi thời gian cuối
const unsigned long postingInterval = 10L * 1000L; // post data every 10 seconds, up data mỗi 10 s
WiFiClient client; //Initialize the wifi client
int status = WL_IDLE_STATUS;     // the Wifi radio's status //Giai đoạn WiFi đang trong quá trình thay đổi giữa các trạng thái
// Khởi tạo các biến lưu giá trị tín 
int vSpeed = 5;
int vRPM = 5;
int vFuel = 5;
int vCool_Temp = 5;
int vOil_Temp = 5;
int vVoltage = 5;

void setup() {
  
  // Serial 
 Serial.begin(115200);

  // Kiểm tra trạng thái kết nối cho đến khi có báo cáo WL_CONNECTED
   // check the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi not ready");
    while(true);  
  }
  while ( status != WL_CONNECTED) {
    // Không kết nối được mạng   
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    
    // Connect to WPA/WPA2 network:
//Chuyển đối sang chế độ station, ta dùng hàm begin. Các tham số cần thiết sẽ là SSID và password, để module có thể kết nối đến một Access Point (AP) cụ thể    
    status = WiFi.begin(ssid, pass);

    // Wait 10 seconds for connection:
    delay(5000);
  }

  // Khi đã kết nối, in ra dòng xác nhận
  // tương đương với điều kiện if (WiFi.status() == WL_CONNECTED)
  Serial.print("You're connected to the network");
  printCurrentNet();//Hàm kiểm tra kiểm tra kết nối network
  printWifiData();
  Serial1.begin(11520);

}


void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // In ra địa chỉ Mac của Arduino MKR1000 station
  // khởi tạo biến mac là một con trỏ đến vị trí bộ nhớ (một mảng uint8_t có 6 phần tử) để lưu địa chỉ mac.
  byte mac[6];
  WiFi.macAddress(mac);// Lấy địa chỉ mac
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}
// hàm này in ra dữ liệu về mạng Wi-Fi mà board Arduino đang kết nối tới:
void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());//Trả lại tên của mạng Wi-Fi đã kết nối.

  // print the MAC address of the router you're attached to:
  // Xuát địa chỉ MAC router mà arduino kết nối đến
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");// Trả về địa chỉ MAC router mà arduino kết nối đến
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
 // Xuất ra  cường độ tín hiệu của mạng Wi-Fi
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");// Trả về cường độ tín hiệu của mạng Wi-Fi
  Serial.println(rssi);

  // print the encryption type:
  //Xuất kiểu báo mật wifi sử dụng
  byte encryption = WiFi.encryptionType();//Hàm Wifi.encryptionType trả về kiểu bảo mật mà mạng Wifi sử dụng
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
void loop() {
  while ( status != WL_CONNECTED) {
    /*lcd.clear();
    lcd.setCursor(0,0);*/
    // Connect to WPA/WPA2 Wi-Fi network
    Serial.println("Connecting to Wifi");
   /* lcd.println("Connect WiFi...");*/
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection
    delay(5000);
  }
  /*
  getSpeed();
  getRPM();
  getFuel();
  getCoolTemp();*/
  
  if (millis() - lastConnectionTime > postingInterval) {
      httpRequest(vSpeed, vRPM,  vFuel, vCool_Temp, vOil_Temp, vVoltage);// gửi yêu cầu 
      lastConnectionTime = millis();
   }
    // Kiểm tra kết nối network mỗi 10s
  
  printCurrentNet();//Hàm kiểm tra kiểm tra kết nối network
}
void httpRequest(int vSpeed, int vRPM, int vFuel,  int vCool_Temp, int vOil_Temp, int vVoltage ) {
  client.stop(); // đóng kế nối 
  // create data string to send to freeboard. tạo chuổi data gửi 
  if (client.connect(server, 80)){ // clien.connect: điều kiện kiểm tra kết nối địa chỉ khách , 80 la ổ chắm máy chủ 
    Serial.println("Connected");
    Serial.println(vRPM);
    Serial.println(vSpeed);
    Serial.println(vFuel);
    Serial.println(vCool_Temp);
     Serial.println(vOil_Temp);
        Serial.println(vVoltage);     
    
    
   String data = "GET /dweet/for/arduino?RPM="; 
   data.concat("&RPM=");
    data.concat(vRPM); // upload engine RPM
    data.concat("&Speed=");
   data.concat(vSpeed);  // upload car speed
    data.concat("&Fuel=");
    data.concat(vFuel);  // upload fuel level
    data.concat("&Cool_Temp=");
    data.concat(vCool_Temp);  // upload coolant temperature
    data.concat("&Voltage=");
    data.concat(vVoltage);  // upload coolant temperature
    data.concat("&Oil_Temp=");
    data.concat(vOil_Temp);  // upload coolant temperature
    
    
    
    
    client.println(data); //in ra thông số cho máy khách 
    
    
  
//client.println("GET/dweet/for/arduino?RPM=" + String(vSpeed) + " HTTP/1.1");//
     // Send the HTTP PUT request:
//client.println("GET /dweet/for/arduino?RPM=" +String(vSpeed) + String(vRPM)+ String(vFuel)+ String(vTemp)+ " HTTP/1.1");
client.println("Host: dweet.io");
client.println("User-Agent: ArduinoWiFi/1.1");
client.println("Connection: close");
client.println();
// Note the time that the connection was made:
lastConnectionTime = millis();
}
else {
// if you couldn't make a connection:
Serial.println("connection failed");
}
}
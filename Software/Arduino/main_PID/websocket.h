#pragma once

// Explain these. 
int myint;
int oldint = 0;

void onEventsCallback(WebsocketsEvent event, String data) 
{
  (void) data;
  
  if (event == WebsocketsEvent::ConnectionOpened) 
  {
    //Serial.println("Connnection Opened");
  } 
  else if (event == WebsocketsEvent::ConnectionClosed) 
  {
    //Serial.println("Connnection Closed");
  } 
  else if (event == WebsocketsEvent::GotPing) 
  {
    //Serial.println("Got a Ping!");
  } 
  else if (event == WebsocketsEvent::GotPong) 
  {
    //Serial.println("Got a Pong!");
  }
}

void sendMessage(String message )
{
// try to connect to Websockets server
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  //bool connected = client.connect("wss://192.168.1.104:80/");
  //bool connected = client.connect("192.168.1.104:80/");
  
  if (connected) 
  {
    Serial.println("Connected!");

    //String WS_msg = String("Hello to Server from ") + BOARD_NAME;
    client.send(message);
  } 
  else 
  {
    Serial.println("Not Connected!");
  }
}

void websocket_init()
{
  Serial.println("\nStarting SAMD-Client with WiFiNINA on " + String(BOARD_NAME));
  Serial.println(WEBSOCKETS2_GENERIC_VERSION);
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) 
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    return;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
  {
    Serial.println("Please upgrade the firmware");
  }

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++)
  {
    Serial.print(".");
    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Connected to Wifi, IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Connecting to WebSockets Server @");
    Serial.println(websockets_server_host);
  }
  else
  {
    Serial.println("\nNo WiFi");
    return;
  }
  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) 
  {
    //Serial.print("Got Message: ");
    //Serial.println(message.data());
    myint = message.data().toInt();
  });

  // run callback when events are occuring
  client.onEvent(onEventsCallback);

}

//If there is a client avaliable it will retrive the byte and if it is differrent
//from the last byte sent it will print it to serial monitor
byte get_data()
{
  if (client.available()) 
  {
    client.poll();
    return myint;
//    if (myint != oldint)
//    {
//      oldint = myint;
//      return myint;
//      Serial.println(myint);
//    }
  } 
}

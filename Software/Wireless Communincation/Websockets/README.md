# Websocket Commuication for Arduino with Wifi

## Goals
The goal of the websocket communication is to enable an Arduino with Wifi to establish a websocket connection, enabling the ability for the attitude (or angle for each axis) to be wirelessly updated through a web browser and the Arduino is able to recive the latest set desired attitude data. 

### 
Why websockets?
- Websockets enable for a realtime communication connection between two devices. An example of websockets is how online chatrooms function. 

## Updates 

### 3/24/22
- Created basic Python Flask based webserver with simple HTML webpage to enable for user to enter in data into a form and then submit their data and send it through a SocketIO (Flask websocket package) to communicate between all the different webrowser connections.



<p align="center">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/Software/Flask_socketio_server_test.png" width="500" >
</p>

- You can see below the browser on the left sends "250" and both browsers recives it. Then the browser on the right sends "100" and both browsers recives it.

- Now it is required for the Arudino with Wifi to connect to the websocket and recive the data send from the browser.

---
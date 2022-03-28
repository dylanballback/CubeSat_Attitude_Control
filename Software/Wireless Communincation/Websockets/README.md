# Websocket Commuication for Arduino with Wifi

## Goals
The goal of the websocket communication is to enable an Arduino with Wifi to establish a websocket connection, enabling the ability for the attitude (or angle for each axis) to be wirelessly updated through a web browser and the Arduino is able to recive the latest set desired attitude data. 

### 
Why websockets?
- Websockets enable for a realtime communication connection between two devices. An example of websockets is how online chatrooms function. 

## Updates 

### 3/24/22
- Created basic Python Flask based webserver with simple HTML webpage to enable for user to enter in data into a form and then submit their data and send it through a SocketIO (Flask websocket package) to communicate between all the different webrowser connections.
<style>
        .center {
            text-align-last: center;
            border: 2px solid black;
        }
</style>

<div class="center">
    <p>
        <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/Software/Flask_socketio_server_test.png" width="500" >
    </p>
</div>

---
### Upload Arduino sketch through Raspberry Pi Terminal

Reference [This tutorial](https://dle-dev.com/index.php/en/2020/07/30/program-an-arduino-with-a-raspberry-pi-via-ssh/)

## How it works

# Installing PlatformIO Core on Raspberry Pi

`sudo python3 -m pip install -U platformio`

First, you must connect to the Raspberry PI via SSH:

    <html>
        <head>
        ssh mylogin@addressIpRaspberry
        </head>

If so, Python3 and pip must be installed. Otherwise enter the following command:

    <html>
        <head>
        sudo apt-get install python3 python3-pip
        </head>

Installation of PlatformIO Core on the Raspberry PI:

    <html>
        <head>
        sudo python3 -m pip install -U platformio
        </head>


### Upload Arduino sketch through Raspberry Pi Terminal

## Disclaimer
This is from [Here](https://dle-dev.com/index.php/en/2020/07/30/program-an-arduino-with-a-raspberry-pi-via-ssh/)

## How it works

# Installing PlatformIO Core on Raspberry Pi

First, you must connect to the Raspberry PI via SSH:

    ssh mylogin@addressIpRaspberry

If so, Python3 and pip must be installed. Otherwise enter the following command:

    sudo apt-get install python3 python3-pip


Installation of PlatformIO Core on the Raspberry PI:

    sudo python3 -m pip install -U platformio


# PlatformIO project creation

PlatformIO contains a database with configurations for many on-board boards. Toolchains and build / download scripts are installed seamlessly. Therefore, it is possible to automatically generate pre-configured projects by specifying the name of the card.

First you need to create a directory for the Arduino program project.

    mkdir ~/ArduinoProject && cd ~/ArduinoProject

PlatformIO project initialization:

    platformio init -b uno

If necessary you can replace “uno” by the name of the card used. The “platformio boards” command returns the list of boards supported by PlatformIO

# Arduino source code upload file

The default main source file is: src/main.cpp

– Edit source files on another computer and copy files via a “scp” command

    scp main.cpp mylogin@addressIpRaspberry:ArduinoProject/src

– Using a version manager (svn, git …)

This solution is the cleanest and most modern. It allows better management of the evolution of the code and of teamwork.

# Compilation and Upload

To compile, you must go to the PlatformIO project directory and enter the command:

    platformio run
    
During the first compilation for a card type, platformIO will download and install all the necessary libraries. It may take some time depending on the connection and the type of Raspberry Pi.

To compile and upload to the Arduino:

    platformio run -t upload


# Serial monitor

The serial monitor is started with the following command:

    platformio device monitor

To start the compilation, upload and serial monitor:

    platformio run -t upload && platformio device monitor
# ACTS-Sensor
This repository contains code for the ACTS sensor module.
The use of this repository is limited to the ACTS (sensor) project.

## Brief explanation about project
This project is meant to collect detailed data from CVA (Cerebral Vasculair Attack) patients. This can then be parsed by complex algorithms and scientists to study movement in these patients. The sensor module also creates a simple performance index that is sent to an AWS backend server. These performance indexes are then parsed to provide feedback to therapists and CVA patients on which they can improve.

### Technical description
This project can be run on a ESP32 with ESP-iDF (2.1). It collects sensor data from the BMP280 and MPU9250.
This data is sampled at (currently) 100Hz and stored in an SD-card. It also creates a performance index counter based on intensity of sampling data. This is then send to an aws backend server. The results can be retrieved from the backend server to generate statistics.

## Recommendations
Before using this project, we recommend that you study the following subjects and resources carefully:

### FreeRTOS
ESP-IDF uses FreeRTOS.
FreeRTOS is a RTOS (Real Time Operating System).
ESP-IDF has a slightly modified version of FreeRTOS but much of it remains the same.
We recommend that you read up on the following resources:
https://www.freertos.org/

### ESP-IDF (2.1)
ESP-IDF is the C framework that is used as the basis for this project.
We commend a read up on the following resources before working on this project:

http://esp-idf.readthedocs.io/en/v2.1/get-started/index.html
http://esp-idf.readthedocs.io/en/v2.1/api-guides/general-notes.html

A link to the 
page that contains examples, which we recommend you also study:

https://github.com/espressif/esp-idf/tree/release/v2.1

### C++ Language
While FreeRTOS and ESP-IDF (2.1) are written in C and some parts in assembly, this project is mostly written in C++.
We recommend that the reader knows the basics of C++ and the  OOP (object oriented programming) paradigm that comes with it.

## Project Setup
### Software
	- Linux, Mac or Windows
	- [ESP-IDF 2.1] (http://esp-idf.readthedocs.io/en/v2.1/)

	Recommended
	-	Eclipse IDE (https://esp-idf.readthedocs.io/en/v2.0/eclipse-setup.html)
	
### Installation
Installation of ESP-IDF 2.1 can be found on the GitHub page of ESP-IDF. 
ESP-IDF 2.1 installation guide
https://github.com/espressif/esp-idf/tree/release/v2.1
	
## Project features
## Software description
This chapter will describe how the software is build from multiple perspectives. 
First a general overview is given about the software. After that the interfaces with explanation followed by a concurrency diagram for task responsibility within FreeRTOS. Afterwards a short guide is given on how to create and change the project to meet the users requirements.

### General overview
The esp32 is a duocore system. This project is designed to utilize both cores with different responsibilities. In short this means that the app core (core 1) is used as the sampling core for fast code routines that do not require alot of time to finish or compute.
Currently the app core samples its peripherals at a 100Hz, however this could be increased if desired.
The second core runs more complex code and code that has a long deadline to finish. The WiFi code can easily take up 5 seconds of CPU time to finish with its retries. Because of the split of responibilities this is acceptable within the current project.

### Interface description
First, a simple overview of all classess is given, with their respective responsibilities. Aftwards a more in-depth version is given that shows the interface of each class.
![alt text](readme-content/actsclassbasic.png)

Markup :
* Blue represents Controller classes. These are responable for system logic. They execute what should be executed and handle task management among them.

* Purple represents Boundary classes. These classes interface with the hardware and drivers.

* Red represents the Utility classes. These are support classes for the rest of the system created for maintanability and code structuring (reducing complexity).

The Sensor classes share a common parent which is Sensor. This ensures that they share the same interface to access functionality.
The same is true for the controllers. These controllers also act as tasks, so they share a BaseTask parent. This is done to make it easier to customize the project or expand upon it. By ensuring all tasks are made in the same way maintanablity and developped becomes easier. It is recommended to follow this pattern when expanding upon this project.

![alt text](readme-content/actsclassinterface.png)

For a further detailed explanation about each class, every class is documented and doxygen has been used to generate HTML pages with the documentation. 

### Task structuring
![alt text](readme-content/actsconcurrency.png)

The Sensortask is activated every 10 ms (100Hz) by a timer and with retrieve a sample of the sensors. The results of the measurement will be send to the DoubleBuffer who stores it internally in a BinaryBuffer that is swappable. When 1 of the 2 internal BinaryBuffers are full, Sensortask will set the BufferReady flag which tells the SdWriterTask to write that BinaryBuffer onto the SD-card.

The SdWriterTask is activated when one of the BinaryBuffers is full, writes the data to the sd card and goes back to wait for the flag to be set again.

The WifiTask is acivated every x time. The Wifitask sends the Movement stack data, which contains performance values to the aws server. It will retry x times and if it fails will go back to sleep, if a succesful connection could be established all data will be send to the AWS server.

The StandbyTask is an activate task with the lowest priority (otherwise it would block the system from sampling!). If the StandbyTask receives a signal from any part of the system to set the system in sleep, it will send the connected flags to SensorTask, SdWriterTask and WifiTask.
It will then respond to interrupts that are system generated.

### Project short guide
Description on recommendations on different project types (such as complex sampling, lots of sampling or lots of WiFi connections). Also fast sampling rates (max 1000Hz espidf?)

## Hardware
Description about the current hardware platform that is used and its interfaces.
### Hardware requirements
Hardware List
Markup : 
* esp32 512 kB ram+
* SD Cardreader SPI
* BMP280 Pressure & Temperature sensor
* MPU-9250 9DOF Magneto, Gyro, Accel

### Hardware pinout
![alt text](readme-content/esp32-connections.png)

## Server
The aws platform is used as the server platform. This chapter will be a small guide consisting mostly of references to explain how the backend works.
MQTT is used as communication protocol to exchange data between the sensor module (this project) and the server.

![project connection image](readme-content/english-db-structure.jpg)

The server consists of three parts: the database, an online environment and a MQTT bus Broker. In the database there are three tables, these are displayed in the cloud. The tables are named results, goals and users. In the light blue blocks connected to the tables, the data inside the tables is described. 

The MQTT broker runs in Amazon Web Services (AWS). The broker provides the sensor module to get the data online available with a MQTT HTPPS/socket connection. The security is important so it uses an embedded certificate to connect. On top of that there is also a difference between the users and the sensor modules by the usage of keys. The broker uses a lambda function written in Python to include the incoming data in the database. The build in lambda system in AWS and Python code that uses a MySQL connector module to add data, are used for this action.

On the broker it’s automatically checked if a user already has a goal for the concerning day. If this is not the case the Python code that is running on the server takes by means of a query the average of the past 7 days. This value then gets increased with a set percentage increase that can be picked per patient.

### Server setup
Note that for every aws service the eu-central Frankfurt server is used.
#### AWS IoT
To get started with the AWS IoT service, we recommend following this guide:
https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html

To test the server instance from a client device you can use curl to emulate MQTT messages using the following command type:

	curl --tlsv1.2 --cacert amazon-cert-bundle.crt --cert amazonggenerated-certificate.pem.crt --key amazon-private.pem.key -X POST -d "	{\"jsonkey\": \"jsonvalue\", \"jsonkey2\": \"jsonvalue2\"}" ""https://server_id.iot.eu-central-1.amazonaws.com:8443/topics/your/topic?qos=1"

The certificate and keys can be generated by aws when following the guide.

#### AWS Lambda
The AWS Lambda service makes it possible to execute code routines. These can be linked to other AWS services. This means that the AWS Lambda will execute when the AWS IoT service receives specific packages on specific topics.

see https://docs.aws.amazon.com/iot/latest/developerguide/iot-lambda-rule.html for more information about this topic.

#### AWS SnS
AWS SnS allows the user to easily send messages using standardized protocols outside of the AWS space. This allows the user to set up rules that if triggered, can send emails, sms (text messages) or trigger other html pages on the web in a very easy manner. For testing this is very convienent because every time a AWS rule is triggered, an email can be send.

#### AWS RDS
AWS RDS is a database management service. It allows the user to store information in a database.

## Links/References
* ESP32: http://espressif.com/en/products/hardware/esp32/overview
* ESP32 setup: http://espressif.com/en/products/hardware/esp32/overview

## Creating a working device
### Programming / uploading to the device
The standard device should operate at 160 MHz minimum to guarantee that all operations are performed in time.
Other than that, the project should just be compiled and uploaded to the esp32 device.
### Generating data
After uploading the device should start running. If possible check with a terminal for device input that could suggest system faults.
If no errors are detected the device should generate data when moving the device around (dont let it go into deepsleep mode) and after x time try and send it to the server.
### Sending it to the server
After x time the sensor device will try to send the performance index to the server. The protocol used is MQTT. The messages are send to topics in AWS.
#### MQTT testing
AWS allows for testing on topics. You can subscribe to specific topics or a range of topics.

## Bugs	
~Good luck developing more content from the ACTS Team and me ♥.

## Future improvements
improvements and / or changes:
Markup : 
* DataProcessor to data processing task (when doing complex calculations)
* Controllers and Basetask changed to allow a generic way of creating the actual tasks in ESP-IDF, this can be done by calling a function ptr in the basetask that calls the main_task again. By implementing the main_task in its children, internal logic can change but interface stays the same.


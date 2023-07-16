# ESP32 Weather Station

The ESP32 Weather Station is a project that monitors environmental conditions using the BMP180 and DHT11 sensors. It uploads the sensor data to Google Firebase Realtime Database (RTDB) and includes authentication for secure access.

## Installation

To set up the ESP32 Weather Station, follow these steps:

1. Clone this repository to your local machine.

```shell
git clone https://github.com/your-username/esp32-weather-station.git
 ```
1. Connect the BMP180 and DHT11 sensors to the ESP32 board according to the wiring instructions provided in the "Hardware Setup" section below.

2. Install the necessary libraries for the ESP32 board, BMP180, and DHT11. Refer to the "Dependencies" section below for the required libraries and installation instructions.

3. Create a Firebase project and set up the Realtime Database. Obtain the Firebase credentials (API key, project ID, etc.) required for authentication.

4. Update the configurations in the project with your Firebase credentials and other configuration settings. Make sure to set the correct pin numbers for the BMP180 and DHT11 sensors.

5. Compile and upload the code to your ESP32 board using the Arduino IDE or any other compatible development environment.

## Usage

Once the ESP32 Weather Station is set up and running, it will continuously monitor the environment and upload the sensor data to the Firebase RTDB. Follow these steps to access and analyze the data:

1. Open the Firebase console and navigate to your project's Realtime Database.

2. Authenticate using the provided authentication method (e.g., email/password, Google Sign-In).

3. Access the sensor data stored in the database. You can use Firebase's REST API or SDKs in various programming languages to retrieve and process the data.

4. Analyze the data to gain insights into the environmental conditions over time.

## Hardware Setup

The hardware setup for the ESP32 Weather Station is as follows:

1. Connect the BMP180 sensor to the ESP32's I2C interface.
2. Connect the DHT11 sensor to a digital pin of the ESP32 board.


## **Dependencies**

The ESP32 Weather Station relies on the following libraries:

1. Adafruit_BMP085 or Adafruit_BMP280 (for BMP180 sensor)
2. DHT (for DHT11 sensor)
3. FirebaseESP32 (for Firebase Realtime Database integration)
4. To install these libraries, follow the instructions provided in their respective repositories.

## Contributing
Contributions are welcome! If you find any issues or have suggestions for improvements, please feel free to open an issue or submit a pull request.

## Contact
If you have any questions or need further assistance, you can reach out to the project maintainer at:

Name: Imdadul Rehmaan

Email: rehmaankhan287@gmail.com

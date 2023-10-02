# Arduino + MQTT + InfluxDB 2

This repository implements a MQTT5 broker, InfluxDB 2.x, Telegraf and Arduino ESP32 MQTT PubSubClient to send data to InfluxDB 2.x. The sensor data corresponds to a DHT11 temperature and humidity sensor.

<details>
<summary>Table of Contents</summary>

- [Requirements](#requirements)
- [Usage](#usage)
    - [Arduino Setup](#arduino-setup)
    - [Telegraf Setup](#telegraf-setup)
    - [Docker Setup](#docker-setup)

</details>

## Requirements

- Docker
- Docker Compose
- Arduino IDE
- ESP32
- DHT11 sensor

## Usage

1. Clone this repository

### Arduino Setup
2. Open the Arduino IDE and install the PubSubClient and Adafruit DHT Sensor libraries
3. Open the `secrets.h` file and replace the placeholders with your WiFi and MQTT broker credentials
4. Open the `publish.ino` file and replace the `DHTPIN`, `MQTT_CLIENT_ID`, `MQTT_SERVER`, `MQTT_TOPIC` with your own values
5. Upload the sketch to your ESP32

### Telegraf Setup
6. Open the `telegraf.conf` file and replace the `servers` and `topics` with your own values

### Docker Setup
7. Create a `.env` file with the following variables:
    ```
    MQTT_USERNAME=changeme
    MQTT_PASSWORD=changeme
    INFLUXDB_TOKEN=default
    DOCKER_INFLUXDB_INIT_USERNAME=changeme
    DOCKER_INFLUXDB_INIT_PASSWORD=changeme
    DOCKER_INFLUXDB_INIT_ORG=changeme
    DOCKER_INFLUXDB_INIT_BUCKET=changeme
    ```

    Note: `MQTT_USERNAME` and `MQTT_PASSWORD` must match the credentials in the `secrets.h` file. This will later be created as a user in the MQTT broker. Also the `INFLUXDB_TOKEN` will be later created as a token in InfluxDB.

8. Create a Mosquitto password file
    ```
    touch mqtt_broker/config/mosquitto/pwfile
    ```

9. Create and run docker container for only mqtt5 broker
    ```
    docker-compose up -d mqtt5
    ```

10. Create a user in the MQTT broker. Replace `<MQTT_USERNAME>` and `<MQTT_PASSWORD>` with your own values also used in the `secrets.h` file.
    ```
    docker-compose exec mqtt5 mosquitto_passwd -b /mosquitto/config/pwfile <MQTT_USERNAME> <MQTT_PASSWORD>
    ```

11. Create and run all docker containers
    ```
    docker-compose up -p arduino-mqtt-influx --build -d
    ```

12. Open the InfluxDB UI at `http://localhost:8086` and create a new API token through the `Load Data` -> `API Tokens` menu. Replace the `INFLUXDB_TOKEN` in the `.env` file with the newly created token.

13. Recreate all docker containers
    ```
    docker-compose up -p arduino-mqtt-influx --build -d
    ```
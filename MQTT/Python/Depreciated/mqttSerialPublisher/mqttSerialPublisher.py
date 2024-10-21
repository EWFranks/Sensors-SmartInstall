#import sys, getopt
import serial
import paho.mqtt.client as mqtt
import configparser

# SERIAL RECEIVED DATA



def readConfig():
    config = configparser.ConfigParser()
    config.read('config.ini')


    # FORMAT: dict['brokerIP'] = config['MQTT']['brokerIP']
    dict = {}

    for section in config.sections():                   # For each section
        for option in config.options(section):          # For each option in that section
            dict[option] = config[section][option]      # Add it to the dictionary under that option

    return dict


if __name__ == '__main__':

    configDict = readConfig()


# Start MQTT

    client = mqtt.Client( configDict['id'] )
    client.connect( configDict['broker'], int(configDict['port']) )


# Start Serial
    comPort = "COM" + configDict['com']
    ser = serial.Serial(comPort, 9600, timeout=10)

    line = ""
    while True:     # LOOP FOREVER

    # Parse Serial Data
        ser.flush()

        line = str(ser.readline())
        readings = line.split()

        readings[0] = readings[0][2:] # Get rid of "'b"

    # Publish each reading to MQTT with format "topic:value"
        for reading in readings:

            if ':' in reading:
                topicValuePair = reading.split(':')
                topic = topicValuePair[0] + "/" + "UnoSensorKit" + configDict['id'] # Publishing with format "Topic/UniqueIdentifyingSubtopic"
                value = topicValuePair[1]
                
                client.publish(topic, value)

        print(readings)




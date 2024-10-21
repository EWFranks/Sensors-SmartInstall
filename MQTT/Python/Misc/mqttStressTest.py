import paho.mqtt.client as mqtt
import time


def main():
    client = mqtt.Client()
    client.connect("192.168.2.238", 1883)

    while True:
        client.publish("temperature/TEST", "93 Celsius")
        client.publish("temperature/TEST", "92 Celsius")
        print("sent")
        time.sleep(.1)


if __name__ == '__main__':
    main()

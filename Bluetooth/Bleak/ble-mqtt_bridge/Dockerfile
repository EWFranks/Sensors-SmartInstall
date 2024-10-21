FROM python:3.8

WORKDIR /app

ADD ble-mqtt_bridge.py .
ADD requirements.txt .
#ADD docker_entrypoint.sh .
# COPY /Data /app/Data

RUN apt-get update
RUN apt-get install -y bluez bluetooth
#ENTRYPOINT sh docker_entrypoint.sh

RUN pip install --no-cache-dir -r requirements.txt

CMD ["python", "ble-mqtt_bridge.py"]
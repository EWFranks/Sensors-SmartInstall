FROM python:3.8-slim-buster

WORKDIR /app

ADD occupancy_mqtt.py .
ADD requirements.txt .

RUN pip install --no-cache-dir -r requirements.txt
CMD ["python", "occupancy_mqtt.py"]
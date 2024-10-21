FROM python:3.8-slim-buster

WORKDIR /app

ADD mqtt_api_bridge.py .
ADD requirements.txt .

RUN pip install --no-cache-dir -r requirements.txt
CMD ["python", "mqtt_api_bridge.py"]
import requests

json_data = {
    "device_id" : "L4MP",
    "command": "power_off",
    "args": {
        # "option": "color",
        # "value": "white"
    }
}

#requests.post('localhost:2020/commands/L4MP', json=json_data)
response = requests.post('http://localhost:2020/commands/L4MP', json=json_data)

print(response.status_code)
print(response.text)
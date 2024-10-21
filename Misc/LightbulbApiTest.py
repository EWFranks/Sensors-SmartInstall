""" This is a file to test requests to the LIFX API."""

import requests

token = "c67e5bf7c0e08de5b3f86d221b40a5fe2a15dc97747711d2d36ed492798b7788"
label = "ERDC PROJECT TEST LIGHTBULB 💡"

headers = {
    "Authorization": "Bearer %s" % token,
}

    # -> GET request to get all lights
response = requests.get(f'https://api.lifx.com/v1/lights/label:{label}', headers=headers)


    # -> PUT request change light state
# putJson = {
#     "power": "on",
#     "color": "blue",
#     "brightness": 0.5,
#     "duration": 5,
# }

# response = requests.put('https://api.lifx.com/v1/lights/all/state', headers=headers, json=putJson)

    # -> POST request to do Morph effect
# postJson = {
#     "period": 10,
#     "duration": 5,
#     "palette": ["red", "orange", "yellow", "green", "blue", "purple", "pink", "white"],
#     "power_on": True,
#     "fast": False
# }

# response = requests.post('https://api.lifx.com/v1/lights/all/effects/morph', headers=headers, json=postJson)


print(response.json())
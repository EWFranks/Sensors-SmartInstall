import requests

json_data = {
    "L4MP": {
        "device_type": "Lifx Lightbulb",
        "building" : "Rayzor",
        "room": "2015",
        "zone" : "K15"
    },

    "Z0R0": {
        "device_type": "SenSource Occupancy Sensor",
        "building" : "Rayzor",
        "room": "2015",
        "zone" : "K15"
    }
}


# response = requests.post('http://localhost:2020/api/data/boards', json=json_data)




# response = requests.get('http://localhost:2020/api/data/boards', params={"zone": "K15", "building": "Rayzor", "room": "2015"})

#response = requests.delete('http://localhost:2020/api/data/boards/T3ST') 

json_data2 = {
    "COFE" : {
        "device_type": "Lifx Lightbulb",
        "building" : "Rayzor",
        "room": "2015",
        "zone" : "K15"
    }
}

# response = requests.post('http://localhost:2020/api/data/boards', json=json_data2)

# json_data3 = {
#     "device_id" : 'C0FE',
#     "device_type": "BRIGHTBULB",
#     "building" : "Bayzor",
#     "room": "B2015",
#     "zone" : "B15"
# }

# response = requests.put('http://localhost:2020/api/data/boards/COFE', json=json_data3)


# json_data4 = {
#     "device_id" : "BOFE",
#     "device_type": "Lifx Lightbulb",
#     "building" : "Rayzor",
#     "room": "2015",
#     "zone" : "K15"
# }

# response = requests.post('http://localhost:2020/api/data/boards/LOFE', json=json_data4)

response = requests.get('http://localhost:2020/api/data/readings')

print(response.status_code)
print(response.text)
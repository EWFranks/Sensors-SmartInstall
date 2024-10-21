import requests


def post_test(dict):

    postRequest = requests.post("http://localhost:5000/api/data", json=dict)
    return postRequest.text

if __name__ == "__main__":

    dict = {"test" : "1", "best" : "2", "rest" : "3"}
    print( post_test(dict) )
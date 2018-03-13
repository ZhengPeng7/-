import requests
# import cv2
import os


def download_from_url(url, file_name):
    try:
        response = requests.get(url, timeout=2)
    except:     # (requests.exceptions.ReadTimeout,
                # requests.exceptions.ConnectTimeout):
        try:
            response = requests.get(url, timeout=2)
        except:
            print("Fail to fetch img from " + url + '\n')
            return -1
    with open(file_name, "wb") as file:
        file.write(response.content)
        # Use cv2.imread to check out if reponse is image
        # t_img = cv2.imread(file_name)
        # if t_img is None:
        #     os.remove(file_name)
    return 0

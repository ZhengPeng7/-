import urllib
import json
import sys
import urllib.request
import os
import imp
import time
import requests
import download_from_url
import numpy as np
import cv2

imp.reload(sys)


# sys.setdefaultencoding('utf8')
if __name__ == "__main__":
    totalNumber = "TotalNumber"
    pageNumber = "ReturnNumber"
    pages = 0
    tn = 0
    pn = 0

    for hw in sys.stdin:
        line = hw.split("\t")
        w = urllib.parse.quote(line[0])
        vdir = line[1].replace("\n", "").strip()
        url = ("http://imgdata.baidu.com/platform/search?"
               "user=lbs_quanjing&word=" + w + "&pn=1&m=10")
        data = urllib.request.urlopen(url).read()
        value = json.loads(data)

        rootlist = value.keys()
        for rootkey in rootlist:
            subvalue = value[rootkey]
            for subkey in subvalue:
                # print("subkey={}, pageNumber={}".format(subkey, pageNumber))
                if (subkey == totalNumber):
                    tn = subvalue[subkey]
                if (subkey == pageNumber):
                    pn = subvalue[subkey]
        print(str(int(tn / pn))+":"+vdir)
        pages = int(tn / pn)
        print("pages:", pages)
        for p in range(1, pages, pn):
            url = ("http://imgdata.baidu.com/platform/search?user="
                   "lbs_quanjing&word="+w+"&pn="+str(p)+"&m=10")
            target_dir = os.path.join("/home/a/scripts_learning/JSONs", vdir)
            img_dir = os.path.join(target_dir[:target_dir.find('/')],
                                   'images', vdir)
            if not os.path.exists(target_dir):
                os.mkdir(target_dir)
            if not os.path.exists(img_dir):
                os.mkdir(img_dir)
            print("url for JSON:", url)
            urllib.request.urlretrieve(url, os.path.join(target_dir, str(p)))
            data = urllib.request.urlopen(url).read()
            value = json.loads(data)
            for j in range(pn):
                if j+p > pages-1:
                    break
                img_url = value['data']['ResultArray'][j]['ObjUrl']
                print("Reading {}/{} from: {}".format(j+p, pages-1, img_url))
                download_from_url.download_from_url(
                    img_url, os.path.join(img_dir, str(j+p))
                )
            # time.sleep(1)
            # time.sleep(60)

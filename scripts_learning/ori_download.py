import urllib
import json  
import sys
import urllib.request      
import os
import imp
import time
imp.reload(sys)
#sys.setdefaultencoding('utf8')
if __name__=="__main__":
	totalNumber = "TotalNumber"
	pageNumber = "ReturnNumber"  
	pages = 0
	tn = 0
	pn = 0

	for hw in sys.stdin:
		line = hw.split("\t")
		w = urllib.parse.quote(line[0])
		vdir = line[1].replace("\n","").strip()
		url = "http://imgdata.baidu.com/platform/search?user=lbs_quanjing&word=" + w + "&pn=1&m=10"
		data = urllib.request.urlopen(url).read()
		value = json.loads(data)
		rootlist = value.keys()
		for rootkey in rootlist:
			subvalue = value[rootkey]
			for subkey in subvalue:
				if (subkey == totalNumber):
					tn = subvalue[subkey]
				if (subkey == pageNumber):
					pn = subvalue[subkey]
		print (str(int(tn / pn))+":"+vdir)
		pages = int(tn / pn)
		for p in range(1,pages):
			url = "http://imgdata.baidu.com/platform/search?user=lbs_quanjing&word="+w+"&pn="+str(p)+"&m=10"
			target_dir = os.path.join("/home/ubuntu/ps/",vdir)
			if not os.path.exists(target_dir):
				os.mkdir(target_dir)
			urllib.request.urlretrieve(url,os.path.join(target_dir,str(p)))
			time.sleep(1)
		time.sleep(60)

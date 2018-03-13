## Function: Scraping images with key words by Baidu image interface

> Usage: 
>
> 1. Put your searching keywords in the query file with each line consisting of Chinese word+'\t'+Pinyin.
> 2. cat query | python download.py to run the scraping. If you have OpenCV, you can uncomment the 'cv2.imread' at the end of download_from_url.py to make sure the image you download to disk is really a readable image.
#!/usr/bin/python  
# -*- coding: utf-8 -*- 

import cv2
import sys
from PIL import Image
import requests
from bs4 import BeautifulSoup

argv_list = sys.argv

opt0 = argv_list[1].strip('"').strip('\\').strip(' ')
opt1 = argv_list[2]

def downloadFile():
	
	DownloadLink = argv_list[1].strip('"').strip(' ')
	DownloadFilePath = argv_list[3].strip('"').strip('\\').strip(' ')
	
	headers={'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36'}
	
	try:
		Download_request=requests.get(DownloadLink,headers=headers)
		Download_filename = DownloadFilePath
		with open(Download_filename,'wb+') as f:
			f.write(Download_request.content)
			
	except BaseException:
		print('failed')	

def getDuration(FILENAME):
	try:
		PIL_Image_object = Image.open(FILENAME)
		PIL_Image_object.seek(0)
		frames = 0
		duration = 0
		while True:
			try:
				frames += 1
				duration += PIL_Image_object.info['duration']
				PIL_Image_object.seek(PIL_Image_object.tell() + 1)
			except EOFError:
				print(int((duration / 1000)/frames * 100))
				return 0
		return None
	except BaseException:
		print(0)

def getVideoInfo():
	cap = cv2.VideoCapture(opt0)
	if opt1=="fps":
		fps = cap.get(cv2.CAP_PROP_FPS)
		print(fps)
	elif opt1=="countframe":
		frame_counter = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
		print(frame_counter)

if opt1=="fps":
	getVideoInfo()
elif opt1=="countframe":
	getVideoInfo()
elif opt1=="duration":
	getDuration(opt0)
elif opt1=="download2":
	downloadFile()

#!/usr/bin/python  
# -*- coding: utf-8 -*- 

import cv2
import sys
from PIL import Image
import requests
from bs4 import BeautifulSoup


argv_list = sys.argv

inputpath = argv_list[1]
opt = argv_list[2]

def checkUpdate():
	try:
		headers={'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36'}
		r1=requests.get('https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest',headers=headers)
		
		soup = BeautifulSoup(r1.text,'lxml')
		
		title = soup.title.string
		p_split_name = re.compile(r' ')
		
		Version_latest = p_split_name.split(title)[1]
		
		print(Version_latest)
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
	cap = cv2.VideoCapture(inputpath)
	if opt=="fps":
		fps = int(round(cap.get(cv2.CAP_PROP_FPS)))
		print(fps)
	elif opt=="countframe":
		frame_counter = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
		print(frame_counter)

if opt=="fps":
	getVideoInfo()
elif opt=="countframe":
	getVideoInfo()
elif opt=="duration":
	getDuration(inputpath)
elif opt=="checkupdate":
	checkUpdate()

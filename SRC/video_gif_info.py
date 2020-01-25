#!/usr/bin/python  
# -*- coding: utf-8 -*- 

import cv2
import sys
from PIL import Image


argv_list = sys.argv

inputpath = argv_list[1]
opt = argv_list[2]

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

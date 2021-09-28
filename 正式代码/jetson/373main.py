import struct

import cv2
import numpy as np
import glob
from scipy.spatial import distance
from imutils import face_utils
from keras.models import load_model
import tensorflow as tf

from fr_utils import *
from inception_blocks_v2 import *
import math
import threading
import time
import pyuart
import serial


serial_port = serial.Serial(
	port="/dev/ttyTHS0",  # 串口设备地址
	baudrate=115200,  # 信号波特率
	bytesize=serial.EIGHTBITS,  # 数据位
	parity=serial.PARITY_NONE,  # 是否启用奇偶校验
	stopbits=serial.STOPBITS_ONE,  # 停止位
)

def camera_read():
	global framebuf
	global video_capture
	while (video_capture.isOpened()) :
		if captureLock.acquire(blocking=True,timeout=0.01):
			ret, framebuf = video_capture.read()
			captureLock.release()
		time.sleep(0.04)


def find_dist_angle(x, y, w, h) :
	# dist is the approximate distance in reality, theta is the pixels face is away from center
	# left is negative, right is positive
	# Constants
	D_0 = 1.2
	H_0 = 100
	dist = D_0 *  H_0 /h
	theta= -(x + w / 2 - 320)
	print(h,dist, theta)
	distserial=struct.pack('f',dist)
	thetaserial=struct.pack('f',theta)
	serial_port.write('\r\n'.encode('utf-8'))
	serial_port.write(distserial)
	serial_port.write(thetaserial)
	serial_port.write('\n\r'.encode('utf-8'))


########################################### load one-shot learning model and data
#with CustomObjectScope({'tf': tf}):
FR_model = load_model('nn4.small2.v1.h5', compile=False)

print("Total Params:", FR_model.count_params())

face_cascade = cv2.CascadeClassifier('haarcascades/haarcascade_frontalface_default.xml')

threshold = 0.25

face_database = {}

for name in os.listdir('images'):
	for image in os.listdir(os.path.join('images',name)):
		identity = os.path.splitext(os.path.basename(image))[0]
		face_database[identity] = fr_utils.img_path_to_encoding(os.path.join('images',name,image), FR_model)

#print(face_database)
##########################################
data=''
print('ready')
while True :
	if serial_port.inWaiting() > 0 :
		data += serial_port.read().decode('utf-8')
		print(data)
		if data == 'ss' :
			print(data)
			break
		if len(data) > 5 :
			exit(3)

video_capture = cv2.VideoCapture(-1)
#video_capture = cv2.VideoCapture('threepeople.mp4')
tracker = cv2.TrackerCSRT_create()

cnt=0 #adjust the frequency of recognize. if cnt=0, recognize, else track
state=0 #means no face to track at present, need to recognize immediately
faces=[]
master=None
masters="Hanyun Zhao Yutong Wu Ziyi Wang"
framebuf=[]
captureLock=threading.Lock()

cameraReadThread=threading.Thread(target=camera_read)
cameraReadThread.start()

print('video loaded')

while (video_capture.isOpened()):

	#ret, frame = video_capture.read()
	if captureLock.acquire(blocking=True,timeout=1):
		frame=framebuf
		captureLock.release()
	frame = cv2.flip(frame, 1)
	frame=np.uint8(np.clip((cv2.add(1 * frame, 30)), 0, 255))
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

	if cnt==0 or state==0:
		state = 0 #if master is found in this process, state will be set to 1
		tracker = cv2.TrackerCSRT_create()#adjust the tracker
		faces = face_cascade.detectMultiScale(
			gray,
			scaleFactor=1.15,
			minNeighbors=5,
			minSize=(5, 5),
			flags=cv2.CASCADE_SCALE_IMAGE
		)
		for(x,y,w,h) in faces:
			cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 255, 0), 2)
			roi = frame[y:y+h, x:x+w]
			encoding = img_to_encoding(roi, FR_model)
			min_dist = 100
			identity = None

			for(name, encoded_image_name) in face_database.items():
				dist = np.linalg.norm(encoding - encoded_image_name)
				if(dist < min_dist):
					min_dist = dist
					identity = name
				#print('Min dist: ',min_dist)

			if min_dist < 0.1:#a team mate
				cv2.putText(frame, "Face : " + identity, (x, y - 50), cv2.FONT_HERSHEY_PLAIN, 1.5, (0, 255, 0), 2)
				cv2.putText(frame, "Dist : " + str(min_dist), (x, y - 20), cv2.FONT_HERSHEY_PLAIN, 1.5, (0, 255, 0), 2)
				if master==None:# the first time meet with a team mate
					tracker.init(frame, (x,y,w,h))
					find_dist_angle(x, y, w, h)
					master=identity[:8]
					state=1
					break #if a team member is found, do not go on recognizing
				else: #later will stick on this member
					#if master==identity[:8]:#if again find the master
					if identity[:8] in masters :#if is any one of the team member, do not support member in the screen
						tracker.init(frame, (x, y, w, h))
						find_dist_angle(x,y,w,h)
						state=1
						break
			else:# passer's by
				cv2.putText(frame, 'No matching faces', (x, y - 20), cv2.FONT_HERSHEY_PLAIN, 1.5, (0, 0, 255), 2)

	# if state == 0 and cnt == 0 :
	# 	if len(faces) == 1 :
	# 		tracker.init(frame, faces[0])
	# 		state += 1

	else : # track instead of recognize
		ok, bbox = tracker.update(frame)

		if ok :
			p1 = (int(bbox[0]), int(bbox[1]))
			p2 = (int(bbox[0] + bbox[2]), int(bbox[1] + bbox[3]))
			cv2.rectangle(frame, p1, p2, (255, 0, 0), 2, 1)
	cnt += 1
	if cnt == 10 :
		cnt = 0

	cv2.imshow('Face Recognition System', frame)
	if(cv2.waitKey(1) & 0xFF == ord('q')):
		break

video_capture.release()
cv2.destroyAllWindows()





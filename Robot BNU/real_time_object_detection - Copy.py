# To run the script, execute the following commands
# workon cv
# python3 real_time_object_detection.py --prototxt MobileNetSSD_deploy.prototxt.txt --model MobileNetSSD_deploy.caffemodel

# import the necessary packages
from imutils.video import VideoStream
from imutils.video import FPS
import numpy as np
import argparse
import imutils
import time

from pykinect import nui
import cv2
from pykinect.nui import JointId
from numpy import * 
import datetime
import os
import os.path
from os.path import join as pjoin
from pykinect.nui import SkeletonTrackingState
from PIL import Image
import shutil
import copy

save_image = True

current_directory = os.getcwd()
kinect = nui.Runtime()


def video_handler_function(frame):
    video = np.empty((480,640,4),np.uint8)
    frame.image.copy_bits(video.ctypes.data)
    cv2.imshow('KINECT Video Stream', video)
    # Update Gambar hasil tracking
    image_name = 'belgedes.jpg'
    if save_image:
        cv2.imwrite(image_name, video)


# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-p", "--prototxt", required = True,  help = "path to Caffe 'deploy' prototxt file")
ap.add_argument("-m", "--model", required = True, help = "path to Caffe pre-trained model")
ap.add_argument("-c", "--probability", type = float, default = 0.2, help = "minimum probability to filter weak detections")
args = vars(ap.parse_args())

# initialize the list of class labels MobileNet SSD was trained to detect
# and generate a set of bounding box colors for each class
CLASSES = ["background", "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
"dog", "horse", "motorbike", "person", "pottedplant", "sheep", "sofa", "train", "tvmonitor"]

COLORS = np.random.uniform(0, 255, size = (len(CLASSES), 3))

# load our serialized model from disk
print("[INFO] loading model...")
net = cv2.dnn.readNetFromCaffe(args["prototxt"], args["model"])

# initialize the video stream, allow the cammera sensor to warmup,
# and initialize the FPS counter
print("[INFO] starting video stream...")
#vs = VideoStream(src=0).start()

kinect = nui.Runtime()
kinect.video_frame_ready += video_handler_function
kinect.video_stream.open(nui.ImageStreamType.Video, 2,nui.ImageResolution.Resolution640x480,nui.ImageType.Color)
cv2.namedWindow('KINECT Video Stream', cv2.WINDOW_AUTOSIZE)

time.sleep(2.0)
fps = FPS().start()

# loop over the frames from the video stream
while True:
	# resize the video stream window at a maximum width of 500 pixels
	#frame = vs.read()

	shutil.copy("belgedes.jpg","belgedes2.jpg")
	
	frame_gambar = cv2.imread("belgedes2.jpg")

	frame_gambar = imutils.resize(frame_gambar, width=500)

	# grab the frame dimensions and convert it to a blob
	# Binary Large Object = BLOB

	(h, w) = frame_gambar.shape[:2]
	#blob = cv2.dnn.blobFromImage(cv2.resize(frame_gambar, (300, 300)), 0.007843, (300, 300), 127.5)
	blob = cv2.dnn.blobFromImage(cv2.resize(frame_gambar, (500, 500)), 0.007843, (500, 500), 127.5)

	# pass the blob through the network and get the detections
	net.setInput(blob)
	detections = net.forward()

	# loop over the detections
	for i in np.arange(0, detections.shape[2]):
		# extract the probability of the prediction
		probability = detections[0, 0, i, 2]

		# filter out weak detections by ensuring that probability is
		# greater than the min probability
		if probability > args["probability"]:
			# extract the index of the class label from the
			# 'detections', then compute the (x, y)-coordinates of
			# the bounding box for the object
			idx = int(detections[0, 0, i, 1])
			box = detections[0, 0, i, 3:7] * np.array([w, h, w, h])
			(startX, startY, endX, endY) = box.astype("int")

			# draw the prediction on the frame
			label = "{}: {:.2f}%".format(CLASSES[idx], probability * 100)
			cv2.rectangle(frame_gambar, (startX, startY), (endX, endY), COLORS[idx], 2)
			y = startY - 15 if startY - 15 > 15 else startY + 15
			cv2.putText(frame_gambar, label, (startX, y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, COLORS[idx], 2)

	# show the output frame
	cv2.imshow("KINECT Video Stream", frame_gambar)
	key = cv2.waitKey(1) & 0xFF

	# if the 'q' key was pressed, break from the loop
	if key == ord("q"):
		break

	# update the FPS counter
	fps.update()

# stop the timer and display FPS information
fps.stop()
print("[INFO] elapsed time: {:.2f}".format(fps.elapsed()))
print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))

# cleanup
kinect.close()
cv2.destroyAllWindows()

#vs.stop()
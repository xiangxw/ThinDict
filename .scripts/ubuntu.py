#!/usr/bin/python

import subprocess
import os

def getVersion():
	"""get version of ldict"""
	changelog = open('debian/changelog', 'r')
	firstLine = changelog.readline()
	words = firstLine.split(' ')
	word1 = words[1]
	version = word1[1 : word1.index('-')]
	changelog.close()
	return version

def dh_make():
	# delete existing origin source package
	for file in os.listdir('..'):
		if file[0:6] == 'ldict_':
			os.remove('../' + file)
	subprocess.call('export DEBFULLNAME="xiangxw"; dh_make -c lgpl3 -p ldict_%s --createorig --single --yes --email xiangxw5689@126.com'%(getVersion()), shell = True)

def debuild(series):
	"""debuild for one series"""

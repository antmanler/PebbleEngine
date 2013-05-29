# *************************************************************************************************
#
# Horde3D
#   Next-Generation Graphics Engine
# --------------------------------------
# Copyright (C) 2006-2009 Nicolas Schulz
#			   2008-2009 Florian Noeding (Python wrapper)
#
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# *************************************************************************************************

from ctypes import *

if not 'c_bool' in globals():
	c_bool = c_int


__all__ = []



try:
	h3d = cdll.LoadLibrary('libHorde3D.so')
except OSError:
	h3d = cdll.LoadLibrary('Horde3D.dll')

if hasattr(h3d, 'getActiveListener'):
	RST_SoundResource = 200
	__all__.append('RST_SoundResource')
	SNT_ListenerNode = 201
	__all__.append('SNT_ListenerNode')
	SNT_SoundNode = 202
	__all__.append('SNT_SoundNode')


	class SoundResParams(object):
		SamplingRate = 20000
		BitDepth = 20001
		Channels = 20002
		BitRate = 20003
		Runtime = 20004
	__all__.append('SoundResParams')


	class ListenerNodeParams(object):
		MasterGain = 20000
		DopplerFactor = 20001
		SpeedOfSound = 20002
	__all__.append('ListenerNodeParams')


	class SoundNodeParams(object):
		SoundRes = 20000
		Gain = 20001
		Pitch = 20002
		Offset = 20003
		Loop = 20004
		MaxDistance = 20005
		RolloffFactor = 20006
		ReferenceDistance = 20007
		MinGain = 20008
		MaxGain = 20009
	__all__.append('SoundNodeParams')


	class DistanceModels(object):
		None_ = 0
		InverseDistance = 1
		InverseDistanceClamped = 2
		LinearDistance = 3
		LinearDistanceClamped = 4
		ExponentDistance = 5
		ExponentDistanceClamped = 6
	__all__.append('DistanceModels')


	openDevice = h3d.openDevice
	openDevice.restype = c_bool
	openDevice.argtypes = [c_char_p]
	__all__.append('openDevice')


	closeDevice = h3d.closeDevice
	closeDevice.restype = None
	closeDevice.argtypes = []
	__all__.append('closeDevice')


	getOpenDevice = h3d.getOpenDevice
	getOpenDevice.restype = c_char_p
	getOpenDevice.argtypes = []
	__all__.append('getOpenDevice')


	queryDevice = h3d.queryDevice
	queryDevice.restype = c_char_p
	queryDevice.argtypes = [c_int]
	__all__.append('queryDevice')


	getDistanceModel = h3d.getDistanceModel
	getDistanceModel.restype = c_int
	getDistanceModel.argtypes = []
	__all__.append('getDistanceModel')


	setDistanceModel = h3d.setDistanceModel
	setDistanceModel.restype = c_bool
	setDistanceModel.argtypes = [c_int]
	__all__.append('setDistanceModel')


	addListenerNode = h3d.addListenerNode
	addListenerNode.restype = c_int
	addListenerNode.argtypes = [c_int, c_char_p]
	__all__.append('addListenerNode')


	getActiveListener = h3d.getActiveListener
	getActiveListener.restype = c_int
	getActiveListener.argtypes = []
	__all__.append('getActiveListener')


	setActiveListener = h3d.setActiveListener
	setActiveListener.restype = c_bool
	setActiveListener.argtypes = [c_int]
	__all__.append('setActiveListener')


	addSoundNode = h3d.addSoundNode
	addSoundNode.restype = c_int
	addSoundNode.argtypes = [c_int, c_char_p, c_int]
	__all__.append('addSoundNode')


	isSoundPlaying = h3d.isSoundPlaying
	isSoundPlaying.restype = c_bool
	isSoundPlaying.argtypes = [c_int]
	__all__.append('isSoundPlaying')


	playSound = h3d.playSound
	playSound.restype = None
	playSound.argtypes = [c_int]
	__all__.append('playSound')


	pauseSound = h3d.pauseSound
	pauseSound.restype = None
	pauseSound.argtypes = [c_int]
	__all__.append('pauseSound')


	rewindSound = h3d.rewindSound
	rewindSound.restype = None
	rewindSound.argtypes = [c_int]






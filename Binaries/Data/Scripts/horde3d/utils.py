# *************************************************************************************************
#
# Horde3D
#   Next-Generation Graphics Engine
# --------------------------------------
# Copyright (C) 2006-2009 Nicolas Schulz
#               2008-2009 Florian Noeding (Python wrapper)
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
import sys
if not 'c_bool' in globals():
	c_bool = c_int


__all__ = []
__platform = sys.platform
if __platform == 'darwin' :
    h3dutils = cdll.LoadLibrary('libHorde3DUtils.dylib')
elif __platform[:3] == 'win' :
    h3dutils = cdll.LoadLibrary('Horde3DUtils.dll')
else:
    h3dutils = cdll.LoadLibrary('libHorde3DUtils.so')

H3DUTMaxStatMode = 2

dumpMessages = h3dutils.h3dutDumpMessages
dumpMessages.restype = c_bool
dumpMessages.argtypes = []
__all__.append('dumpMessages')


getResourcePath = h3dutils.h3dutGetResourcePath
getResourcePath.restype = c_char_p
getResourcePath.argtypes = [c_int]
__all__.append('getResourcePath')


setResourcePath = h3dutils.h3dutSetResourcePath
setResourcePath.restype = None
setResourcePath.argtypes = [c_int, c_char_p]
__all__.append('setResourcePath')

screenshot = h3dutils.h3dutScreenshot
screenshot.restype = c_bool
screenshot.argtypes = [c_char_p]
__all__.append('screenshot')

loadResourcesFromDisk = h3dutils.h3dutLoadResourcesFromDisk
loadResourcesFromDisk.restype = c_bool
loadResourcesFromDisk.argtypes = [c_char_p]
__all__.append('loadResourcesFromDisk')


_pickRay = h3dutils.h3dutPickRay
_pickRay.restype = None
_pickRay.argtypes = [
		c_int,
		c_float, c_float,
		POINTER(c_float), POINTER(c_float), POINTER(c_float),
		POINTER(c_float), POINTER(c_float), POINTER(c_float),
		]
def pickRay(cameraNode, nwx, nwy):
	ox, oy, oz = c_float(), c_float(), c_float()
	dx, dy, dz = c_float(), c_float(), c_float()
	_pickRay(cameraNode,
			c_float(nwx), c_float(nwy),
			byref(ox), byref(oy), byref(oz),
			byref(dx), byref(dy), byref(dz),
			)
	return [[ox.value, oy.value, oz.value], [dx.value, dy.value, dz.value]]
__all__.append('pickRay')


_pickNode = h3dutils.h3dutPickNode
_pickNode.restype = c_int
_pickNode.argtypes = [c_int, c_float, c_float]
def pickNode(cameraNode, nwx, nwy):
	return _pickNode(cameraNode, c_float(nwx), c_float(nwy))
__all__.append('pickNode')


_showText = h3dutils.h3dutShowText
_showText.restype = None
_showText.argtypes = [c_char_p, c_float, c_float, c_float, c_float, c_float, c_float, c_int]
def showText(text, x, y, size, r, g, b, fontMaterialRes):
	return _showText(text, c_float(x), c_float(y), c_float(size), c_float(r), c_float(g), c_float(b), fontMaterialRes)
__all__.append('showText')


_showFrameStats = h3dutils.h3dutShowFrameStats
_showFrameStats.restype = None
_showFrameStats.argtypes = [c_int, c_int, c_int]
def showFrameStats(fontMaterialRes, boxMaterialRes, mode):
	return _showFrameStats(fontMaterialRes, boxMaterialRes, mode)
__all__.append('showFrameStats')

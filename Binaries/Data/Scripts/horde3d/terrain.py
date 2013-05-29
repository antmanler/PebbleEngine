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
    h3d = cdll.LoadLibrary('libHorde3D.dylib')
elif __platform[:3] == 'win' :
    h3d = cdll.LoadLibrary('Horde3D.dll')
else:
    h3d = cdll.LoadLibrary('libHorde3D.so')

if hasattr(h3d, 'h3dextAddTerrainNode'):
	NodeType_Terrain = 100
	__all__.append('NodeType_Terrain')

	class TerrainNodeParams(object):
		HeightTexResI = 10000
		MatResI = 10001
		MeshQualityF = 10002
		SkirtHeightF = 10003
		BlockSizeI = 10004
	__all__.append('Terrain')


	addTerrainNode = h3d.h3dextAddTerrainNode
	addTerrainNode.restype = c_int
	addTerrainNode.argtypes = [c_int, c_char_p, c_int, c_int]
	__all__.append('addTerrainNode')

	_createTerrainGeoRes = h3d.h3dextCreateTerrainGeoRes
	_createTerrainGeoRes.restype = c_int
	_createTerrainGeoRes.argtypes = [c_int, c_char_p, c_float]
	def createTerrainGeoRes(node, resName, meshQuality):
		return _createTerrainGeoRes(node, resName, c_float(meshQuality))
	__all__.append('createTerrainGeoRes')





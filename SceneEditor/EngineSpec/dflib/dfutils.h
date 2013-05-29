// ****************************************************************************************
//
// Pebble Scene Editor
// --------------------------------------
// Copyright (C) 2011 Antmanler
//
// This file is part of the Pebble Scene Editor.
//
// The Pebble Scene Editor is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License
//
// The Pebble Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Credits : Volker Wiendl who created the major work of the original editor and Horde3D
// is a flexible graphics engine. see <http://www.horde3d.org> and <http://http://goo.gl/Me2HT>
//
// ****************************************************************************************

#pragma once

#include "mesh.h"

namespace dfut
{

/**
  * \brief convert horde3d geometry to mesh and append it to given mesh object
  * @param hid the horde3d id of a model or terrain
  * @param mesh which appened to
  */
void appendH3DGeomToMesh(int hid, Mesh &mesh);
}

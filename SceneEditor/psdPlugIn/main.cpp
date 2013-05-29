// ****************************************************************************************
//
// Horde3D Scene Editor 
// --------------------------------------
// Copyright (C) 2007 Volker Wiendl
// 
// This file is part of the Horde3D Scene Editor.
// The code is based on the psd loader code by Thatcher Ulrich and the Qt tga loader plugin
// by Dominik Seichter and Ignacio Casta�o 
//
// The Horde3D Scene Editor is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License 
//
// The Horde3D Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ****************************************************************************************

#include <QtGui/QImageIOHandler>
#include <QtCore/QStringList>

#ifndef QT_NO_IMAGEFORMATPLUGIN

#ifdef QT_NO_IMAGEFORMAT_PSD
#undef QT_NO_IMAGEFORMAT_PSD
#endif
#include "qpsdhandler.h"

class QPsdPlugin : public QImageIOPlugin
{
public:
    QStringList keys() const;
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;
};

QStringList QPsdPlugin::keys() const
{
    return QStringList() << "psd" << "psd";
}

QImageIOPlugin::Capabilities QPsdPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "psd" || format == "psd")
        return Capabilities(CanRead /*| CanWrite*/);
    if (!format.isEmpty())
        return 0;
    if (!device->isOpen())
        return 0;

    Capabilities cap;
    if (device->isReadable() && QPsdHandler::canRead(device))
        cap |= CanRead;
    //if (device->isWritable())
    //    cap |= CanWrite;
    return cap;
}

QImageIOHandler *QPsdPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new QPsdHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}

Q_EXPORT_STATIC_PLUGIN(QPsdPlugin)
Q_EXPORT_PLUGIN2(qpsd, QPsdPlugin)

#endif // QT_NO_IMAGEFORMATPLUGIN

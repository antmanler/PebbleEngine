#include <PythonQt.h>
#include <QObject>
#include <QVariant>
#include <audiooutput.h>
#include <effectparameter.h>
#include <objectdescription.h>
#include <qaction.h>
#include <qbitmap.h>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qcursor.h>
#include <qevent.h>
#include <qfont.h>
#include <qgraphicseffect.h>
#include <qgraphicsproxywidget.h>
#include <qicon.h>
#include <qinputcontext.h>
#include <qkeysequence.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlocale.h>
#include <qmargins.h>
#include <qobject.h>
#include <qpaintdevice.h>
#include <qpaintengine.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpoint.h>
#include <qrect.h>
#include <qregion.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstyle.h>
#include <qwidget.h>
#include <volumefadereffect.h>
#include <volumeslider.h>



class PythonQtShell_VolumeFaderEffect : public Phonon::VolumeFaderEffect
{
public:
    PythonQtShell_VolumeFaderEffect(QObject*  parent = 0):Phonon::VolumeFaderEffect(parent),_wrapper(NULL) {};

virtual void childEvent(QChildEvent*  arg__1);
virtual void customEvent(QEvent*  arg__1);
virtual bool  event(QEvent*  arg__1);
virtual bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
virtual void timerEvent(QTimerEvent*  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_VolumeFaderEffect : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::VolumeFaderEffect* new_VolumeFaderEffect(QObject*  parent = 0);
void delete_VolumeFaderEffect(Phonon::VolumeFaderEffect* obj) { delete obj; } 
   Phonon::VolumeFaderEffect::FadeCurve  fadeCurve(Phonon::VolumeFaderEffect* theWrappedObject) const;
   float  volume(Phonon::VolumeFaderEffect* theWrappedObject) const;
   double  volumeDecibel(Phonon::VolumeFaderEffect* theWrappedObject) const;
};





class PythonQtShell_VolumeSlider : public Phonon::VolumeSlider
{
public:
    PythonQtShell_VolumeSlider(Phonon::AudioOutput*  arg__1, QWidget*  parent = 0):Phonon::VolumeSlider(arg__1, parent),_wrapper(NULL) {};
    PythonQtShell_VolumeSlider(QWidget*  parent = 0):Phonon::VolumeSlider(parent),_wrapper(NULL) {};

virtual void actionEvent(QActionEvent*  arg__1);
virtual void changeEvent(QEvent*  arg__1);
virtual void childEvent(QChildEvent*  arg__1);
virtual void closeEvent(QCloseEvent*  arg__1);
virtual void contextMenuEvent(QContextMenuEvent*  arg__1);
virtual void customEvent(QEvent*  arg__1);
virtual int  devType() const;
virtual void dragEnterEvent(QDragEnterEvent*  arg__1);
virtual void dragLeaveEvent(QDragLeaveEvent*  arg__1);
virtual void dragMoveEvent(QDragMoveEvent*  arg__1);
virtual void dropEvent(QDropEvent*  arg__1);
virtual void enterEvent(QEvent*  arg__1);
virtual bool  event(QEvent*  arg__1);
virtual bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
virtual void focusInEvent(QFocusEvent*  arg__1);
virtual bool  focusNextPrevChild(bool  next);
virtual void focusOutEvent(QFocusEvent*  arg__1);
virtual int  heightForWidth(int  arg__1) const;
virtual void hideEvent(QHideEvent*  arg__1);
virtual void inputMethodEvent(QInputMethodEvent*  arg__1);
virtual QVariant  inputMethodQuery(Qt::InputMethodQuery  arg__1) const;
virtual void keyPressEvent(QKeyEvent*  arg__1);
virtual void keyReleaseEvent(QKeyEvent*  arg__1);
virtual void languageChange();
virtual void leaveEvent(QEvent*  arg__1);
virtual int  metric(QPaintDevice::PaintDeviceMetric  arg__1) const;
virtual QSize  minimumSizeHint() const;
virtual void mouseDoubleClickEvent(QMouseEvent*  arg__1);
virtual void mouseMoveEvent(QMouseEvent*  arg__1);
virtual void mousePressEvent(QMouseEvent*  arg__1);
virtual void mouseReleaseEvent(QMouseEvent*  arg__1);
virtual void moveEvent(QMoveEvent*  arg__1);
virtual QPaintEngine*  paintEngine() const;
virtual void paintEvent(QPaintEvent*  arg__1);
virtual void resizeEvent(QResizeEvent*  arg__1);
virtual void showEvent(QShowEvent*  arg__1);
virtual QSize  sizeHint() const;
virtual void tabletEvent(QTabletEvent*  arg__1);
virtual void timerEvent(QTimerEvent*  arg__1);
virtual void wheelEvent(QWheelEvent*  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_VolumeSlider : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::VolumeSlider* new_VolumeSlider(Phonon::AudioOutput*  arg__1, QWidget*  parent = 0);
Phonon::VolumeSlider* new_VolumeSlider(QWidget*  parent = 0);
void delete_VolumeSlider(Phonon::VolumeSlider* obj) { delete obj; } 
   Phonon::AudioOutput*  audioOutput(Phonon::VolumeSlider* theWrappedObject) const;
   bool  hasTracking(Phonon::VolumeSlider* theWrappedObject) const;
   QSize  iconSize(Phonon::VolumeSlider* theWrappedObject) const;
   bool  isMuteVisible(Phonon::VolumeSlider* theWrappedObject) const;
   qreal  maximumVolume(Phonon::VolumeSlider* theWrappedObject) const;
   Qt::Orientation  orientation(Phonon::VolumeSlider* theWrappedObject) const;
   int  pageStep(Phonon::VolumeSlider* theWrappedObject) const;
   void setPageStep(Phonon::VolumeSlider* theWrappedObject, int  milliseconds);
   void setSingleStep(Phonon::VolumeSlider* theWrappedObject, int  milliseconds);
   void setTracking(Phonon::VolumeSlider* theWrappedObject, bool  tracking);
   int  singleStep(Phonon::VolumeSlider* theWrappedObject) const;
};



#include <PythonQt.h>
#include <QObject>
#include <QVariant>
#include <abstractaudiooutput.h>
#include <abstractmediastream.h>
#include <abstractvideooutput.h>
#include <addoninterface.h>
#include <audiooutput.h>
#include <backendcapabilities.h>
#include <backendinterface.h>
#include <effect.h>
#include <effectinterface.h>
#include <effectparameter.h>
#include <effectwidget.h>
#include <mediacontroller.h>
#include <medianode.h>
#include <mediaobject.h>
#include <mediaobjectinterface.h>
#include <mediasource.h>
#include <objectdescription.h>
#include <path.h>
#include <platformplugin.h>
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
#include <qimage.h>
#include <qinputcontext.h>
#include <qiodevice.h>
#include <qkeysequence.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlocale.h>
#include <qmargins.h>
#include <qobject.h>
#include <qpaintdevice.h>
#include <qpaintengine.h>
#include <qpainter.h>
#include <qpair.h>
#include <qpalette.h>
#include <qpoint.h>
#include <qrect.h>
#include <qregion.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstringlist.h>
#include <qstyle.h>
#include <qurl.h>
#include <qwidget.h>
#include <seekslider.h>
#include <streaminterface.h>
#include <videoplayer.h>
#include <videowidget.h>
#include <videowidgetinterface.h>
#include <volumefaderinterface.h>



class PythonQtShell_AbstractAddon : public Phonon::AddonInterface
{
public:
    PythonQtShell_AbstractAddon():Phonon::AddonInterface(),_wrapper(NULL) {};

virtual bool  hasInterface(Phonon::AddonInterface::Interface  iface) const;
virtual QVariant  interfaceCall(Phonon::AddonInterface::Interface  iface, int  command, const QList<QVariant >&  arguments = QList<QVariant>());

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_AbstractAddon : public QObject
{ Q_OBJECT
public:
Q_ENUMS(SubtitleCommand TitleCommand Interface AudioChannelCommand ChapterCommand NavigationCommand AngleCommand )
enum SubtitleCommand{
  availableSubtitles = Phonon::AddonInterface::availableSubtitles,   currentSubtitle = Phonon::AddonInterface::currentSubtitle,   setCurrentSubtitle = Phonon::AddonInterface::setCurrentSubtitle};
enum TitleCommand{
  availableTitles = Phonon::AddonInterface::availableTitles,   title = Phonon::AddonInterface::title,   setTitle = Phonon::AddonInterface::setTitle,   autoplayTitles = Phonon::AddonInterface::autoplayTitles,   setAutoplayTitles = Phonon::AddonInterface::setAutoplayTitles};
enum Interface{
  NavigationInterface = Phonon::AddonInterface::NavigationInterface,   ChapterInterface = Phonon::AddonInterface::ChapterInterface,   AngleInterface = Phonon::AddonInterface::AngleInterface,   TitleInterface = Phonon::AddonInterface::TitleInterface,   SubtitleInterface = Phonon::AddonInterface::SubtitleInterface,   AudioChannelInterface = Phonon::AddonInterface::AudioChannelInterface};
enum AudioChannelCommand{
  availableAudioChannels = Phonon::AddonInterface::availableAudioChannels,   currentAudioChannel = Phonon::AddonInterface::currentAudioChannel,   setCurrentAudioChannel = Phonon::AddonInterface::setCurrentAudioChannel};
enum ChapterCommand{
  availableChapters = Phonon::AddonInterface::availableChapters,   chapter = Phonon::AddonInterface::chapter,   setChapter = Phonon::AddonInterface::setChapter};
enum NavigationCommand{
  Menu1Button = Phonon::AddonInterface::Menu1Button};
enum AngleCommand{
  availableAngles = Phonon::AddonInterface::availableAngles,   angle = Phonon::AddonInterface::angle,   setAngle = Phonon::AddonInterface::setAngle};
public slots:
Phonon::AddonInterface* new_AbstractAddon();
void delete_AbstractAddon(Phonon::AddonInterface* obj) { delete obj; } 
};





class PythonQtShell_AbstractAudioOutput : public Phonon::AbstractAudioOutput
{
public:

virtual void childEvent(QChildEvent*  arg__1);
virtual void customEvent(QEvent*  arg__1);
virtual bool  event(QEvent*  arg__1);
virtual bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
virtual void timerEvent(QTimerEvent*  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_AbstractAudioOutput : public QObject
{ Q_OBJECT
public:
public slots:
void delete_AbstractAudioOutput(Phonon::AbstractAudioOutput* obj) { delete obj; } 
};





class PythonQtShell_AbstractBackend : public Phonon::BackendInterface
{
public:
    PythonQtShell_AbstractBackend():Phonon::BackendInterface(),_wrapper(NULL) {};

virtual QStringList  availableMimeTypes() const;
virtual bool  connectNodes(QObject*  arg__1, QObject*  arg__2);
virtual QObject*  createObject(Phonon::BackendInterface::Class  c, QObject*  parent, const QList<QVariant >&  args = QList<QVariant>());
virtual bool  disconnectNodes(QObject*  arg__1, QObject*  arg__2);
virtual bool  endConnectionChange(QSet<QObject* >  arg__1);
virtual QList<int >  objectDescriptionIndexes(Phonon::ObjectDescriptionType  type) const;
virtual QHash<QByteArray , QVariant >  objectDescriptionProperties(Phonon::ObjectDescriptionType  type, int  index) const;
virtual bool  startConnectionChange(QSet<QObject* >  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_AbstractBackend : public QObject
{ Q_OBJECT
public:
Q_ENUMS(Class )
enum Class{
  MediaObjectClass = Phonon::BackendInterface::MediaObjectClass,   VolumeFaderEffectClass = Phonon::BackendInterface::VolumeFaderEffectClass,   AudioOutputClass = Phonon::BackendInterface::AudioOutputClass,   AudioDataOutputClass = Phonon::BackendInterface::AudioDataOutputClass,   VisualizationClass = Phonon::BackendInterface::VisualizationClass,   VideoDataOutputClass = Phonon::BackendInterface::VideoDataOutputClass,   EffectClass = Phonon::BackendInterface::EffectClass,   VideoWidgetClass = Phonon::BackendInterface::VideoWidgetClass};
public slots:
Phonon::BackendInterface* new_AbstractBackend();
void delete_AbstractBackend(Phonon::BackendInterface* obj) { delete obj; } 
};





class PythonQtShell_AbstractEffect : public Phonon::EffectInterface
{
public:
    PythonQtShell_AbstractEffect():Phonon::EffectInterface(),_wrapper(NULL) {};

virtual QVariant  parameterValue(const Phonon::EffectParameter&  arg__1) const;
virtual QList<Phonon::EffectParameter >  parameters() const;
virtual void setParameterValue(const Phonon::EffectParameter&  arg__1, const QVariant&  newValue);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_AbstractEffect : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::EffectInterface* new_AbstractEffect();
void delete_AbstractEffect(Phonon::EffectInterface* obj) { delete obj; } 
};





class PythonQtShell_AbstractMediaObject : public Phonon::MediaObjectInterface
{
public:
    PythonQtShell_AbstractMediaObject():Phonon::MediaObjectInterface(),_wrapper(NULL) {};

virtual qint64  currentTime() const;
virtual QString  errorString() const;
virtual Phonon::ErrorType  errorType() const;
virtual bool  hasVideo() const;
virtual bool  isSeekable() const;
virtual void pause();
virtual void play();
virtual int  prefinishMark() const;
virtual qint64  remainingTime() const;
virtual void seek(qint64  milliseconds);
virtual void setNextSource(const Phonon::MediaSource&  source);
virtual void setPrefinishMark(int  arg__1);
virtual void setSource(const Phonon::MediaSource&  arg__1);
virtual void setTickInterval(int  interval);
virtual void setTransitionTime(int  arg__1);
virtual Phonon::MediaSource  source() const;
virtual Phonon::State  state() const;
virtual void stop();
virtual int  tickInterval() const;
virtual qint64  totalTime() const;
virtual int  transitionTime() const;

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtPublicPromoter_AbstractMediaObject : public Phonon::MediaObjectInterface
{ public:
inline qint64  promoted_remainingTime() const { return Phonon::MediaObjectInterface::remainingTime(); }
};

class PythonQtWrapper_AbstractMediaObject : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::MediaObjectInterface* new_AbstractMediaObject();
void delete_AbstractMediaObject(Phonon::MediaObjectInterface* obj) { delete obj; } 
   qint64  remainingTime(Phonon::MediaObjectInterface* theWrappedObject) const;
};





class PythonQtShell_AbstractMediaStream : public Phonon::AbstractMediaStream
{
public:
    PythonQtShell_AbstractMediaStream(QObject*  parent = 0):Phonon::AbstractMediaStream(parent),_wrapper(NULL) {};

virtual void childEvent(QChildEvent*  arg__1);
virtual void customEvent(QEvent*  arg__1);
virtual void enoughData();
virtual bool  event(QEvent*  arg__1);
virtual bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
virtual void needData();
virtual void reset();
virtual void seekStream(qint64  offset);
virtual void timerEvent(QTimerEvent*  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtPublicPromoter_AbstractMediaStream : public Phonon::AbstractMediaStream
{ public:
inline void promoted_enoughData() { Phonon::AbstractMediaStream::enoughData(); }
inline void promoted_seekStream(qint64  offset) { Phonon::AbstractMediaStream::seekStream(offset); }
};

class PythonQtWrapper_AbstractMediaStream : public QObject
{ Q_OBJECT
public:
public slots:
void delete_AbstractMediaStream(Phonon::AbstractMediaStream* obj) { delete obj; } 
   void enoughData(Phonon::AbstractMediaStream* theWrappedObject);
   void seekStream(Phonon::AbstractMediaStream* theWrappedObject, qint64  offset);
};





class PythonQtShell_AbstractStream : public Phonon::StreamInterface
{
public:
    PythonQtShell_AbstractStream():Phonon::StreamInterface(),_wrapper(NULL) {};

virtual void endOfData();
virtual void setStreamSeekable(bool  s);
virtual void setStreamSize(qint64  newSize);
virtual void writeData(const QByteArray&  data);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_AbstractStream : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::StreamInterface* new_AbstractStream();
void delete_AbstractStream(Phonon::StreamInterface* obj) { delete obj; } 
   void connectToSource(Phonon::StreamInterface* theWrappedObject, const Phonon::MediaSource&  mediaSource);
   void enoughData(Phonon::StreamInterface* theWrappedObject);
   void needData(Phonon::StreamInterface* theWrappedObject);
   void reset(Phonon::StreamInterface* theWrappedObject);
   void seekStream(Phonon::StreamInterface* theWrappedObject, qint64  seekTo);
};





class PythonQtWrapper_AbstractVideoOutput : public QObject
{ Q_OBJECT
public:
public slots:
void delete_AbstractVideoOutput(Phonon::AbstractVideoOutput* obj) { delete obj; } 
};





class PythonQtShell_AbstractVideoWidget : public Phonon::VideoWidgetInterface
{
public:
    PythonQtShell_AbstractVideoWidget():Phonon::VideoWidgetInterface(),_wrapper(NULL) {};

virtual Phonon::VideoWidget::AspectRatio  aspectRatio() const;
virtual qreal  brightness() const;
virtual qreal  contrast() const;
virtual qreal  hue() const;
virtual qreal  saturation() const;
virtual Phonon::VideoWidget::ScaleMode  scaleMode() const;
virtual void setAspectRatio(Phonon::VideoWidget::AspectRatio  arg__1);
virtual void setBrightness(qreal  arg__1);
virtual void setContrast(qreal  arg__1);
virtual void setHue(qreal  arg__1);
virtual void setSaturation(qreal  arg__1);
virtual void setScaleMode(Phonon::VideoWidget::ScaleMode  arg__1);
virtual QWidget*  widget();

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_AbstractVideoWidget : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::VideoWidgetInterface* new_AbstractVideoWidget();
void delete_AbstractVideoWidget(Phonon::VideoWidgetInterface* obj) { delete obj; } 
};





class PythonQtShell_AbstractVolumeFader : public Phonon::VolumeFaderInterface
{
public:
    PythonQtShell_AbstractVolumeFader():Phonon::VolumeFaderInterface(),_wrapper(NULL) {};

virtual Phonon::VolumeFaderEffect::FadeCurve  fadeCurve() const;
virtual void fadeTo(float  arg__1, int  arg__2);
virtual void setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve  arg__1);
virtual void setVolume(float  arg__1);
virtual float  volume() const;

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtPublicPromoter_AbstractVolumeFader : public Phonon::VolumeFaderInterface
{ public:
inline Phonon::VolumeFaderEffect::FadeCurve  promoted_fadeCurve() const { return Phonon::VolumeFaderInterface::fadeCurve(); }
inline void promoted_fadeTo(float  arg__1, int  arg__2) { Phonon::VolumeFaderInterface::fadeTo(arg__1, arg__2); }
inline void promoted_setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve  arg__1) { Phonon::VolumeFaderInterface::setFadeCurve(arg__1); }
inline void promoted_setVolume(float  arg__1) { Phonon::VolumeFaderInterface::setVolume(arg__1); }
inline float  promoted_volume() const { return Phonon::VolumeFaderInterface::volume(); }
};

class PythonQtWrapper_AbstractVolumeFader : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::VolumeFaderInterface* new_AbstractVolumeFader();
void delete_AbstractVolumeFader(Phonon::VolumeFaderInterface* obj) { delete obj; } 
   Phonon::VolumeFaderEffect::FadeCurve  fadeCurve(Phonon::VolumeFaderInterface* theWrappedObject) const;
   void fadeTo(Phonon::VolumeFaderInterface* theWrappedObject, float  arg__1, int  arg__2);
   void setFadeCurve(Phonon::VolumeFaderInterface* theWrappedObject, Phonon::VolumeFaderEffect::FadeCurve  arg__1);
   void setVolume(Phonon::VolumeFaderInterface* theWrappedObject, float  arg__1);
   float  volume(Phonon::VolumeFaderInterface* theWrappedObject) const;
};





class PythonQtWrapper_AudioCaptureDevice : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::AudioCaptureDevice* new_AudioCaptureDevice();
Phonon::AudioCaptureDevice* new_AudioCaptureDevice(int  index, const QHash<QByteArray , QVariant >&  properties);
Phonon::AudioCaptureDevice* new_AudioCaptureDevice(const Phonon::AudioCaptureDevice& other) {
Phonon::AudioCaptureDevice* a = new Phonon::AudioCaptureDevice();
*((Phonon::AudioCaptureDevice*)a) = other;
return a; }
void delete_AudioCaptureDevice(Phonon::AudioCaptureDevice* obj) { delete obj; } 
   QString  description(Phonon::AudioCaptureDevice* theWrappedObject) const;
   int  index(Phonon::AudioCaptureDevice* theWrappedObject) const;
   bool  isValid(Phonon::AudioCaptureDevice* theWrappedObject) const;
   QString  name(Phonon::AudioCaptureDevice* theWrappedObject) const;
   QVariant  property(Phonon::AudioCaptureDevice* theWrappedObject, const char*  name) const;
   QList<QByteArray >  propertyNames(Phonon::AudioCaptureDevice* theWrappedObject) const;
};





class PythonQtWrapper_AudioChannelDescription : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::AudioChannelDescription* new_AudioChannelDescription();
Phonon::AudioChannelDescription* new_AudioChannelDescription(int  index, const QHash<QByteArray , QVariant >&  properties);
Phonon::AudioChannelDescription* new_AudioChannelDescription(const Phonon::AudioChannelDescription& other) {
Phonon::AudioChannelDescription* a = new Phonon::AudioChannelDescription();
*((Phonon::AudioChannelDescription*)a) = other;
return a; }
void delete_AudioChannelDescription(Phonon::AudioChannelDescription* obj) { delete obj; } 
   QString  description(Phonon::AudioChannelDescription* theWrappedObject) const;
   int  index(Phonon::AudioChannelDescription* theWrappedObject) const;
   bool  isValid(Phonon::AudioChannelDescription* theWrappedObject) const;
   QString  name(Phonon::AudioChannelDescription* theWrappedObject) const;
   QVariant  property(Phonon::AudioChannelDescription* theWrappedObject, const char*  name) const;
   QList<QByteArray >  propertyNames(Phonon::AudioChannelDescription* theWrappedObject) const;
};





class PythonQtShell_AudioOutput : public Phonon::AudioOutput
{
public:
    PythonQtShell_AudioOutput(Phonon::Category  category, QObject*  parent = 0):Phonon::AudioOutput(category, parent),_wrapper(NULL) {};
    PythonQtShell_AudioOutput(QObject*  parent = 0):Phonon::AudioOutput(parent),_wrapper(NULL) {};

virtual void childEvent(QChildEvent*  arg__1);
virtual void customEvent(QEvent*  arg__1);
virtual bool  event(QEvent*  arg__1);
virtual bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
virtual void timerEvent(QTimerEvent*  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_AudioOutput : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::AudioOutput* new_AudioOutput(Phonon::Category  category, QObject*  parent = 0);
Phonon::AudioOutput* new_AudioOutput(QObject*  parent = 0);
void delete_AudioOutput(Phonon::AudioOutput* obj) { delete obj; } 
   Phonon::Category  category(Phonon::AudioOutput* theWrappedObject) const;
   bool  isMuted(Phonon::AudioOutput* theWrappedObject) const;
   QString  name(Phonon::AudioOutput* theWrappedObject) const;
   Phonon::AudioOutputDevice  outputDevice(Phonon::AudioOutput* theWrappedObject) const;
   qreal  volume(Phonon::AudioOutput* theWrappedObject) const;
   qreal  volumeDecibel(Phonon::AudioOutput* theWrappedObject) const;
};





class PythonQtWrapper_AudioOutputDevice : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::AudioOutputDevice* new_AudioOutputDevice();
Phonon::AudioOutputDevice* new_AudioOutputDevice(int  index, const QHash<QByteArray , QVariant >&  properties);
Phonon::AudioOutputDevice* new_AudioOutputDevice(const Phonon::AudioOutputDevice& other) {
Phonon::AudioOutputDevice* a = new Phonon::AudioOutputDevice();
*((Phonon::AudioOutputDevice*)a) = other;
return a; }
void delete_AudioOutputDevice(Phonon::AudioOutputDevice* obj) { delete obj; } 
   QString  description(Phonon::AudioOutputDevice* theWrappedObject) const;
   int  index(Phonon::AudioOutputDevice* theWrappedObject) const;
   bool  isValid(Phonon::AudioOutputDevice* theWrappedObject) const;
   QString  name(Phonon::AudioOutputDevice* theWrappedObject) const;
   QVariant  property(Phonon::AudioOutputDevice* theWrappedObject, const char*  name) const;
   QList<QByteArray >  propertyNames(Phonon::AudioOutputDevice* theWrappedObject) const;
};





class PythonQtWrapper_BackendCapabilities : public QObject
{ Q_OBJECT
public:
public slots:
   QList<Phonon::AudioCaptureDevice >  static_BackendCapabilities_availableAudioCaptureDevices();
   QList<Phonon::EffectDescription >  static_BackendCapabilities_availableAudioEffects();
   QList<Phonon::AudioOutputDevice >  static_BackendCapabilities_availableAudioOutputDevices();
   QStringList  static_BackendCapabilities_availableMimeTypes();
   bool  static_BackendCapabilities_isMimeTypeAvailable(const QString&  mimeType);
};





class PythonQtShell_Effect : public Phonon::Effect
{
public:
    PythonQtShell_Effect(const Phonon::EffectDescription&  description, QObject*  parent = 0):Phonon::Effect(description, parent),_wrapper(NULL) {};

virtual void childEvent(QChildEvent*  arg__1);
virtual void customEvent(QEvent*  arg__1);
virtual bool  event(QEvent*  arg__1);
virtual bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
virtual void timerEvent(QTimerEvent*  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_Effect : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::Effect* new_Effect(const Phonon::EffectDescription&  description, QObject*  parent = 0);
void delete_Effect(Phonon::Effect* obj) { delete obj; } 
   Phonon::EffectDescription  description(Phonon::Effect* theWrappedObject) const;
   QVariant  parameterValue(Phonon::Effect* theWrappedObject, const Phonon::EffectParameter&  arg__1) const;
   QList<Phonon::EffectParameter >  parameters(Phonon::Effect* theWrappedObject) const;
   void setParameterValue(Phonon::Effect* theWrappedObject, const Phonon::EffectParameter&  arg__1, const QVariant&  value);
};





class PythonQtWrapper_EffectDescription : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::EffectDescription* new_EffectDescription();
Phonon::EffectDescription* new_EffectDescription(int  index, const QHash<QByteArray , QVariant >&  properties);
Phonon::EffectDescription* new_EffectDescription(const Phonon::EffectDescription& other) {
Phonon::EffectDescription* a = new Phonon::EffectDescription();
*((Phonon::EffectDescription*)a) = other;
return a; }
void delete_EffectDescription(Phonon::EffectDescription* obj) { delete obj; } 
   QString  description(Phonon::EffectDescription* theWrappedObject) const;
   int  index(Phonon::EffectDescription* theWrappedObject) const;
   bool  isValid(Phonon::EffectDescription* theWrappedObject) const;
   QString  name(Phonon::EffectDescription* theWrappedObject) const;
   QVariant  property(Phonon::EffectDescription* theWrappedObject, const char*  name) const;
   QList<QByteArray >  propertyNames(Phonon::EffectDescription* theWrappedObject) const;
};





class PythonQtWrapper_EffectParameter : public QObject
{ Q_OBJECT
public:
Q_ENUMS(Hint )
Q_FLAGS(Hints )
enum Hint{
  ToggledHint = Phonon::EffectParameter::ToggledHint,   LogarithmicHint = Phonon::EffectParameter::LogarithmicHint,   IntegerHint = Phonon::EffectParameter::IntegerHint};
Q_DECLARE_FLAGS(Hints, Hint)
public slots:
Phonon::EffectParameter* new_EffectParameter(int  parameterId, const QString&  name, Phonon::EffectParameter::Hints  hints, const QVariant&  defaultValue, const QVariant&  min = QVariant(), const QVariant&  max = QVariant(), const QList<QVariant >&  values = QVariantList(), const QString&  description = QString());
void delete_EffectParameter(Phonon::EffectParameter* obj) { delete obj; } 
   QVariant  defaultValue(Phonon::EffectParameter* theWrappedObject) const;
   const QString*  description(Phonon::EffectParameter* theWrappedObject) const;
   bool  isLogarithmicControl(Phonon::EffectParameter* theWrappedObject) const;
   QVariant  maximumValue(Phonon::EffectParameter* theWrappedObject) const;
   QVariant  minimumValue(Phonon::EffectParameter* theWrappedObject) const;
   const QString*  name(Phonon::EffectParameter* theWrappedObject) const;
   QList<QVariant >  possibleValues(Phonon::EffectParameter* theWrappedObject) const;
   QVariant::Type  type(Phonon::EffectParameter* theWrappedObject) const;
};





class PythonQtShell_EffectWidget : public Phonon::EffectWidget
{
public:
    PythonQtShell_EffectWidget(Phonon::Effect*  effect, QWidget*  parent = 0):Phonon::EffectWidget(effect, parent),_wrapper(NULL) {};

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

class PythonQtWrapper_EffectWidget : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::EffectWidget* new_EffectWidget(Phonon::Effect*  effect, QWidget*  parent = 0);
void delete_EffectWidget(Phonon::EffectWidget* obj) { delete obj; } 
};





class PythonQtShell_MediaController : public Phonon::MediaController
{
public:
    PythonQtShell_MediaController(Phonon::MediaObject*  parent):Phonon::MediaController(parent),_wrapper(NULL) {};

virtual void childEvent(QChildEvent*  arg__1);
virtual void customEvent(QEvent*  arg__1);
virtual bool  event(QEvent*  arg__1);
virtual bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
virtual void timerEvent(QTimerEvent*  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_MediaController : public QObject
{ Q_OBJECT
public:
Q_ENUMS(Feature )
Q_FLAGS(Features )
enum Feature{
  Angles = Phonon::MediaController::Angles,   Chapters = Phonon::MediaController::Chapters,   Titles = Phonon::MediaController::Titles};
Q_DECLARE_FLAGS(Features, Feature)
public slots:
Phonon::MediaController* new_MediaController(Phonon::MediaObject*  parent);
void delete_MediaController(Phonon::MediaController* obj) { delete obj; } 
   bool  autoplayTitles(Phonon::MediaController* theWrappedObject) const;
   int  availableAngles(Phonon::MediaController* theWrappedObject) const;
   QList<Phonon::AudioChannelDescription >  availableAudioChannels(Phonon::MediaController* theWrappedObject) const;
   int  availableChapters(Phonon::MediaController* theWrappedObject) const;
   QList<Phonon::SubtitleDescription >  availableSubtitles(Phonon::MediaController* theWrappedObject) const;
   int  availableTitles(Phonon::MediaController* theWrappedObject) const;
   int  currentAngle(Phonon::MediaController* theWrappedObject) const;
   Phonon::AudioChannelDescription  currentAudioChannel(Phonon::MediaController* theWrappedObject) const;
   int  currentChapter(Phonon::MediaController* theWrappedObject) const;
   Phonon::SubtitleDescription  currentSubtitle(Phonon::MediaController* theWrappedObject) const;
   int  currentTitle(Phonon::MediaController* theWrappedObject) const;
   void setCurrentAudioChannel(Phonon::MediaController* theWrappedObject, const Phonon::AudioChannelDescription&  stream);
   void setCurrentSubtitle(Phonon::MediaController* theWrappedObject, const Phonon::SubtitleDescription&  stream);
   Phonon::MediaController::Features  supportedFeatures(Phonon::MediaController* theWrappedObject) const;
};





class PythonQtShell_MediaNode : public Phonon::MediaNode
{
public:


  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_MediaNode : public QObject
{ Q_OBJECT
public:
public slots:
void delete_MediaNode(Phonon::MediaNode* obj) { delete obj; } 
   QList<Phonon::Path >  inputPaths(Phonon::MediaNode* theWrappedObject) const;
   bool  isValid(Phonon::MediaNode* theWrappedObject) const;
   QList<Phonon::Path >  outputPaths(Phonon::MediaNode* theWrappedObject) const;
};





class PythonQtShell_MediaObject : public Phonon::MediaObject
{
public:
    PythonQtShell_MediaObject(QObject*  parent = 0):Phonon::MediaObject(parent),_wrapper(NULL) {};

virtual void childEvent(QChildEvent*  arg__1);
virtual void customEvent(QEvent*  arg__1);
virtual bool  event(QEvent*  arg__1);
virtual bool  eventFilter(QObject*  arg__1, QEvent*  arg__2);
virtual void timerEvent(QTimerEvent*  arg__1);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtWrapper_MediaObject : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::MediaObject* new_MediaObject(QObject*  parent = 0);
void delete_MediaObject(Phonon::MediaObject* obj) { delete obj; } 
   void clearQueue(Phonon::MediaObject* theWrappedObject);
   Phonon::MediaSource  currentSource(Phonon::MediaObject* theWrappedObject) const;
   qint64  currentTime(Phonon::MediaObject* theWrappedObject) const;
   void enqueue(Phonon::MediaObject* theWrappedObject, const Phonon::MediaSource&  source);
   void enqueue(Phonon::MediaObject* theWrappedObject, const QList<Phonon::MediaSource >&  sources);
   void enqueue(Phonon::MediaObject* theWrappedObject, const QList<QUrl >&  urls);
   QString  errorString(Phonon::MediaObject* theWrappedObject) const;
   Phonon::ErrorType  errorType(Phonon::MediaObject* theWrappedObject) const;
   bool  hasVideo(Phonon::MediaObject* theWrappedObject) const;
   bool  isSeekable(Phonon::MediaObject* theWrappedObject) const;
   QMultiMap<QString , QString >  metaData(Phonon::MediaObject* theWrappedObject) const;
   QStringList  metaData(Phonon::MediaObject* theWrappedObject, Phonon::MetaData  key) const;
   QStringList  metaData(Phonon::MediaObject* theWrappedObject, const QString&  key) const;
   int  prefinishMark(Phonon::MediaObject* theWrappedObject) const;
   QList<Phonon::MediaSource >  queue(Phonon::MediaObject* theWrappedObject) const;
   qint64  remainingTime(Phonon::MediaObject* theWrappedObject) const;
   void setCurrentSource(Phonon::MediaObject* theWrappedObject, const Phonon::MediaSource&  source);
   void setPrefinishMark(Phonon::MediaObject* theWrappedObject, int  msecToEnd);
   void setQueue(Phonon::MediaObject* theWrappedObject, const QList<Phonon::MediaSource >&  sources);
   void setQueue(Phonon::MediaObject* theWrappedObject, const QList<QUrl >&  urls);
   void setTransitionTime(Phonon::MediaObject* theWrappedObject, int  msec);
   Phonon::State  state(Phonon::MediaObject* theWrappedObject) const;
   int  tickInterval(Phonon::MediaObject* theWrappedObject) const;
   qint64  totalTime(Phonon::MediaObject* theWrappedObject) const;
   int  transitionTime(Phonon::MediaObject* theWrappedObject) const;
};





class PythonQtWrapper_MediaSource : public QObject
{ Q_OBJECT
public:
Q_ENUMS(Type )
enum Type{
  Invalid = Phonon::MediaSource::Invalid,   LocalFile = Phonon::MediaSource::LocalFile,   Url = Phonon::MediaSource::Url,   Disc = Phonon::MediaSource::Disc,   Stream = Phonon::MediaSource::Stream,   Empty = Phonon::MediaSource::Empty};
public slots:
Phonon::MediaSource* new_MediaSource();
Phonon::MediaSource* new_MediaSource(Phonon::AbstractMediaStream*  stream);
Phonon::MediaSource* new_MediaSource(Phonon::DiscType  discType, const QString&  deviceName = QString());
Phonon::MediaSource* new_MediaSource(QIODevice*  ioDevice);
Phonon::MediaSource* new_MediaSource(const Phonon::MediaSource&  rhs);
Phonon::MediaSource* new_MediaSource(const QString&  fileName);
Phonon::MediaSource* new_MediaSource(const QUrl&  url);
void delete_MediaSource(Phonon::MediaSource* obj) { delete obj; } 
   bool  autoDelete(Phonon::MediaSource* theWrappedObject) const;
   QString  deviceName(Phonon::MediaSource* theWrappedObject) const;
   Phonon::DiscType  discType(Phonon::MediaSource* theWrappedObject) const;
   QString  fileName(Phonon::MediaSource* theWrappedObject) const;
   bool  __eq__(Phonon::MediaSource* theWrappedObject, const Phonon::MediaSource&  rhs) const;
   void setAutoDelete(Phonon::MediaSource* theWrappedObject, bool  enable);
   Phonon::AbstractMediaStream*  stream(Phonon::MediaSource* theWrappedObject) const;
   Phonon::MediaSource::Type  type(Phonon::MediaSource* theWrappedObject) const;
   QUrl  url(Phonon::MediaSource* theWrappedObject) const;
};





class PythonQtWrapper_Path : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::Path* new_Path();
Phonon::Path* new_Path(const Phonon::Path&  arg__1);
void delete_Path(Phonon::Path* obj) { delete obj; } 
   bool  disconnect(Phonon::Path* theWrappedObject);
   QList<Phonon::Effect* >  effects(Phonon::Path* theWrappedObject) const;
   bool  insertEffect(Phonon::Path* theWrappedObject, Phonon::Effect*  newEffect, Phonon::Effect*  insertBefore = 0);
   Phonon::Effect*  insertEffect(Phonon::Path* theWrappedObject, const Phonon::EffectDescription&  desc, Phonon::Effect*  insertBefore = 0);
   bool  isValid(Phonon::Path* theWrappedObject) const;
   bool  __ne__(Phonon::Path* theWrappedObject, const Phonon::Path&  p) const;
   bool  __eq__(Phonon::Path* theWrappedObject, const Phonon::Path&  p) const;
   bool  reconnect(Phonon::Path* theWrappedObject, Phonon::MediaNode*  source, Phonon::MediaNode*  sink);
   bool  removeEffect(Phonon::Path* theWrappedObject, Phonon::Effect*  effect);
   Phonon::MediaNode*  sink(Phonon::Path* theWrappedObject) const;
   Phonon::MediaNode*  source(Phonon::Path* theWrappedObject) const;
};





class PythonQtWrapper_Phonon : public QObject
{ Q_OBJECT
public:
Q_ENUMS(DiscType MetaData Category ObjectDescriptionType State ErrorType )
enum DiscType{
  NoDisc = Phonon::NoDisc,   Cd = Phonon::Cd,   Dvd = Phonon::Dvd,   Vcd = Phonon::Vcd};
enum MetaData{
  ArtistMetaData = Phonon::ArtistMetaData,   AlbumMetaData = Phonon::AlbumMetaData,   TitleMetaData = Phonon::TitleMetaData,   DateMetaData = Phonon::DateMetaData,   GenreMetaData = Phonon::GenreMetaData,   TracknumberMetaData = Phonon::TracknumberMetaData,   DescriptionMetaData = Phonon::DescriptionMetaData,   MusicBrainzDiscIdMetaData = Phonon::MusicBrainzDiscIdMetaData};
enum Category{
  NoCategory = Phonon::NoCategory,   NotificationCategory = Phonon::NotificationCategory,   MusicCategory = Phonon::MusicCategory,   VideoCategory = Phonon::VideoCategory,   CommunicationCategory = Phonon::CommunicationCategory,   GameCategory = Phonon::GameCategory,   AccessibilityCategory = Phonon::AccessibilityCategory,   LastCategory = Phonon::LastCategory};
enum ObjectDescriptionType{
  AudioOutputDeviceType = Phonon::AudioOutputDeviceType,   EffectType = Phonon::EffectType,   AudioChannelType = Phonon::AudioChannelType,   SubtitleType = Phonon::SubtitleType,   AudioCaptureDeviceType = Phonon::AudioCaptureDeviceType};
enum State{
  LoadingState = Phonon::LoadingState,   StoppedState = Phonon::StoppedState,   PlayingState = Phonon::PlayingState,   BufferingState = Phonon::BufferingState,   PausedState = Phonon::PausedState,   ErrorState = Phonon::ErrorState};
enum ErrorType{
  NoError = Phonon::NoError,   NormalError = Phonon::NormalError,   FatalError = Phonon::FatalError};
public slots:
   QString  static_Phonon_categoryToString(Phonon::Category  c);
   Phonon::Path  static_Phonon_createPath(Phonon::MediaNode*  source, Phonon::MediaNode*  sink);
   Phonon::MediaObject*  static_Phonon_createPlayer(Phonon::Category  category, const Phonon::MediaSource&  source = Phonon::MediaSource());
   const char*  static_Phonon_phononVersion();
   uint  static_Phonon_qHash(const Phonon::EffectParameter&  param);
};





class PythonQtShell_PlatformPlugin : public Phonon::PlatformPlugin
{
public:
    PythonQtShell_PlatformPlugin():Phonon::PlatformPlugin(),_wrapper(NULL) {};

virtual QString  applicationName() const;
virtual QObject*  createBackend();
virtual QObject*  createBackend(const QString&  library, const QString&  version);
virtual Phonon::AbstractMediaStream*  createMediaStream(const QUrl&  url, QObject*  parent);
virtual QList<QPair<QByteArray , QString >  >  deviceAccessListFor(const Phonon::AudioOutputDevice&  arg__1) const;
virtual QIcon  icon(const QString&  name) const;
virtual bool  isMimeTypeAvailable(const QString&  mimeType) const;
virtual qreal  loadVolume(const QString&  outputName) const;
virtual void notification(const char*  notificationName, const QString&  text, const QStringList&  actions = QStringList(), QObject*  receiver = 0, const char*  actionSlot = 0) const;
virtual QList<int >  objectDescriptionIndexes(Phonon::ObjectDescriptionType  type) const;
virtual QHash<QByteArray , QVariant >  objectDescriptionProperties(Phonon::ObjectDescriptionType  type, int  index) const;
virtual void saveVolume(const QString&  outputName, qreal  volume);

  PythonQtInstanceWrapper* _wrapper; 
};

class PythonQtPublicPromoter_PlatformPlugin : public Phonon::PlatformPlugin
{ public:
inline QList<QPair<QByteArray , QString >  >  promoted_deviceAccessListFor(const Phonon::AudioOutputDevice&  arg__1) const { return Phonon::PlatformPlugin::deviceAccessListFor(arg__1); }
};

class PythonQtWrapper_PlatformPlugin : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::PlatformPlugin* new_PlatformPlugin();
void delete_PlatformPlugin(Phonon::PlatformPlugin* obj) { delete obj; } 
   QList<QPair<QByteArray , QString >  >  deviceAccessListFor(Phonon::PlatformPlugin* theWrappedObject, const Phonon::AudioOutputDevice&  arg__1) const;
};





class PythonQtShell_SeekSlider : public Phonon::SeekSlider
{
public:
    PythonQtShell_SeekSlider(Phonon::MediaObject*  media, QWidget*  parent = 0):Phonon::SeekSlider(media, parent),_wrapper(NULL) {};
    PythonQtShell_SeekSlider(QWidget*  parent = 0):Phonon::SeekSlider(parent),_wrapper(NULL) {};

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

class PythonQtWrapper_SeekSlider : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::SeekSlider* new_SeekSlider(Phonon::MediaObject*  media, QWidget*  parent = 0);
Phonon::SeekSlider* new_SeekSlider(QWidget*  parent = 0);
void delete_SeekSlider(Phonon::SeekSlider* obj) { delete obj; } 
   bool  hasTracking(Phonon::SeekSlider* theWrappedObject) const;
   QSize  iconSize(Phonon::SeekSlider* theWrappedObject) const;
   bool  isIconVisible(Phonon::SeekSlider* theWrappedObject) const;
   Phonon::MediaObject*  mediaObject(Phonon::SeekSlider* theWrappedObject) const;
   Qt::Orientation  orientation(Phonon::SeekSlider* theWrappedObject) const;
   int  pageStep(Phonon::SeekSlider* theWrappedObject) const;
   void setPageStep(Phonon::SeekSlider* theWrappedObject, int  milliseconds);
   void setSingleStep(Phonon::SeekSlider* theWrappedObject, int  milliseconds);
   void setTracking(Phonon::SeekSlider* theWrappedObject, bool  tracking);
   int  singleStep(Phonon::SeekSlider* theWrappedObject) const;
};





class PythonQtWrapper_SubtitleDescription : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::SubtitleDescription* new_SubtitleDescription();
Phonon::SubtitleDescription* new_SubtitleDescription(int  index, const QHash<QByteArray , QVariant >&  properties);
Phonon::SubtitleDescription* new_SubtitleDescription(const Phonon::SubtitleDescription& other) {
Phonon::SubtitleDescription* a = new Phonon::SubtitleDescription();
*((Phonon::SubtitleDescription*)a) = other;
return a; }
void delete_SubtitleDescription(Phonon::SubtitleDescription* obj) { delete obj; } 
   QString  description(Phonon::SubtitleDescription* theWrappedObject) const;
   int  index(Phonon::SubtitleDescription* theWrappedObject) const;
   bool  isValid(Phonon::SubtitleDescription* theWrappedObject) const;
   QString  name(Phonon::SubtitleDescription* theWrappedObject) const;
   QVariant  property(Phonon::SubtitleDescription* theWrappedObject, const char*  name) const;
   QList<QByteArray >  propertyNames(Phonon::SubtitleDescription* theWrappedObject) const;
};





class PythonQtShell_VideoPlayer : public Phonon::VideoPlayer
{
public:
    PythonQtShell_VideoPlayer(Phonon::Category  category, QWidget*  parent = 0):Phonon::VideoPlayer(category, parent),_wrapper(NULL) {};
    PythonQtShell_VideoPlayer(QWidget*  parent = 0):Phonon::VideoPlayer(parent),_wrapper(NULL) {};

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

class PythonQtWrapper_VideoPlayer : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::VideoPlayer* new_VideoPlayer(Phonon::Category  category, QWidget*  parent = 0);
Phonon::VideoPlayer* new_VideoPlayer(QWidget*  parent = 0);
void delete_VideoPlayer(Phonon::VideoPlayer* obj) { delete obj; } 
   Phonon::AudioOutput*  audioOutput(Phonon::VideoPlayer* theWrappedObject) const;
   qint64  currentTime(Phonon::VideoPlayer* theWrappedObject) const;
   bool  isPaused(Phonon::VideoPlayer* theWrappedObject) const;
   bool  isPlaying(Phonon::VideoPlayer* theWrappedObject) const;
   Phonon::MediaObject*  mediaObject(Phonon::VideoPlayer* theWrappedObject) const;
   qint64  totalTime(Phonon::VideoPlayer* theWrappedObject) const;
   Phonon::VideoWidget*  videoWidget(Phonon::VideoPlayer* theWrappedObject) const;
   float  volume(Phonon::VideoPlayer* theWrappedObject) const;
};





class PythonQtShell_VideoWidget : public Phonon::VideoWidget
{
public:
    PythonQtShell_VideoWidget(QWidget*  parent = 0):Phonon::VideoWidget(parent),_wrapper(NULL) {};

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

class PythonQtPublicPromoter_VideoWidget : public Phonon::VideoWidget
{ public:
inline bool  promoted_event(QEvent*  arg__1) { return Phonon::VideoWidget::event(arg__1); }
inline void promoted_mouseMoveEvent(QMouseEvent*  arg__1) { Phonon::VideoWidget::mouseMoveEvent(arg__1); }
};

class PythonQtWrapper_VideoWidget : public QObject
{ Q_OBJECT
public:
public slots:
Phonon::VideoWidget* new_VideoWidget(QWidget*  parent = 0);
void delete_VideoWidget(Phonon::VideoWidget* obj) { delete obj; } 
   Phonon::VideoWidget::AspectRatio  aspectRatio(Phonon::VideoWidget* theWrappedObject) const;
   qreal  brightness(Phonon::VideoWidget* theWrappedObject) const;
   qreal  contrast(Phonon::VideoWidget* theWrappedObject) const;
   bool  event(Phonon::VideoWidget* theWrappedObject, QEvent*  arg__1);
   qreal  hue(Phonon::VideoWidget* theWrappedObject) const;
   void mouseMoveEvent(Phonon::VideoWidget* theWrappedObject, QMouseEvent*  arg__1);
   qreal  saturation(Phonon::VideoWidget* theWrappedObject) const;
   Phonon::VideoWidget::ScaleMode  scaleMode(Phonon::VideoWidget* theWrappedObject) const;
   QImage  snapshot(Phonon::VideoWidget* theWrappedObject) const;
};



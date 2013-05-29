#include <PythonQt.h>
#include "com_trolltech_qt_phonon0.h"
#include "com_trolltech_qt_phonon1.h"


void PythonQt_init_QtPhonon(PyObject* module) {
PythonQt::priv()->registerCPPClass("Phonon::AddonInterface", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractAddon>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractAddon>, module, 0);
PythonQt::self()->addParentClass("Phonon::AddonInterface", "AbstractAddon",PythonQtUpcastingOffset<Phonon::AddonInterface,AbstractAddon>());
PythonQt::priv()->registerClass(&Phonon::AbstractAudioOutput::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractAudioOutput>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractAudioOutput>, module, 0);
PythonQt::self()->addParentClass("Phonon::AbstractAudioOutput", "MediaNode",PythonQtUpcastingOffset<Phonon::AbstractAudioOutput,MediaNode>());
PythonQt::priv()->registerCPPClass("Phonon::BackendInterface", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractBackend>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractBackend>, module, 0);
PythonQt::self()->addParentClass("Phonon::BackendInterface", "AbstractBackend",PythonQtUpcastingOffset<Phonon::BackendInterface,AbstractBackend>());
PythonQt::priv()->registerCPPClass("Phonon::EffectInterface", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractEffect>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractEffect>, module, 0);
PythonQt::self()->addParentClass("Phonon::EffectInterface", "AbstractEffect",PythonQtUpcastingOffset<Phonon::EffectInterface,AbstractEffect>());
PythonQt::priv()->registerCPPClass("Phonon::MediaObjectInterface", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractMediaObject>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractMediaObject>, module, 0);
PythonQt::self()->addParentClass("Phonon::MediaObjectInterface", "AbstractMediaObject",PythonQtUpcastingOffset<Phonon::MediaObjectInterface,AbstractMediaObject>());
PythonQt::priv()->registerClass(&Phonon::AbstractMediaStream::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractMediaStream>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractMediaStream>, module, 0);
PythonQt::priv()->registerCPPClass("Phonon::StreamInterface", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractStream>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractStream>, module, 0);
PythonQt::self()->addParentClass("Phonon::StreamInterface", "AbstractStream",PythonQtUpcastingOffset<Phonon::StreamInterface,AbstractStream>());
PythonQt::priv()->registerCPPClass("Phonon::AbstractVideoOutput", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractVideoOutput>, NULL, module, 0);
PythonQt::self()->addParentClass("Phonon::AbstractVideoOutput", "AbstractVideoOutput",PythonQtUpcastingOffset<Phonon::AbstractVideoOutput,AbstractVideoOutput>());
PythonQt::self()->addParentClass("Phonon::AbstractVideoOutput", "MediaNode",PythonQtUpcastingOffset<Phonon::AbstractVideoOutput,MediaNode>());
PythonQt::priv()->registerCPPClass("Phonon::VideoWidgetInterface", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractVideoWidget>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractVideoWidget>, module, 0);
PythonQt::self()->addParentClass("Phonon::VideoWidgetInterface", "AbstractVideoWidget",PythonQtUpcastingOffset<Phonon::VideoWidgetInterface,AbstractVideoWidget>());
PythonQt::priv()->registerCPPClass("Phonon::VolumeFaderInterface", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AbstractVolumeFader>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AbstractVolumeFader>, module, 0);
PythonQt::self()->addParentClass("Phonon::VolumeFaderInterface", "AbstractVolumeFader",PythonQtUpcastingOffset<Phonon::VolumeFaderInterface,AbstractVolumeFader>());
PythonQt::priv()->registerCPPClass("Phonon::AudioCaptureDevice", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AudioCaptureDevice>, NULL, module, PythonQt::Type_RichCompare);
PythonQt::priv()->registerCPPClass("Phonon::AudioChannelDescription", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AudioChannelDescription>, NULL, module, PythonQt::Type_RichCompare);
PythonQt::priv()->registerClass(&Phonon::AudioOutput::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AudioOutput>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_AudioOutput>, module, 0);
PythonQt::priv()->registerCPPClass("Phonon::AudioOutputDevice", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_AudioOutputDevice>, NULL, module, PythonQt::Type_RichCompare);
PythonQt::priv()->registerCPPClass("Phonon::BackendCapabilities", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_BackendCapabilities>, NULL, module, 0);
PythonQt::priv()->registerClass(&Phonon::Effect::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_Effect>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_Effect>, module, 0);
PythonQt::self()->addParentClass("Phonon::Effect", "MediaNode",PythonQtUpcastingOffset<Phonon::Effect,MediaNode>());
PythonQt::priv()->registerCPPClass("Phonon::EffectDescription", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_EffectDescription>, NULL, module, PythonQt::Type_RichCompare);
PythonQt::priv()->registerCPPClass("Phonon::EffectParameter", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_EffectParameter>, NULL, module, PythonQt::Type_RichCompare);
PythonQt::priv()->registerClass(&Phonon::EffectWidget::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_EffectWidget>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_EffectWidget>, module, 0);
PythonQt::priv()->registerClass(&Phonon::MediaController::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_MediaController>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_MediaController>, module, 0);
PythonQt::priv()->registerCPPClass("Phonon::MediaNode", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_MediaNode>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_MediaNode>, module, 0);
PythonQt::self()->addParentClass("Phonon::MediaNode", "MediaNode",PythonQtUpcastingOffset<Phonon::MediaNode,MediaNode>());
PythonQt::priv()->registerClass(&Phonon::MediaObject::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_MediaObject>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_MediaObject>, module, 0);
PythonQt::self()->addParentClass("Phonon::MediaObject", "MediaNode",PythonQtUpcastingOffset<Phonon::MediaObject,MediaNode>());
PythonQt::priv()->registerCPPClass("Phonon::MediaSource", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_MediaSource>, NULL, module, PythonQt::Type_RichCompare);
PythonQt::priv()->registerCPPClass("Phonon::Path", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_Path>, NULL, module, PythonQt::Type_RichCompare);
PythonQt::priv()->registerCPPClass("Phonon", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_Phonon>, NULL, module, 0);
PythonQt::priv()->registerCPPClass("Phonon::PlatformPlugin", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_PlatformPlugin>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_PlatformPlugin>, module, 0);
PythonQt::self()->addParentClass("Phonon::PlatformPlugin", "PlatformPlugin",PythonQtUpcastingOffset<Phonon::PlatformPlugin,PlatformPlugin>());
PythonQt::priv()->registerClass(&Phonon::SeekSlider::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_SeekSlider>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_SeekSlider>, module, 0);
PythonQt::priv()->registerCPPClass("Phonon::SubtitleDescription", "", "QtPhonon", PythonQtCreateObject<PythonQtWrapper_SubtitleDescription>, NULL, module, PythonQt::Type_RichCompare);
PythonQt::priv()->registerClass(&Phonon::VideoPlayer::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_VideoPlayer>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_VideoPlayer>, module, 0);
PythonQt::priv()->registerClass(&Phonon::VideoWidget::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_VideoWidget>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_VideoWidget>, module, 0);
PythonQt::self()->addParentClass("Phonon::VideoWidget", "AbstractVideoOutput",PythonQtUpcastingOffset<Phonon::VideoWidget,AbstractVideoOutput>());
PythonQt::self()->addParentClass("Phonon::VideoWidget", "MediaNode",PythonQtUpcastingOffset<Phonon::VideoWidget,MediaNode>());
PythonQt::priv()->registerClass(&Phonon::VolumeFaderEffect::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_VolumeFaderEffect>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_VolumeFaderEffect>, module, 0);
PythonQt::priv()->registerClass(&Phonon::VolumeSlider::staticMetaObject, "QtPhonon", PythonQtCreateObject<PythonQtWrapper_VolumeSlider>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_VolumeSlider>, module, 0);

}

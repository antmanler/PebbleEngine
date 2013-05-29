from PythonQt.PIWWidgets import *
from PythonQt.QtCore import Qt
import horde3d as h3d
import os

BaseDir = os.path.dirname(os.path.abspath('../'))
DataDir = os.path.join(BaseDir, 'Data/Common')
SceneDir = os.path.join(BaseDir, 'Data/Scene')

class SceneBase(object):

    def __init__(self, name):
        self.__camera = 0
        self.__scene_name = name
        self.__pause = False

    def add_camera(self, name, pipeRes):
        self.__camera = h3d.addCameraNode(h3d.RootNode, name, pipeRes)
        h3d.setNodeTransform(self.camera, 0, 0, 50, 0, 0, 0, 1, 1, 1)

    @property
    def pause(self):
        return self.__pause
    
    @pause.setter
    def pause(self, val):
        self.__pause = val
        
    @property
    def camera(self):
        return self.__camera

    @property
    def name(self):
        return self.__scene_name

    def load_res(self): raise NotImplementedError
    def setup_secen(self): raise NotImplementedError

    def graphic_update_func(self, *vars) :
        if not self.pause :
            self.graphic_update(*vars)

    def graphic_update(self, curr_fps, anim_time, weight) : pass

class SceneHolder(object):
    def __init__(self):
        self.__curr = None
        self.__gSceneDict = {}
    @property
    def curr(self) : return self.__curr

gScnHolder = SceneHolder()
def AddNewScene(scene, override=True) :
    global gScnHolder
    name = scene.name
    def ch_scene() :
        ChangeScene(name)

    __gSceneDict = gScnHolder.__dict__['_SceneHolder__gSceneDict']
    if name not in __gSceneDict or override :
        __gSceneDict[name] = scene
        gScnHolder.__dict__[name] = ch_scene

def LoadScene(file_name) :
    full_path = os.path.realpath(os.path.join(SceneDir, file_name))
    if os.path.exists(full_path) :
        code = compile(open(full_path).read(), os.path.split(full_path)[-1] ,'exec')
        global_dict = dict(globals(), __file__=full_path)
        exec(code, global_dict)
    else :
        print 'Can not load scene {}'.format(file_name)

# python slots for handle signals
def set_engine_options() :
    h3d.setOption(h3d.Options.LoadTextures, 1)
    h3d.setOption(h3d.Options.TexCompression, 0)
    h3d.setOption(h3d.Options.FastAnimation, 0)
    h3d.setOption(h3d.Options.MaxAnisotropy, 4)
    h3d.setOption(h3d.Options.ShadowMapSize, 2048)

def load_res() :
    if gScnHolder.curr is None : return
    gScnHolder.curr.load_res()

def setup_secen():
    if gScnHolder.curr is None : return
    gScnHolder.curr.setup_scene()

def graphic_update(curr_fps, anim_time, weight) :
    if gScnHolder.curr is None : return
    gScnHolder.curr.graphic_update_func(curr_fps, anim_time, weight)

horde_window.connect('load_resource()', load_res)
horde_window.connect('setup_scene()', setup_secen)
horde_window.connect('set_engine_options()', set_engine_options)
horde_window.connect('graphic_update(float, float, float)', graphic_update)

# contian the target and its signals and connected slots
# for each target there a tupe of (signal_names, signal_dict) attached
# for each signal_dict there a list of slots that connected to
# { target : 
#    ( signal_names,
#      signal_dict { signal_name : list_of_slots[] }
#    )
# }
__signal_slot_ = {}

def ConnectorGenerator(target) :
    global __signal_slot_
    
    has_sig = hasattr(target, 'help') and hasattr(target, 'connect')
    signal_dict = {}
    if has_sig :
        # generate signals table for target
        if not __signal_slot_.has_key(target) :
            sigs = target.help()
            sigs = (sigs[sigs.find('Signals:')+9:]).splitlines()
            sigs_dict = dict([(signal, []) for signal in sigs])
            # add to signals dict
            __signal_slot_[target] = ('#'.join(sigs), sigs_dict)
        signal_dict = __signal_slot_[target][1]
        has_sig = has_sig and len(signal_dict) > 0

    # start the connector
    def connector_warpper(signal) :
        import types
        signal = ''.join(signal.split())
        def connector(func) :
            if type(func) is not types.FunctionType:
                print >>sys.stderr, "{} is not a function".format(func)
            elif has_sig:
                if signal_dict.has_key(signal) :
                    target.connect(signal, func)
                    signal_dict[signal].append(func)
                else :
                    print >>sys.stderr, "{} is not a valid signal of {}".format(signal, target)
            return func
        return connector
           
    return connector_warpper


def setup_module(mod) :
    import types
    if type(mod) is not types.ModuleType : return
    mod_name = mod.__name__
    mod_dir = os.path.join(BaseDir, 'Data/Scripts/ui', mod_name)
    if not os.path.isdir(mod_dir) : return
    # process for each valid object
    for obj in dir(mod) :
        if obj.startswith('_') : continue
        sname = os.path.join(mod_dir, obj) + '.py'
        if os.path.exists(sname) and os.path.isfile(sname) :
            print >>sys.stderr, "'adding {}'".format(obj)
            obj_inst = getattr(mod, obj)
            # execute the script
            code = compile(open(sname).read(), os.path.split(full_path)[-1] ,'exec')
            global_dict = dict(connect = ConnectorGenerator(obj_inst),
                               get_connector = ConnectorGenerator)
            exec(code, global_dict)


def clean_connections() :
    global __signal_slot_
    for obj, data in __signal_slot_.items() :
        for sig, slots in data[1].items() :
            for slot in slots :
                # disconnect the old connections
                if not obj.disconnect(sig, slot) :
                    print >>sys.stderr, \
                    "the slot({}) of signal {} cannot be removed".format(slot, sig)
    # derefrence the old container
    __signal_slot_ = {}


# setup scripts
import PiwMainWindow
setup_module(PiwMainWindow)


def ChangeScene(name) :
    global gScnHolder
    __gSceneDict = gScnHolder.__dict__['_SceneHolder__gSceneDict']
    scene = __gSceneDict.get(name, None) 

    if scene is not None :
        gScnHolder.__dict__['_SceneHolder__curr'] = scene
        horde_window.Reset()
        horde_window.SetCurrentCamera(scene.camera)
        # setup_scene_tree_widget()
    else:
        print 'Can not change scene {}'.format(name)

def SetCamera(cam) :
    horde_window.SetCurrentCamera(cam)
    horde_window.activateWindow()

def ReLoad() :
    horde_window.Reset()
    #SetCamera(CurrentScene().camera)
    SetCamera(gScnHolder.curr.camera)
    
def ViewRenderTarget(cam=0):
    if cam == 0: 
        print 'Invalid camera node'
        if gScnHolder.curr is not None :
            cam = gScnHolder.curr.camera
        else :
            print ''
            return
    #global __rt_window
    __rt_window = None
    if __rt_window is None :
        __rt_window = RenderTargetView()
    __rt_window.setCamera(cam)
    __rt_window.show()
    return __rt_window

# clear globals
globals().pop('SceneHolder')

# load default scenes
LoadScene('default.scn.py')
LoadScene('terrain.scn.py')
LoadScene('chicago.scn.py')
LoadScene('box.scn.py')
LoadScene('sphwater.scn.py')
LoadScene('sph1.scn.py')
LoadScene('sphwin.scn.py')


#horde_window.show()
# ChangeScene('Default')
if sys.platform == 'darwin' :
    ChangeScene('Sph1')
else:
    ChangeScene('SphWin')
    
#horde_window.close()

#h3d.release()

glw = GLWidget()
glw.show()

def quit() : 
    from PythonQt import QtGui
    QtGui.QApplication.exit()


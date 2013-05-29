

from ctypes import *
import sys
import json

if not 'c_bool' in globals():
	c_bool = c_int

__all__ = []

__platform = sys.platform


try:
    if __platform == 'darwin' :
        piw = cdll.LoadLibrary('libEngine.dylib')
    elif __platform[:3] == 'win' :
        piw = cdll.LoadLibrary('Engine.dll')
    else:
        piw = cdll.LoadLibrary('libEngine.so')
except OSError:
    print '---------------------------------------------------'
    print '---       Pebble Engine Python Wrapper          ---'
    print '---                                             ---'
    print '--- could not find libEngine.so or Engine.dll ---'
    print '--- check the search path for library files!    ---'
    print '--- or modify horde3d/__init__.py               ---'
    print '---------------------------------------------------'
    raise


__broadcastMessage = piw.piwBroadcastMessage
__broadcastMessage.restype = None
__broadcastMessage.argtypes = [c_char_p, c_char_p]

def broadcastMessage(name, *args, **kwargs):
    params = dict(argv=args, kwargv=kwargs)
    try:
        data = json.dumps(params)
        print name.encode('UTF-8'), data.encode('UTF-8')
        __broadcastMessage(name.encode('UTF-8'), data.encode('UTF-8'))
    except TypeError :
        # can not serialize to json, failed quietly
        return False
    
__all__.append('broadcastMessage')
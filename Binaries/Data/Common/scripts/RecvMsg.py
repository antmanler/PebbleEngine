from Engine import *
from PythonBehavior import *

class RecvMsg(PythonBehavior) :
    
    def printH3dId(self, id = 0) :
        print self.typeName(), ': receive, horde id is ', id
        
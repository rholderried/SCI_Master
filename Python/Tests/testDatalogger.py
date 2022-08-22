import sys, os
sys.path.append(os.path.realpath(os.path.join(os.path.dirname(__file__), '..')))

from SCI import Datatype, SCI, Variable
from Datalogger import Datalogger
from time import sleep

cur_A = Variable(1, Datatype.DTYPE_UINT16, 'phase A current, in Q format')
cur_B = Variable(2, Datatype.DTYPE_UINT16, 'phase B current, in Q format')
cur_C = Variable(3, Datatype.DTYPE_UINT16, 'phase C current, in Q format')

sciHdl = SCI('COM27', 128)
dlogHdl = Datalogger(sciHdl, 0, 256)

dlogHdl.assignCmdNumbers(   cmdNumGetDataloggerVersion=5, 
                            cmdNumRegisterLogFromVarStruct=7, 
                            cmdNumInitializeDatalogger=9, 
                            cmdNumStartDatalogger=10, 
                            cmdNumGetLogData=12)
dlogHdl.getVersion()

dlogHdl.register(cur_A, 10, 1)
dlogHdl.register(cur_B, 10, 1)
dlogHdl.register(cur_C, 10, 1)

dlogHdl.initializeLogger()

dlogHdl.start()
sleep(0.5)

data = dlogHdl.getData()
print(data)

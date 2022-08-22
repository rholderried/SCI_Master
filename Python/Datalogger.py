"""
Datalogger SCI interface

History:
--------
- Created by Holderried, Roman, 22.08.2022
"""

from xmlrpc.client import boolean
from SCI import SCI, Function, Variable, Datatype
from typing import *
from Common import Version


class Datalogger:

    MAX_NUM_LOGS = 8

    def __init__(self, SCI: SCI, index: int, maxNumBytes : int):
        """
        Datalogger API initializer

        Parameters:
        -----------
        - SCI           : Handle to the SCI Master
        - index         : Index of the datalogger
        - maxNumBytes   : Maximum number of bytes that the on-board buffer can hold
        """

        self.functions : Dict[str, Function] = \
                         {  'GetDataloggerVersion'      : Function(0, [Datatype.DTYPE_UINT8], [Datatype.DTYPE_UINT8, Datatype.DTYPE_UINT8, Datatype.DTYPE_UINT8], 'Get the Datalogger version in Format V{Version_Major}_{Version_Minor}_{Revision}.', False),
                            'RegisterLogFromVarStruct'  : Function(0, [Datatype.DTYPE_UINT8, Datatype.DTYPE_UINT8, Datatype.DTYPE_UINT16, Datatype.DTYPE_UINT16, Datatype.DTYPE_UINT32], [], 'Register a log channel with a variable from the variable structure.', False),
                            'InitializeDatalogger'      : Function(0, [Datatype.DTYPE_UINT8], [], 'Initialize the datalogger with the registered logger channels.', False),
                            'StartDatalogger'           : Function(0, [Datatype.DTYPE_UINT8], [], 'Start the initialized datalogger.',False),
                            'StopDatalogger'            : Function(0, [Datatype.DTYPE_UINT8], [], 'Stop the datalogger.', False),
                            'GetLogData'                : Function(0, [Datatype.DTYPE_UINT8], [], 'Initiate the data upstream.', True),
                            'GetChannelInfo'            : Function(0, [Datatype.DTYPE_UINT8, Datatype.DTYPE_UINT8], [Datatype.DTYPE_UINT32, Datatype.DTYPE_UINT16, Datatype.DTYPE_UINT32, Datatype.DTYPE_UINT32, Datatype.DTYPE_UINT32], 'Return the relevant channel data.', False),
                            'ResetDatalogger'           : Function(0, [Datatype.DTYPE_UINT8], [], 'Reset the data structure.', False),
                            'SetOpMode'                 : Function(0, [Datatype.DTYPE_UINT8, Datatype.DTYPE_UINT32], [], 'Set the operation mode of the data logger.', False)}

        self.logsRegistered : Dict[int, Variable] = dict()

        self.index          : int = index
        self.sciHdl         : SCI = SCI
        self.version        : Version = Version()
        self.maxNumBytes    : int = maxNumBytes
        self.numBytesLeft   : int = maxNumBytes

        self.loggerInit     : boolean = False



    def assignCmdNumbers(self,  cmdNumGetDataloggerVersion : int = 0, cmdNumRegisterLogFromVarStruct : int = 0,
                                cmdNumInitializeDatalogger : int = 0, cmdNumStartDatalogger : int = 0,
                                cmdNumStopDatalogger : int = 0, cmdNumGetLogData : int = 0, cmdNumGetChannelInfo : int = 0,
                                cmdNumResetDatalogger : int = 0, cmdNumSetOpMode : int = 0):
        """
        Assign a command number to the functions. This must be done before calling the external function!

        Parameters:
        -----------
        - cmdNumX       : Command number of the callback
        """

        self.functions['GetDataloggerVersion'].number = cmdNumGetDataloggerVersion
        self.functions['RegisterLogFromVarStruct'].number = cmdNumRegisterLogFromVarStruct
        self.functions['InitializeDatalogger'].number = cmdNumInitializeDatalogger
        self.functions['StartDatalogger'].number = cmdNumStartDatalogger
        self.functions['StopDatalogger'].number = cmdNumStopDatalogger
        self.functions['GetLogData'].number = cmdNumGetLogData
        self.functions['GetChannelInfo'].number = cmdNumGetChannelInfo
        self.functions['ResetDatalogger'].number = cmdNumResetDatalogger
        self.functions['SetOpMode'].number = cmdNumSetOpMode
    
    def getVersion(self):
        """
        Queries the version numbers from the datalogger.
        """
        
        resp = self.sciHdl.command(self.functions['GetDataloggerVersion'], [self.index])

        self.version.versionMajor = resp[0]
        self.version.versionMinor = resp[1]
        self.version.revision = resp[2]

    def register(self, variable : Variable, recLen : int, divider : int, channel : int = 0):

        if (self.functions['RegisterLogFromVarStruct'].number <= 0):
            raise Exception(f'No valid channel number for the callback RegisterLogFromVarStruct assigned.')

        freeChannels : List = list(range(1,self.MAX_NUM_LOGS + 1))

        if len(self.logsRegistered) == self.MAX_NUM_LOGS:
            raise Exception(f'Maximum number of channels is {self.MAX_NUM_LOGS}.')

        # Variable checks for plausibility ----------------------------------------------
        if divider <= 0:
            divider = 1

        if recLen < 0:
            recLen = 0
        # -------------------------------------------------------------------------------

        for key in sorted(self.logsRegistered, reverse=True):
                freeChannels.pop(key - 1)

        if channel <= 0:
            # Choose the next free channel for this log
            channel = freeChannels[0]

        elif channel not in freeChannels:
            raise Exception(f'Channel {channel} is already assigned.')

        recByteLen = variable.type.value[1] * recLen

        if recByteLen > self.numBytesLeft:
            raise Exception(f'Requested size of log {recLen} cannot be hold by the onboard buffer, which has {int(self.numBytesLeft / variable.type.value[1])} bytes left.')

        self.sciHdl.command(self.functions['RegisterLogFromVarStruct'], [self.index, channel, variable.number, divider, recLen])

        # We get here: Command was successful
        self.numBytesLeft = self.numBytesLeft - recByteLen
        self.logsRegistered.update({channel : variable})
        self.loggerInit = False

    def initializeLogger(self):

        if (self.functions['InitializeDatalogger'].number <= 0):
            raise Exception(f'No valid channel number for the callback InitializeDatalogger assigned.')

        if len(self.logsRegistered) == 0:
            raise Exception(f'Channels must be registered before they can be initialized.')

        self.sciHdl.command(self.functions['InitializeDatalogger'], [self.index])

        self.loggerInit = True


    def start(self):

        if (self.functions['StartDatalogger'].number <= 0):
            raise Exception(f'No valid channel number for the callback StartDatalogger assigned.')

        if not self.loggerInit:
            raise Exception(f'Onboard logger must be initialized befor starting the logger.')

        self.sciHdl.command(self.functions['StartDatalogger'], [self.index])

    def stop(self):

        if (self.functions['StopDatalogger'].number <= 0):
            raise Exception(f'No valid channel number for the callback StopDatalogger assigned.')

        self.sciHdl.command(self.functions['StopDatalogger'], [self.index])

    def getData(self):

        data = self.sciHdl.requestUpstream(self.functions['GetLogData'], [self.index])

        return data











from enum import Enum

class CMD(Enum):
    PING = 0
    SEND_TWO_INTS = 1
    SEND_THREE_FLOATS = 2
    ECHO = 3
    DANCE = 4
    SET_VEL = 5
    GET_TIME_MILLIS = 6
    SEND_TIME_DATA = 7
    GET_TEMP_READINGS = 8
    GET_REPLY_LEN = 9
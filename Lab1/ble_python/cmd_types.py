from enum import Enum, auto

class AutoNumber(Enum):
    """
    枚举的基类，修改生成枚举值的规则使其从 0 开始自动递增。
    """
    def _generate_next_value_(name, start, count, last_values):
        return count

class CMD(AutoNumber):
    PING = auto()
    ECHO = auto()
    STOP = auto()
    START_AUTO = auto()
    STOP_AUTO = auto()
    SET_TWO_PWM = auto()
    SET_TWO_CTRL = auto()
    GET_CURR_DATA = auto()
    GET_HISTORY_DATA = auto()
    SET_GOAL_DISTANCE = auto()
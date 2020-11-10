import os
import time
from ctypes import *
from pathlib import Path

so_file_path = Path("c_lib.so").absolute()
c_lib = CDLL(so_file_path)

def doublePtr2_list(ptr, ch_list):
    for i in range(8):
        ch_list.append(ptr[i].decode("utf-8"))
        
def doublePtr2_doubleList(ptr, ch_list):
    for i in range(8):
        if 4 <= i <= 6:
            ch_list.append([ptr[i].decode("utf-8"), True])
        else:
            ch_list.append([ptr[i].decode("utf-8"), False])

class Conf():
    serial_port_path = "/dev/ttyACM0"
    serial_port = 0
    name = ""
    switch_B = []
    switch_L = []
    digital_in = []
    analog_in = []

    def conf_init(app):
        app.logger.info("Inizio configurazione")
        Conf.serial_port = os.open(Conf.serial_port_path, os.O_RDWR | os.O_NOCTTY | os.O_SYNC)
        time.sleep(2)
        app.logger.info("Porta seriale aperta correttamente")
        
        if c_lib.serial_port_init(Conf.serial_port) == 0:
            app.logger.info("Porta seriale configurata correttamente")
        else:
            app.logger.info("Errore durante la configurazione della porta seriale")
            os.close(Conf.serial_port)
        time.sleep(1)

        if c_lib.download_conf(Conf.serial_port) == 1:
            app.logger.info("Errore durante il caricamento della configurazione")
            os.close(Conf.serial_port)
        else:
            app.logger.info("Configurazione caricata correttamente")

        c_lib.get_name.restype = c_char_p
        c_lib.get_switch_B.restype = POINTER(c_char_p)
        c_lib.get_switch_L.restype = POINTER(c_char_p)
        c_lib.get_digital_in.restype = POINTER(c_char_p)
        c_lib.get_analog_in.restype = POINTER(c_char_p)
        
        Conf.name = c_lib.get_name().decode("utf-8").replace('_', ' ')
        doublePtr2_doubleList(c_lib.get_switch_B(), Conf.switch_B)
        doublePtr2_list(c_lib.get_switch_L(), Conf.switch_L)
        doublePtr2_list(c_lib.get_digital_in(), Conf.digital_in)
        doublePtr2_list(c_lib.get_analog_in(), Conf.analog_in)
        
        app.logger.info("Configurazione terminata")
        
    def set_channel_name(old_name, new_name):
        for i in range(8):
            if Conf.switch_B[i][0] == old_name:
                Conf.switch_B[i][0] = new_name
                return
            if Conf.switch_L[i] == old_name:
                Conf.switch_L[i] = new_name
                return
            if Conf.digital_in[i] == old_name:
                Conf.digital_in[i] = new_name
                return
            if Conf.analog_in[i] == old_name:
                Conf.analog_in[i] = new_name
                return
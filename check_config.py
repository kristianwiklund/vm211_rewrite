Import("env")
import sys
from os.path import isfile
from platformio.exception import PlatformioException

def check_config():

    if not isfile("src/user.h"):
        print("Copy src/user.h.dist to src/user.h and edit it to fit your setup")
        print("Terminating build")
        raise PlatformioException("Missing config file src/user.h")

check_config()

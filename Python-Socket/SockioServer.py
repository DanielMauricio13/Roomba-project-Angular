import socket

import eventlet
import socketio
import CyBotSock
import threading
import socket
import re
from flask import Flask
import queue
from gevent import pywsgi

eventlet.monkey_patch()

# create flask server, allow CORS to all sources (we don't care about security)
sio = socketio.Server(cors_allowed_origins='*')


startChar = "start"
endChar = "end"

# Allows calling emit to the flask socketio from separate thread
def bg_emit(_str):
    sio.emit('message', _str)

# Function periodically by eventlet threads. Listens for events
def listen(sock):
    str = ""
    while True:

        line = sock.recv(1024).decode('utf8')
        str  += line
        print(str)
        data = re.findall("START.+?END", str)
        for d in data:
            bg_emit(d)
            str = str.replace(d, "")


def listenFake():
    while True:
        msg = input("input message: ")
        bg_emit(msg)
class CyBotSocketServer():
    def __init__(self):

        self.app = socketio.WSGIApp(sio)

        # On connection events
        @sio.event
        def connect(sid, environ):
            sio.emit("message", "Welcome")
            print('connect ', sid)
        # On receiving messages on flask socketio server
        @sio.on("message")
        def my_message(sid, data):
            print('message ', data)
            s.send(bytes(data, 'utf8'))

        # On disconnect events
        @sio.event
        def disconnect(sid):
            print('disconnect ', sid)
        # Start the server listening process. Blocks current thread

        eventlet.wsgi.server(eventlet.listen(('', 5000)), self.app)


if __name__ == "__main__":
    useCybot = True
    if useCybot:
        global s
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Open socket to CyBot
        s.connect(("192.168.1.1", 288))
        print("Connected")
        eventlet.spawn(listen, s) # Start listen thread
    else:
        eventlet.spawn(listenFake) # Start listen thread
    server = CyBotSocketServer() # Create and start flask server

///<reference path="Constants.ts"/>


import {Injectable} from '@angular/core';
import io from 'socket.io-client'
import {BehaviorSubject, map, Observable, ReplaySubject, Subject} from "rxjs";


/**
 * @Author Coby Konkol
 * Component for managing socket with Flask Server
 */
@Injectable({
  providedIn: 'root'
})
export class SocketioService {





private _socket;
sharedMessage = new ReplaySubject<string>(); // Provides access to message events other components

  constructor() {
    // Create socket with Flask SocketIO server
    this._socket = io('127.0.0.1:5000', {
      reconnectionDelay: 1000,
      reconnection: true,
      reconnectionAttempts: 10,
      transports: ['websocket'],
      agent: false,
      rejectUnauthorized: false,
      path: '',
    });

    console.log('connecting');

    // connection events
    this._socket.on("connect_error", (err) => {
      console.log(`connect_error due to ${err.message}`);
      this._socket.emit("Connected!");
    });
    this._socket.on("connection", (socket) => {
      console.log("Connected");
    });
    // On message events
    this.socket.on("message", (msg) =>{
      this.sharedMessage.next(msg);
      }
    );

  }

// Send message over socket through Flask server
  sendMessage(message: string){
    console.log("sending: " + message);
    this._socket.emit('message', message);
  }

  // accessor for socket
  get socket() {
    return this._socket;
  }




}

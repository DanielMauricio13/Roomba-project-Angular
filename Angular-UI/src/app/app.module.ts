import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppComponent } from './app.component';
import { CanvasComponent } from './canvas/canvas.component';
import { TallObstacle } from './models/TallObstacle';
import { Cliff } from './models/Cliff';
import {ScenarioService} from "./scenario.service";
import {SocketioService} from "./socketio.service";
import {SocketIoConfig, SocketIoModule} from "ngx-socket-io";
const config: SocketIoConfig = { url: "localhost:4200", options: {} };

@NgModule({
  declarations: [
    AppComponent,
    CanvasComponent
  ],
  imports: [
    BrowserModule,
    SocketIoModule.forRoot(config)
  ],
  providers: [
    ScenarioService,
    SocketioService,

  ],
  bootstrap: [AppComponent]
})
export class AppModule { }

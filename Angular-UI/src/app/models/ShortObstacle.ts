import { Component, OnInit } from '@angular/core';
import {fieldWidth} from "../Constants";
import {Drawable} from "./Drawable";
///<reference path="../Constants.ts"/>

/**
 * @Author Coby Konkol
 * Represents short obstacles detected by the bump sensors
 * Stores X, Y, and width
 */
export class ShortObstacle extends Drawable{



  constructor(xCm: number, yCm: number) {
    super(xCm, yCm, 13);
    this.color = "#000088";
  }
}

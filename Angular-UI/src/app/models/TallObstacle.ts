import { Component, OnInit } from '@angular/core';
import {fieldWidth} from "../Constants";
import {Drawable} from "./Drawable";
///<reference path="../Constants.ts"/>

/**
 * @Author Coby Konkol
 * Represents objects found by IR sensor
 * Stores X, and Y, and width for a single tall obstacle
 */
export class TallObstacle extends Drawable{



  constructor(xCm: number, yCm: number, width: number) {
    super(xCm, yCm, width);
    this.color = "#0000FF";
  }
}

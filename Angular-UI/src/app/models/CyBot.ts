///<reference path="../Constants.ts"/>


import {Drawable} from "./Drawable";
import {scale} from "../Constants";

/**
 * @Author Coby Konkol
 * Manage CyBot location, angle, and drawing
 */
export class CyBot extends Drawable{

  private _angle: number; // angle in degrees

  constructor( xCm: number,  yCm: number) {
    super(xCm, yCm, 20);
    this.color = "#00AA00"
    this._angle = 0;
  }

  /**
   * @Author Coby Konkol
   * Called when the CyBot is displaced a certain number of cm. Calculates the new X and Y locations.
   * @param cm
   */
  public move(cm:number){
    let deltaX = Math.cos(this._angle * Math.PI / 180) * cm;
    let deltaY = Math.sin(this._angle * Math.PI / 180) * cm;
    this.xCm = this.xCm + deltaX;
    this.yCm = this.yCm + deltaY;
    //TODO TEST
  }

  /**
   * @Author Coby Konkol
   * Turns the CyBot a number of degrees
   * @param angle
   */
  public turn(angle:number){
    let mAngle = this._angle + angle;
    //  Force angle to between +- 180
    if(mAngle > 180){
      mAngle = -1 * (360 - mAngle);
    }
    else if(mAngle < -180){
      mAngle = -1 * (-360 - mAngle);
    }
    this._angle = mAngle;
    //TODO TEST
  }

  override draw(ctx: CanvasRenderingContext2D) {
    ctx.fillStyle = this.color;
    ctx.arc(this.xCm * scale, this.yCm * scale, this.width * scale, 0, 2 * Math.PI);
    ctx.fill();
    this.draw_arrow(ctx, this.xCm* scale, this.yCm * scale, this._angle, 15);
    ctx.stroke();
  }

  get angle(){
    return this._angle;
  }

}

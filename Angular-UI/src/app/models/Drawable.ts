import {scale} from "../Constants";

/**
 * @Author Coby Konkol
 * Base class for all elements that should be drawn by the CyBot
 * Provides color, x and y management, and drawing. Each of which can be overridden
 */
export class Drawable {
  protected color: string = "3370d4"


  constructor(protected xCm: number, protected yCm: number, protected width: number) {
  }

  /**
   * Draw the drawable. Note we use the y axis to represent the x axis for the bot for math purposes
   * @param ctx
   */
  draw(ctx: CanvasRenderingContext2D) {

    ctx.beginPath();
    ctx.fillStyle = this.color;
    ctx.arc(this.xCm * scale, this.yCm * scale, this.width * scale, 0, 2 * Math.PI);
    ctx.fill();
    ctx.stroke();
  }

   setX(_x: number){
    this.xCm = _x
  }
  setY(_y: number){
    this.yCm = _y;
  }
  get XCm(){
    return this.xCm;
  }
  get YCm(){
    return this.yCm;
  }

  draw_arrow(ctx: CanvasRenderingContext2D, fromx: number, fromy: number, angle: number, r: number) {
    ctx.beginPath();

    let headLen = 10;
    let toX = Math.cos(angle * Math.PI / 180) * r + fromx;
    let toY = Math.sin(angle * Math.PI / 180)* r + fromy;

    ctx.moveTo(fromx, fromy);
    ctx.lineTo(toX, toY);

    ctx.lineTo( toX - headLen * Math.cos(angle * Math.PI / 180 - Math.PI/6), toY - headLen * Math.sin(angle * Math.PI / 180 - Math.PI / 6));
    ctx.moveTo(toX, toY);
    ctx.lineTo( toX - headLen * Math.cos(angle * Math.PI / 180 + Math.PI/6), toY - headLen * Math.sin(angle * Math.PI / 180 + Math.PI / 6));
    ctx.stroke();
  }
}

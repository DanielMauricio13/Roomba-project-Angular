import {Injectable} from '@angular/core';
import {TallObstacle} from "./models/TallObstacle";
import {Cliff} from './models/Cliff';
import {CyBot} from "./models/CyBot";
import {SocketioService} from "./socketio.service";
import {scale} from "./Constants";
import {ShortObstacle} from "./models/ShortObstacle";
import {BehaviorSubject} from "rxjs";

/**
 * @Author Coby Konkol
 * Scenario for the CyBot.
 * Stores array of each type of obstacle, cybot instance, and accesses the socket
 */
@Injectable({
  providedIn: 'root'
})
export class ScenarioService {


  private _tallObstacles: TallObstacle[];
  private _shortObstacles: ShortObstacle[];
  private _cliffs: Cliff[];
  private _cyBot!: CyBot;
  private _moving: boolean;
  updateGui: BehaviorSubject<boolean> = new BehaviorSubject<boolean>(false);// flag for updating GUI events


  /**
   * Initializes scenario to match initial conditions of the CyBot
   * @param socketService
   */
  constructor(private socketService: SocketioService) {

    this._tallObstacles = [];
    this._shortObstacles = [];
    this._cliffs = [];

    this.cyBot = new CyBot(20, 20);
    // listener for when the socket receives a complete command from the CyBot
    this.socketService.sharedMessage.subscribe(value => {
      this.receiveMessage(value);
      this.updateGui.next(true);

    });
    this._moving = false;
  }

  get cyBot(): CyBot {
    return this._cyBot;
  }

  set cyBot(value: CyBot) {
    this._cyBot = value;
  }

  get tallObstacles(): TallObstacle[] {
    return this._tallObstacles;
  }

  addTallObstacle(obstacle: TallObstacle) {
    this.tallObstacles.push(obstacle);
  }

  get shortObstacles(): ShortObstacle[]{
    return this._shortObstacles;
  }
  addShortObstacle(xCm:number, yCm:number){
    let obstacle = new ShortObstacle(xCm, yCm);
    this._shortObstacles.push(obstacle);
  }

  get cliffs(): Cliff[] {
    return this._cliffs;
  }
  addCliff(xCm:number, yCm:number){
    this._cliffs.push(new Cliff(xCm, yCm));
  }

  /**
   * @Author Coby Konkol
   * Draw all drawable elements on the canvas
   * @param ctx
   */
  drawElements(ctx: CanvasRenderingContext2D){



    this.cyBot.draw(ctx);

    this.tallObstacles.forEach((obstacle) =>{
      obstacle.draw(ctx);
    });

    this.shortObstacles.forEach((obstacle) =>{
      obstacle.draw(ctx);
    });

    this.cliffs.forEach((cliff) =>{
      cliff.draw(ctx);
    });

  }


  // MOUSE INTERACTIONS:

  /**
   * @Author Coby Konkol
   * The user clicked a location on the screen
   * @param ev
   */
  onClick(ev:MouseEvent){
    console.log(" " + ev.offsetX + " " + ev.offsetY);


      let newXCm = (ev.x) / scale;
      let newYCm = (ev.y) / scale;

      let mAngle = this._cyBot.angle;

      console.log("Clicked " + newXCm + ", " + newYCm);

      this.goToXYCM(newXCm, newYCm);



  }

  /**
   * @Author Daniel Vergara
   * The user right-clicked a location on the screen, freeing the sensors
   * @param ev
   */
  onRightClick(ev:MouseEvent){
    this.socketService.sendMessage("f" +"\r\n");
    return false;
  }

  /**
   * @Author Nate Trotter
   * Calculate angle and distance to send to CyBot based on mouse click input
   * @param xCm
   * @param yCm
   */
  goToXYCM(xCm: number, yCm: number){
    let cyX = this._cyBot.XCm;
    let cyY = this._cyBot.YCm;

    let xChange = xCm - cyX;
    let yChange = yCm - cyY;



    let distance = Math.sqrt(xChange * xChange + yChange * yChange) - 20;

    let angleChange = Math.atan2(yChange, xChange); //math.atan2 takes delta y, delta x
    let cyBotAngle = this._cyBot.angle;

    let newAngle = ((angleChange * 180 / Math.PI) - cyBotAngle);

    if(newAngle > 180){
      newAngle = -1 * (360 - newAngle);
    }
    else if(newAngle < -180){
      newAngle = -1 * (-360 - newAngle);
    }
    console.log("Found: " + angleChange + "xchange, ychange" + xChange + " "  + yChange);
    console.log("Found: angle: " + newAngle + ", distance " + distance);
    this.socketService.sendMessage("" + parseInt(String(newAngle))+ ","+ parseInt(String(distance)) + "q" +"\r\n");
    this._moving = true;

  }

  //MESSAGES
  /**
   * @Author Coby Konkol
   * Receive general message (called from subscription) over socket
   * @param msg
   */
  receiveMessage(msg: string){
    msg = msg.trim();
    msg = msg.replace("START", "");
    msg = msg.replace("END", "");
    msg = msg.trim();


    if(msg.startsWith("obj")){
      this.getTallObstacleMessage(msg);
    }
    else if(msg.startsWith("bump")){
      this.getBumpMessage(msg);
    }
    else if(msg.startsWith("cliff")){
      this.getCliffMessage(msg);
    }
    else if(msg.startsWith("move")){
      this.getAmountMoved(msg);
    }
    else if(msg.startsWith("turn")){
      this.getAmountTurned(msg);
    }
    else if(msg.startsWith("READY")){
      this.getReadyMessage(msg);
    }


  }

  /**
   * @Author Daniel Vergara
   * Found a tall obstacle using IR sensor
   * Called when message of format "START obj <distance> END"
   * @param msg
   */
  getTallObstacleMessage(msg: string){
    // START obj <angle>, <distance>, <width> END

    msg = msg.replace("obj", "");
    msg = msg.trim();


    let textTemp1= msg.substring(0, msg.indexOf(','));
    let angle= parseInt(textTemp1);
    msg= msg.replace(textTemp1+ ',', "");
    textTemp1= msg.substring(0, msg.indexOf(','));
    let distance= parseInt(textTemp1);
    msg= msg.replace(textTemp1+ ',', "");
    let width= parseInt(msg);
    let tempX;
    let tempY;
    console.log("Received Object at angle:"+ angle + " " + " at distance: " + distance + " with a width of: " + width);
    let tempAng = angle + this._cyBot.angle; //in degrees
    if (tempAng > 180){
        tempAng -= 360;
    }
    else if (tempAng < -180){
        tempAng += 360;
    }

    tempX = this._cyBot.XCm + (distance+16) * Math.cos(tempAng * Math.PI / 180);
    tempY = this._cyBot.YCm + (distance+16) * Math.sin(tempAng * Math.PI / 180);

    console.log("tall Object: x: " +tempX + " y " + tempY +"\n\t Cybot: " + this._cyBot.XCm + " " +this._cyBot.YCm + " " + this._cyBot.angle);
    this._tallObstacles.push(new TallObstacle(tempX, tempY, width))

  }

  /**
   * @Author Daniel Vergara
   * Robot bumped into an obstacle
   * Called whe message of format "START BUMP {l/r/b} END"
   * @param msg
   */
  getBumpMessage(msg: string){
    msg = msg.replace("bump", "");
    msg = msg.trim();
    console.log("Bump:\n\tCybot Location" + this._cyBot.XCm + " " + this._cyBot.YCm + " " + this._cyBot.angle + "\n\tBump: " + msg);
    let bumpOffset = 16; //offset for x or y direction (2b^2 = ((32+13)/2)^2 where {32=len of bot} and {13=len of short obj})

    if(msg == 'r'){
      if( 45 <= this._cyBot.angle && this._cyBot.angle <= 135) //bot facing up
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm - bumpOffset, this._cyBot.YCm + bumpOffset));

      else if(-135 <= this.cyBot.angle &&  this._cyBot.angle <= -45) //bot facing down
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm + bumpOffset, this.cyBot.YCm - bumpOffset));

      else if(-45 < this._cyBot.angle && this._cyBot.angle < 45) //bot facing right
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm + bumpOffset, this.cyBot.YCm + bumpOffset));

      else if(135 < this._cyBot.angle && this._cyBot.angle < 225) //bot facing left
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm - bumpOffset , this.cyBot.YCm - bumpOffset));
    }
    else if(msg == 'l'){
      if( 45 <= this._cyBot.angle && this._cyBot.angle <= 135) //bot facing up
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm + bumpOffset, this._cyBot.YCm + bumpOffset));

      else if(-135 <= this.cyBot.angle &&  this._cyBot.angle <= -45) //bot facing down
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm - bumpOffset, this.cyBot.YCm - bumpOffset));

      else if(-45 < this._cyBot.angle && this._cyBot.angle < 45) //bot facing right
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm + bumpOffset, this.cyBot.YCm - bumpOffset));

      else if(135 < this._cyBot.angle && this._cyBot.angle < 225) //bot facing left
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm - bumpOffset , this.cyBot.YCm + bumpOffset));
    }

    else if(msg == 'b'){
      bumpOffset = 22; //offset for x or y direction (2b = (32+13)/2 where {32=len of bot} and {13=len of short obj})
      if( 45 <= this._cyBot.angle && this._cyBot.angle <= 135) //bot facing up
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm, this._cyBot.YCm + bumpOffset));

      else if(-135 <= this.cyBot.angle &&  this._cyBot.angle <= -45) //bot facing down
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm, this.cyBot.YCm - bumpOffset));

      else if(-45 < this._cyBot.angle && this._cyBot.angle < 45) //bot facing right
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm + bumpOffset, this.cyBot.YCm));

      else if(135 < this._cyBot.angle && this._cyBot.angle < 225) //bot facing left
        this._shortObstacles.push(new ShortObstacle(this._cyBot.XCm - bumpOffset , this.cyBot.YCm));
    }
    else{
      this.getUnknownMessage("bump " +msg);
    }
  }
  /**
   * @Author Coby Konkol
   * Robot drove a certain distance from current position
   * Called wHen message of format "START amount moved <distance> END
   * @param msg
   */
  getAmountMoved(msg: string){
    msg = msg.replace("move", "");
    msg = msg.trim();
    console.log("Moved: " + msg);

    var dist = Number(msg);
    this._cyBot.move(dist);

    //TODO TEST
  }

  /**
   * @Author Coby Konkol
   * CyBot turned a certain angle from current position
   * Called when message format "START amount turned <angle> END"
   * @param msg
   */
  getAmountTurned(msg: string){
    msg = msg.replace("turn", "");
    msg = msg.trim();
    console.log("Turned: " + msg);
    var angle = Number(msg);

    this._cyBot.turn(angle);
    //TODO TEST

  }

  /**
   * @Author Murali Patibandla
   * CyBot detected a cliff at the corresponding sensor
   * Called when message format "START cliff {ll/ml/mr/rr} END"
   * @param msg
   */
  getCliffMessage(msg: string){
    msg = msg.replace("cliff", "");
    msg = msg.trim();


    let cyX = this._cyBot.XCm;
    let cyY = this._cyBot.YCm;

    let clX: number;
    let clY: number;

    let cyBotAngle = this._cyBot.angle;

    clX=cyX;
    clY=cyY;

  if(msg == "ll"){
      if( this._cyBot.angle > 45 && this._cyBot.angle <= 135)
        this._cliffs.push(new Cliff(this._cyBot.XCm +14, this._cyBot.YCm ));
      else if(this.cyBot.angle > -135 &&  this._cyBot.angle < -45)
         this._cliffs.push(new Cliff(this._cyBot.XCm -14, this._cyBot.YCm ));
      else if((this._cyBot.angle > 0 && this._cyBot.angle < 45) || (this._cyBot.angle < -135))
        this._cliffs.push(new Cliff(this._cyBot.XCm , this.cyBot.YCm -14));
      else if(this._cyBot.angle > 135 || (this._cyBot.angle < 0 && this._cyBot.angle >= -45))
        this._cliffs.push(new Cliff(this._cyBot.XCm , this.cyBot.YCm +14));
  }
  else if( msg== "ml"){
      if( this._cyBot.angle > 45 && this._cyBot.angle <= 135)
        this._cliffs.push(new Cliff(this._cyBot.XCm +14, this._cyBot.YCm +2));
      else if(this.cyBot.angle > -135 &&  this._cyBot.angle < -45)
         this._cliffs.push(new Cliff(this._cyBot.XCm -14, this._cyBot.YCm  -2));
      else if((this._cyBot.angle > 0 && this._cyBot.angle < 45) || (this._cyBot.angle < -135))
        this._cliffs.push(new Cliff(this._cyBot.XCm + 14, this.cyBot.YCm -2));
      else if(this._cyBot.angle > 135 || (this._cyBot.angle < 0 && this._cyBot.angle >= -45))
        this._cliffs.push(new Cliff(this._cyBot.XCm -14, this.cyBot.YCm +2));
  }
  else if( msg== "mr"){
     if( this._cyBot.angle > 45 && this._cyBot.angle <= 135)
        this._cliffs.push(new Cliff(this._cyBot.XCm -2, this._cyBot.YCm +14));
      else if(this.cyBot.angle > -135 &&  this._cyBot.angle < -45)
         this._cliffs.push(new Cliff(this._cyBot.XCm +14, this._cyBot.YCm -2));
      else if((this._cyBot.angle > 0 && this._cyBot.angle < 45) || (this._cyBot.angle < -135))
        this._cliffs.push(new Cliff(this._cyBot.XCm + 2, this.cyBot.YCm +14));
      else if(this._cyBot.angle > 135 || (this._cyBot.angle < 0 && this._cyBot.angle >= -45))
        this._cliffs.push(new Cliff(this._cyBot.XCm -2, this.cyBot.YCm -14));
  }
  else if(msg== "rr"){
     if( this._cyBot.angle > 45 && this._cyBot.angle <= 135)
        this._cliffs.push(new Cliff(this._cyBot.XCm -14, this._cyBot.YCm ));
      else if(this.cyBot.angle > -135 &&  this._cyBot.angle < -45)
         this._cliffs.push(new Cliff(this._cyBot.XCm +14, this._cyBot.YCm ));
      else if((this._cyBot.angle > 0 && this._cyBot.angle < 45) || (this._cyBot.angle < -135))
        this._cliffs.push(new Cliff(this._cyBot.XCm , this.cyBot.YCm +14));
      else if(this._cyBot.angle > 135 || (this._cyBot.angle < 0 && this._cyBot.angle >= -45))
        this._cliffs.push(new Cliff(this._cyBot.XCm , this.cyBot.YCm -14));
  }
  else {
      if( this._cyBot.angle > 45 && this._cyBot.angle <= 135)
        this._cliffs.push(new Cliff(this._cyBot.XCm , this._cyBot.YCm +3 ));
      else if(this.cyBot.angle > -135 &&  this._cyBot.angle < -45)
         this._cliffs.push(new Cliff(this._cyBot.XCm , this._cyBot.YCm -3 ));
      else if((this._cyBot.angle > 0 && this._cyBot.angle < 45) || (this._cyBot.angle < -135))
        this._cliffs.push(new Cliff(this._cyBot.XCm  +3, this.cyBot.YCm ));
      else if(this._cyBot.angle > 135 || (this._cyBot.angle < 0 && this._cyBot.angle >= -45))
        this._cliffs.push(new Cliff(this._cyBot.XCm -3 , this.cyBot.YCm));
  }

    console.log("Detected Cliff at Sensor:\n\tCybot: " + cyX + " " + cyY + " " + cyBotAngle + "\n\tcliff: " + clX + " " + clY + " " + msg);

  }

  /**
   * @Author Coby Konkol
   * CyBot is ready to receive more instructions
   * Called when message format "START READY END"
   * @param msg
   */
  getReadyMessage(msg: string){
    console.log("READY RECEIVED");
    this._moving = false;
  }

  /**
   * @Author Coby Konkol
   * CyBot sent a message that is not recognized by the code
   * @param msg
   */
  getUnknownMessage(msg: string){
    console.log("UNKNOWN MESSAGE: " + msg);
  }
}

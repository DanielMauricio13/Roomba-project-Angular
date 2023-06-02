import {Component, ElementRef, OnInit, ViewChild} from '@angular/core';
import {animate} from "@angular/animations";
import {TallObstacle} from "../models/TallObstacle";
import {CyBot} from "../models/CyBot";
import {ScenarioService} from "../scenario.service";

/**
 * @Author Coby Konkol
 * Manages screen and on click listeners
 */
@Component({
  selector: 'app-canvas',
  templateUrl: './canvas.component.html',
  styleUrls: ['./canvas.component.css']
})
export class CanvasComponent implements OnInit {



  @ViewChild('canvas', { static: true })
  canvas!: ElementRef<HTMLCanvasElement>;

  private ctx!: CanvasRenderingContext2D;


  constructor(private scenario: ScenarioService) {
    this.scenario.updateGui.subscribe((value) => {
      if(value){
        this.animate();
      }
    })

  }

  /**
   * Initializes listeners for clicking and initializes canvas
   */
  ngOnInit(): void {
    this.ctx = this.canvas.nativeElement.getContext('2d')!;

    this.canvas.nativeElement.addEventListener("click", ev => {
        this.scenario.onClick(ev);
    });
    this.canvas.nativeElement.addEventListener("contextmenu", ev=> {
        ev.preventDefault();
        this.scenario.onRightClick(ev);
        return false;
    });

    this.animate();
  }

  /**
   * Draw all elements, utilizes scenario.drawElements
   */
  animate(): void {
    this.ctx.clearRect(0, 0, this.canvas.nativeElement.width, this.canvas.nativeElement.height);
    this.scenario.drawElements(this.ctx);
    this.ctx.beginPath();
  }




}

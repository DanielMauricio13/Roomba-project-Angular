package model;/**
 * @author Coby Konkol
 */

public class Obstacle {
     private float xPosCm;
     private float yPosCm;
     private float widthCm;

    public Obstacle(float xPosCm, float yPosCm, float widthCm) {
        this.xPosCm = xPosCm;
        this.yPosCm = yPosCm;
        this.widthCm = widthCm;
    }

    public float getxPosCm() {
        return xPosCm;
    }

    public void setxPosCm(float xPosCm) {
        this.xPosCm = xPosCm;
    }

    public float getyPosCm() {
        return yPosCm;
    }

    public void setyPosCm(float yPosCm) {
        this.yPosCm = yPosCm;
    }

    public float getWidthCm() {
        return widthCm;
    }

    public void setWidthCm(float widthCm) {
        this.widthCm = widthCm;
    }
}

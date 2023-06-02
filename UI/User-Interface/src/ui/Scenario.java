package ui;

import model.Obstacle;
import ui.MainPanel;

import javax.swing.*;
import java.util.ArrayList;

/**
 * This class manages the input stream, output stream and user interface
 * @author Coby Konkol
 */

public class Scenario extends JFrame {

//    Data/CyBot focused fields
    private float botPositionCm;
    private ArrayList<Obstacle> obstacles;


    // UI focused fields
    private int width, height;
    private MainPanel mainPanel;


    public Scenario(String title, int width, int height){
        super(title);
        this.width = width;
        this.height = height;


        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setSize(width, height);
        this.setVisible(true);

    }
    /**
     * Determine obstacle's absolute position using bot's absolute position and object relative position
     * Once obstacle is added to model, repaint UI
     * @param angle in degrees
     * @param linearWidthCm
     * @param distanceCm
     */
    public void detectObstacle(float angle, float linearWidthCm, float distanceCm){

    }
}

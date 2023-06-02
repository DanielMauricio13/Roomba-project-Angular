package ui;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

/**
 * @author Coby Konkol
 */

public class MainPanel extends JPanel implements MouseListener {
    private Scenario scenario;

    Image img;
    public MainPanel(Scenario scenario){
        this.addMouseListener(this);
        this.scenario = scenario;


    }

    /*
    Draw bot, draw obstacles
     */
    public void paintComponents(Graphics g){

    }


    /**
     * If valid click (inside screen), calculate directions for bot, then send instructions to bot
     * @param e the event to be processed
     */
    @Override
    public void mouseClicked(MouseEvent e) {
        int x = e.getX();
        int y = e.getY();
    }

    @Override
    public void mousePressed(MouseEvent e) {

    }

    @Override
    public void mouseReleased(MouseEvent e) {


    }

    @Override
    public void mouseEntered(MouseEvent e) {

    }

    @Override
    public void mouseExited(MouseEvent e) {

    }


}

package network;/**
 * @author Coby Konkol
 */

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

/**
 * Asynchronous thread, receives messages from the CyBot, processes messages, and queues appropriate action
 */
public class CybotInputStream {
    private Socket socket;
    private InputStream inputStream;

    public CybotInputStream(Socket socket, InputStream inputStream) {
        this.socket = socket;
        this.inputStream = inputStream;
    }

    public void read() throws IOException {
        if(inputStream.available() > 0){
            String s = new String(inputStream.readAllBytes());

        }
    }
}

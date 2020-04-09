package MyApp.Disgust;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;
import MyApp.Emotion.Emotion;


//======================================================================
// Disgust
public class Disgust extends Emotion {
    //------------------------------------------------------------
    // Disgust
    public Disgust(String id, String neighbor, AppKickstarter appKickstarter) {
	super(id, neighbor, appKickstarter);
    } // Disgust


    //------------------------------------------------------------
    // handleHello
    protected void handleHello(Msg msg) {
        log.info("Hello " + msg.getSender() + "!  This is Disgust.  Now what??  What do you want??");
        neighborMBox.send(new Msg(id, mbox, Msg.Type.Hello, "Hey, what do you want from me?"));
    } // handleHello
} // Disgust

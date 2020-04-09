package MyApp.Sadness;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;
import MyApp.Emotion.Emotion;


//======================================================================
// Sadness
public class Sadness extends Emotion {
    //------------------------------------------------------------
    // Sadness
    public Sadness(String id, String neighbor, AppKickstarter appKickstarter) {
	super(id, neighbor, appKickstarter);
    } // Sadness


    //------------------------------------------------------------
    // handleHello
    protected void handleHello(Msg msg) {
        log.info("Hello " + msg.getSender() + "!  This is Sadness.  Why is the sky so blue....  =(");
        neighborMBox.send(new Msg(id, mbox, Msg.Type.Hello, "Why?  Why???  Why is the sky always blue???"));
    } // handleHello
} // Sadness

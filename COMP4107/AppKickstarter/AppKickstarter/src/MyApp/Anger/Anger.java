package MyApp.Anger;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;
import MyApp.Emotion.Emotion;


//======================================================================
// Anger
public class Anger extends Emotion {
    //------------------------------------------------------------
    // Anger
    public Anger(String id, String neighbor, AppKickstarter appKickstarter) {
	super(id, neighbor, appKickstarter);
    } // Anger


    //------------------------------------------------------------
    // handleHello
    protected void handleHello(Msg msg) {
        log.info("Hello " + msg.getSender() + "!  This is Anger.  I am always ANGRY!!!");
        neighborMBox.send(new Msg(id, mbox, Msg.Type.Hello, "Hi there, I am mad!"));
    } // handleHello
} // Anger

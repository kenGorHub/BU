package MyApp.Joy;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;
import MyApp.Emotion.Emotion;


//======================================================================
// Joy
public class Joy extends Emotion {
    //------------------------------------------------------------
    // Joy
    public Joy(String id, String neighbor, AppKickstarter appKickstarter) {
	super(id, neighbor, appKickstarter);
    } // Joy


    //------------------------------------------------------------
    // handleHello
    protected void handleHello(Msg msg) {
        log.info("Hello " + msg.getSender() + "!  This is Joy.  La, la, la, la, lah!  I am HAPPY!!!");
        neighborMBox.send(new Msg(id, mbox, Msg.Type.Hello, "Hi there, aren't your happy??  =)"));
    } // handleHello
} // Joy

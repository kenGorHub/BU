package MyApp.Fear;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;
import MyApp.Emotion.Emotion;


//======================================================================
// Fear
public class Fear extends Emotion {
    //------------------------------------------------------------
    // Fear
    public Fear(String id, String neighbor, AppKickstarter appKickstarter) {
	super(id, neighbor, appKickstarter);
    } // Fear


    //------------------------------------------------------------
    // handleHello
    protected void handleHello(Msg msg) {
        log.info("Hello " + msg.getSender() + "!  This is Fear.  I am SCARED!!!");
        neighborMBox.send(new Msg(id, mbox, Msg.Type.Hello, "Hey there!  Look who's behind you!!  =O"));
    } // handleHello
} // Fear

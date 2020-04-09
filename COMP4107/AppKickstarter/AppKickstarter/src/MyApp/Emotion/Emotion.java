package MyApp.Emotion;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;


//======================================================================
// Emotion
public class Emotion extends AppThread {
    protected final String neighbor;
    protected final MBox neighborMBox;

    //------------------------------------------------------------
    // Emotion
    public Emotion(String id, String neighbor, AppKickstarter appKickstarter) {
	super(id, appKickstarter);
	this.neighbor = neighbor;
	neighborMBox = appKickstarter.getThread(this.neighbor).getMBox();
    } // Emotion


    //------------------------------------------------------------
    // run
    public void run() {
	Thread.currentThread().setName(id);
	log.info(id + ": starting...");

	for (boolean quit = false; !quit;) {
	    Msg msg = mbox.receive();

	    log.fine(id + ": message received: [" + msg + "].");

	    switch (msg.getType()) {
		case Hello:
		    handleHello(msg);
		    break;

		case Terminate:
		    quit = true;
		    break;

		default:
		    log.warning(id + ": unknown message type: [" + msg + "]");
	    }
	}

	// declaring our departure
	appKickstarter.unregThread(this);
	log.info(id + ": terminating...");
    } // run


    //------------------------------------------------------------
    // handleHello
    protected void handleHello(Msg msg) {
	log.info("Hello!  This is Emotion.  I am so emotional!!");
    } // handleHello
} // Emotion

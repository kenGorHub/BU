package MyApp.Riley;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;
import AppKickstarter.timer.Timer;


//======================================================================
// Riley
public class Riley extends AppThread {
    private MBox angerMBox;
    private final int pollTime;

    //------------------------------------------------------------
    // Riley
    public Riley(String id, AppKickstarter appKickstarter) throws Exception {
	super(id, appKickstarter);
	this.pollTime = Integer.parseInt(appKickstarter.getProperty("Riley.HelloTime"));
    } // Riley


    //------------------------------------------------------------
    // run
    public void run() {
	Thread.currentThread().setName(id);
	log.info(id + ": starting...");
	Timer.setTimer(id, mbox, pollTime);

	angerMBox = appKickstarter.getThread("Anger").getMBox();

	for (boolean quit = false; !quit;) {
	    Msg msg = mbox.receive();

	    log.fine(id + ": message received: [" + msg + "].");

	    switch (msg.getType()) {
		case TimesUp:
		    Timer.setTimer(id, mbox, pollTime);
		    log.info("------------------------------------------------------------");
		    log.info(id + ": " + msg.getDetails() + "  Let's say Hello to Anger!");
		    angerMBox.send(new Msg(id, mbox, Msg.Type.Hello, "Hey, this is Riley!"));
		    break;

		case Hello:
		    log.info("Hello " + msg.getSender() + "!  This is Riley.  Nice meeting you!!");
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
} // Riley

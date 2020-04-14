package PCS.SensorHandler;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;


//======================================================================
// SensorHandler
public class SensorHandler extends AppThread {
    protected final MBox pcsCore;
    private SensorStatus sensorStatus;

    //------------------------------------------------------------
    // SensorHandler
    public SensorHandler(String id, AppKickstarter appKickstarter) {
		super(id, appKickstarter);
		pcsCore = appKickstarter.getThread("PCSCore").getMBox();
		sensorStatus = SensorStatus.SensorClosed;
    } // SensorHandler


    //------------------------------------------------------------
    // run
    public void run() {
		Thread.currentThread().setName(id);
		log.info(id + ": starting...");

		for (boolean quit = false; !quit;) {
			Msg msg = mbox.receive();

			log.fine(id + ": message received: [" + msg + "].");

			quit = processMsg(msg);
		}

		// declaring our departure
		appKickstarter.unregThread(this);
		log.info(id + ": terminating...");
    } // run

	protected boolean processMsg(Msg msg){
		System.out.println("processMsg");
		return false;
	}

    //------------------------------------------------------------
    // Gate Status
    private enum SensorStatus {
		SensorOpened,
		SensorClosed,
    }
} // GateHandler

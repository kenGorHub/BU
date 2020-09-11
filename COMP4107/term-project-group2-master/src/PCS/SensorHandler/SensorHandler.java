package PCS.SensorHandler;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;


//======================================================================
// SensorHandler
public class SensorHandler extends AppThread {
	protected int n = 3;
	int currentFloor = 0;
	protected Floor[] clonedfloor = new Floor[n+1];
    //------------------------------------------------------------
    // SensorHandler
    public SensorHandler(String id, AppKickstarter appKickstarter) {
		super(id, appKickstarter);
    } // SensorHandler


    //------------------------------------------------------------
    // run
    public void run() {
		
		Thread.currentThread().setName(id);
		log.info(id + ": starting...");
		FloorFactory.floorStructure(n); //build up floor
		for(int i = 0; i <= n; i++){
			clonedfloor[i] = FloorFactory.makeFloor(i); // copy floor to handler
		}

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
		boolean quit = false;

		switch (msg.getType()) {
			case FloorNumber: 
				currentFloor = Integer.parseInt(msg.getDetails());
				log.fine(id + ": Now floor: "+ msg.getDetails() + " Car number:" +clonedfloor[currentFloor].getCarNum());
				break;
			case GoingUp:
				clonedfloor[currentFloor].movement(msg.getDetails());
				log.fine(id + ": Car is going up.");
				break;
			case GoingDown:
				clonedfloor[currentFloor].movement(msg.getDetails());
				log.fine(id + ": Car is going down.");
				break;
			case Terminate:
				quit = true;
				break;
			default:
			log.warning(id + ": unknown message type: [" + msg + "]");
		}
		return quit;
	}

} // GateHandler

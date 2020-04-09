package PCS;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.Msg;
import AppKickstarter.timer.Timer;

import PCS.PCSCore.PCSCore;
import PCS.GateHandler.GateHandler;

import javafx.application.Platform;


//======================================================================
// PCSStarter
public class PCSStarter extends AppKickstarter {
    protected Timer timer;
    protected PCSCore pcsCore;
    protected GateHandler gateHandler;


    //------------------------------------------------------------
    // main
    public static void main(String [] args) {
        new PCSStarter().startApp();
    } // main


    //------------------------------------------------------------
    // PCSStart
    public PCSStarter() {
	super("PCSStarter", "etc/PCS.cfg");
    } // PCSStart


    //------------------------------------------------------------
    // startApp
    protected void startApp() {
	// start our application
	log.info("");
	log.info("");
	log.info("============================================================");
	log.info(id + ": Application Starting...");

	startHandlers();
    } // startApp


    //------------------------------------------------------------
    // startHandlers
    protected void startHandlers() {
	// create handlers
	try {
	    timer = new Timer("timer", this);
	    pcsCore = new PCSCore("PCSCore", this);
	    gateHandler = new GateHandler("GateHandler", this);
	} catch (Exception e) {
	    System.out.println("AppKickstarter: startApp failed");
	    e.printStackTrace();
	    Platform.exit();
	}

	// start threads
	new Thread(timer).start();
	new Thread(pcsCore).start();
	new Thread(gateHandler).start();
    } // startHandlers


    //------------------------------------------------------------
    // stopApp
    public void stopApp() {
	log.info("");
	log.info("");
	log.info("============================================================");
	log.info(id + ": Application Stopping...");
	pcsCore.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
	gateHandler.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
	timer.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
    } // stopApp
} // PCS.PCSStarter

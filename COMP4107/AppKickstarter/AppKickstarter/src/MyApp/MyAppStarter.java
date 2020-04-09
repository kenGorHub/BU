package MyApp;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.Msg;
import AppKickstarter.timer.Timer;

import MyApp.Riley.Riley;
import MyApp.Anger.Anger;
import MyApp.Disgust.Disgust;
import MyApp.Fear.Fear;
import MyApp.Joy.Joy;
import MyApp.Sadness.Sadness;

import javafx.application.Platform;


//======================================================================
// MyAppStarter
public class MyAppStarter extends AppKickstarter {
    protected Timer   timer;
    protected Riley   riley;
    protected Anger   anger;
    protected Disgust disgust;
    protected Fear    fear;
    protected Joy     joy;
    protected Sadness sadness;


    //------------------------------------------------------------
    // main
    public static void main(String [] args) {
        new MyAppStarter().startApp();
    } // main


    //------------------------------------------------------------
    // PCSStart
    public MyAppStarter() {
	super("MyAppStarter", "etc/MyApp.cfg");
    } // PCSStart


    //------------------------------------------------------------
    // startApp
    protected void startApp() {
	// start our application
	log.info("");
	log.info("");
	log.info("============================================================");
	log.info(id + ": Application Starting...");

	startAllThreads();
    } // startApp


    //------------------------------------------------------------
    // startAllThreads
    protected void startAllThreads() {
	// create threads
	try {
	    timer   = new Timer("timer", this);
	    riley   = new Riley("Riley", this);
	    sadness = new Sadness("Sadness", "Riley", this);
	    joy     = new Joy("Joy", "Sadness", this);
	    fear    = new Fear("Fear", "Joy", this);
	    disgust = new Disgust("Disgust", "Fear", this);
	    anger   = new Anger("Anger", "Disgust", this);
	} catch (Exception e) {
	    System.out.println("AppKickstarter: startApp failed");
	    e.printStackTrace();
	    Platform.exit();
	}

	// start threads
	new Thread(timer).start();
	new Thread(riley).start();
	new Thread(anger).start();
	new Thread(disgust).start();
	new Thread(fear).start();
	new Thread(joy).start();
	new Thread(sadness).start();
    } // startAllThreads


    //------------------------------------------------------------
    // stopApp
    public void stopApp() {
	log.info("");
	log.info("");
	log.info("============================================================");
	log.info(id + ": Application Stopping...");
	riley.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
	anger.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
	disgust.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
	fear.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
	joy.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
	sadness.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
	timer.getMBox().send(new Msg(id, null, Msg.Type.Terminate, "Terminate now!"));
    } // stopApp
} // MyAppStarter

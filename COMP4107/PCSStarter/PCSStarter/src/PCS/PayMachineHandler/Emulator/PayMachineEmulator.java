package PCS.PayMachineHandler.Emulator;

import AppKickstarter.misc.*;
import AppKickstarter.timer.Timer;

import PCS.PCSStarter;
import PCS.PayMachineHandler.PayMachineHandler;

import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.stage.StageStyle;
import javafx.stage.WindowEvent;


//======================================================================
// PayMachineEmulator
public class PayMachineEmulator extends PayMachineHandler {
    private Stage myStage;
    private PayMachineEmulatorController paymachineEmulatorController;
    private final PCSStarter pcsStarter;
    private final String id;
    private final int gateOpenTime;
    private final int gateCloseTime;
    private final int GateOpenTimerID = 1;
    private final int GateCloseTimerID = 2;
    private boolean autoRead_CNR;
    private boolean autoRead_AP;
    private boolean autoRead_UP;
	private boolean autoPay_S;
	private boolean autoPay_F;
	private boolean autoReturn;


    //------------------------------------------------------------
    // PayMachineEmulator
    public PayMachineEmulator(String id, PCSStarter pcsStarter) {
	super(id, pcsStarter);
	this.pcsStarter = pcsStarter;
	this.id = id + "Emulator";
	this.gateOpenTime = Integer.parseInt(this.pcsStarter.getProperty("Gate.GateOpenTime"));
	this.gateCloseTime = Integer.parseInt(this.pcsStarter.getProperty("Gate.GateCloseTime"));
	this.autoRead_CNR = true;
	this.autoRead_AP = true;
	this.autoRead_UP = true;
	this.autoPay_S = true;
	this.autoPay_F = true;
	this.autoReturn = true;
    } // PayMachineEmulator


    //------------------------------------------------------------
    // start
    public void start() throws Exception {
	Parent root;
	myStage = new Stage();
	FXMLLoader loader = new FXMLLoader();
	String fxmlName = "PayMachineEmulator.fxml";
	loader.setLocation(PayMachineEmulator.class.getResource(fxmlName));
	root = loader.load();
	paymachineEmulatorController = (PayMachineEmulatorController) loader.getController();
	paymachineEmulatorController.initialize(id, pcsStarter, log, this);
	myStage.initStyle(StageStyle.DECORATED);
	myStage.setScene(new Scene(root, 420, 470));
	myStage.setTitle("PayMachine Emulator");
	myStage.setResizable(false);
	myStage.setOnCloseRequest((WindowEvent event) -> {
	    pcsStarter.stopApp();
	    Platform.exit();
	});
	myStage.show();
    } // GateEmulator


    //------------------------------------------------------------
    // processMsg
    protected final boolean processMsg(Msg msg) {
        boolean quit = false;

	switch (msg.getType()) {
	    case TimesUp:
	        handleTimesUp(msg);
	        break;

	    case PayMachineEmulatorAutoRead_CNRToggle:
		handlePayMachineEmulatorAutoRead_CNRToggle();
		break;

	    case PayMachineEmulatorAutoRead_APToggle:
		handlePayMachineEmulatorAutoRead_APToggle();
		break;

	    case PayMachineEmulatorAutoRead_UPToggle:
		handlePayMachineEmulatorAutoRead_UPToggle();
	    break;

		case PayMachineEmulatorAutoPay_SToggle:
		handlePayMachineEmulatorAutoPay_SToggle();
		break;

		case PayMachineEmulatorAutoPay_FToggle:
		handlePayMachineEmulatorAutoPay_FToggle();
		break;

		case PayMachineEmulatorAutoReturnToggle:
		handlePayMachineEmulatorAutoReturnToggle();
		break;

	    default:
		quit = super.processMsg(msg);
	}
	return quit;
    } // processMsg


    //------------------------------------------------------------
    // sendGateOpenSignal
    @Override
    protected void sendGateOpenSignal() {
	logFine("Gate open signal received.  [autoOpen is " + (autoOpen ? "on]" : "off]"));
        if (autoOpen) {
	    logFine("Gate open timer started.");
	    Timer.setTimer(id, mbox, gateOpenTime, GateOpenTimerID);
	}
    } // sendGateOpenSignal


    //------------------------------------------------------------
    // sendGateCloseSignal
    @Override
    protected void sendGateCloseSignal() {
	logFine("Gate close signal received.  [autoClose is " + (autoClose ? "on]" : "off]"));
	if (autoClose) {
	    logFine("Gate close timer started.");
	    Timer.setTimer(id, mbox, gateCloseTime, GateCloseTimerID);
	}
    } // sendGateCloseSignal


    //------------------------------------------------------------
    // sendPollReq
    @Override
    protected void sendPollReq() {
	logFine("Poll request received.  [autoPoll is " + (autoPoll ? "on]" : "off]"));
	if (autoPoll) {
	    logFine("Send poll ack.");
	    mbox.send(new Msg(id, mbox, Msg.Type.PollAck, ""));
	}
    } // sendPollReq


    //------------------------------------------------------------
    // handleTimesUp
    public final void handleTimesUp(Msg msg) {
	logFine("Times up received.");

	switch (Timer.getTimesUpMsgTimerId(msg)) {
	    case GateOpenTimerID:
		logFine("Gate open timer is up.  [autoOpen is " + (autoOpen ? "on]" : "off]"));
	        if (autoOpen) {
		    logFine("Send gate open reply");
		    mbox.send(new Msg(id, mbox, Msg.Type.GateOpenReply, "Gate Open Reply"));
		} else {
	            // autoOpen is off.  just ignore timeout msg
		    logFine("Auto open is off.  Timer ignored.");
		}
		break;

	    case GateCloseTimerID:
		logFine("Gate close timer is up.  [autoClose is " + (autoClose ? "on]" : "off]"));
		if (autoClose) {
		    logFine("Send gate close reply");
		    mbox.send(new Msg(id, mbox, Msg.Type.GateCloseReply, "Gate Close Reply"));
		} else {
		    // autoClose is off.  just ignore timeout msg
		    logFine("Auto close is off.  Timer ignored.");
		}
		break;

	    default:
		logSevere("Unknown timer id!!");
	}
    } // handleTimesUp


    //------------------------------------------------------------
    // handlePayMachineEmulatorAutoRead_CNRToggle
    public final boolean handlePayMachineEmulatorAutoRead_CNRToggle() {
	autoRead_CNR = !autoRead_CNR;
	logFine("AutoRead_CNR change: " + (autoRead_CNR ? "off --> on" : "on --> off"));
	return autoRead_CNR;
    } // handlePayMachineEmulatorAutoRead_CNRToggle


    //------------------------------------------------------------
    // handlePayMachineEmulatorAutoRead_APToggle
    public final boolean handlePayMachineEmulatorAutoRead_APToggle() {
	autoRead_AP = !autoRead_AP;
	logFine("AutoRead_AP change: " + (autoRead_AP ? "off --> on" : "on --> off"));
	return autoRead_AP;
    } // handlePayMachineEmulatorAutoRead_APToggle


    //------------------------------------------------------------
    // handlePayMachineEmulatorAutoRead_UPToggle
    public final boolean handlePayMachineEmulatorAutoRead_UPToggle() {
    autoRead_UP = !autoRead_UP;
	logFine("AutoRead_UP change: " + (autoRead_UP ? "off --> on" : "on --> off"));
    return autoRead_UP;
    } // handlePayMachineEmulatorAutoRead_UPToggle


    //------------------------------------------------------------

	// handlePayMachineEmulatorAutoPay_SToggle
	public final boolean handlePayMachineEmulatorAutoPay_SToggle() {
		autoPay_S = !autoPay_S;
		logFine("AutoPay_S change: " + (autoPay_S ? "off --> on" : "on --> off"));
		return autoRead_UP;
	} // handlePayMachineEmulatorAutoPay_SToggle


	//------------------------------------------------------------


	// handlePayMachineEmulatorAutoPay_FToggle
	public final boolean handlePayMachineEmulatorAutoPay_FToggle() {
		autoPay_F = !autoPay_F;
		logFine("AutoPay_F change: " + (autoPay_F ? "off --> on" : "on --> off"));
		return autoPay_F;
	} // handlePayMachineEmulatorAutoPay_FToggle


	//------------------------------------------------------------


	// handlePayMachineEmulatorAutoReturnToggle
	public final boolean handlePayMachineEmulatorAutoReturnToggle() {
		autoReturn = !autoReturn;
		logFine("AutoReturn change: " + (autoReturn ? "off --> on" : "on --> off"));
		return autoRead_UP;
	} // PayMachineEmulatorAutoReturnToggle


	//------------------------------------------------------------


    // logFine
    private final void logFine(String logMsg) {
	paymachineEmulatorController.appendTextArea("[FINE]: " + logMsg);
	log.fine(id + ": " + logMsg);
    } // logFine


    //------------------------------------------------------------
    // logInfo
    private final void logInfo(String logMsg) {
	paymachineEmulatorController.appendTextArea("[INFO]: " + logMsg);
	log.info(id + ": " + logMsg);
    } // logInfo


    //------------------------------------------------------------
    // logWarning
    private final void logWarning(String logMsg) {
	paymachineEmulatorController.appendTextArea("[WARNING]: " + logMsg);
	log.warning(id + ": " + logMsg);
    } // logWarning


    //------------------------------------------------------------
    // logSevere
    private final void logSevere(String logMsg) {
	gateEmulatorController.appendTextArea("[SEVERE]: " + logMsg);
	log.severe(id + ": " + logMsg);
    } // logSevere
} // GateEmulator

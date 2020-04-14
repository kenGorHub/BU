package PCS.SensorHandler.Emulator;

import AppKickstarter.misc.*;
import AppKickstarter.timer.Timer;

import PCS.PCSStarter;
import PCS.SensorHandler.SensorHandler;

import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.stage.StageStyle;
import javafx.stage.WindowEvent;


//======================================================================
// SensorEmulator
public class SensorEmulator extends SensorHandler {
    private Stage myStage;
    private SensorEmulatorController sensorEmulatorController;
    private final PCSStarter pcsStarter;
    private final String id;
    private final int gateOpenTime;
    private final int gateCloseTime;
    private final int GateOpenTimerID = 1;
    private final int GateCloseTimerID = 2;
    private boolean autoOpen;
    private boolean autoClose;
    private boolean autoPoll;


    //------------------------------------------------------------
    // SensorEmulator
    public SensorEmulator(String id, PCSStarter pcsStarter) {
		super(id, pcsStarter);
		this.pcsStarter = pcsStarter;
		this.id = id + "Emulator";
		this.autoOpen = false;
		this.autoClose = false;
		this.autoPoll = false;
    } // SensorEmulator


    //------------------------------------------------------------
    // start
    public void start() throws Exception {
		Parent root;
		myStage = new Stage();
		FXMLLoader loader = new FXMLLoader();
		String fxmlName = "SensorEmulator.fxml";
		loader.setLocation(SensorEmulator.class.getResource(fxmlName));
		root = loader.load();
		sensorEmulatorController = (SensorEmulatorController) loader.getController();
		sensorEmulatorController.initialize(id, pcsStarter, log, this);
		myStage.initStyle(StageStyle.DECORATED);
		myStage.setScene(new Scene(root, 420, 470));
		myStage.setTitle("Sensor Emulator");
		myStage.setResizable(false);
		myStage.setOnCloseRequest((WindowEvent event) -> {
			pcsStarter.stopApp();
			Platform.exit();
		});
		myStage.show();
    } // SensorEmulator

    //------------------------------------------------------------
    // logFine
    private final void logFine(String logMsg) {
		sensorEmulatorController.appendTextArea("[FINE]: " + logMsg);
		log.fine(id + ": " + logMsg);
    } // logFine


    //------------------------------------------------------------
    // logInfo
    private final void logInfo(String logMsg) {
		sensorEmulatorController.appendTextArea("[INFO]: " + logMsg);
		log.info(id + ": " + logMsg);
    } // logInfo


    //------------------------------------------------------------
    // logWarning
    private final void logWarning(String logMsg) {
		sensorEmulatorController.appendTextArea("[WARNING]: " + logMsg);
		log.warning(id + ": " + logMsg);
    } // logWarning


    //------------------------------------------------------------
    // logSevere
    private final void logSevere(String logMsg) {
		sensorEmulatorController.appendTextArea("[SEVERE]: " + logMsg);
		log.severe(id + ": " + logMsg);
    } // logSevere
} // SensorEmulator

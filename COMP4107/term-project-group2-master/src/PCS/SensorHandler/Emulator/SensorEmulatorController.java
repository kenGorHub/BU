package PCS.SensorHandler.Emulator;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.MBox;
import AppKickstarter.misc.Msg;
import java.util.logging.Logger;

import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;


//======================================================================
// SensorEmulatorController
public class SensorEmulatorController {
    private String id;
    private AppKickstarter appKickstarter;
    private Logger log;
    private SensorEmulator sensorEmulator;
    private MBox gateMBox;
    public TextArea gateTextArea;
    public Button autoOpenButton;
    public Button autoCloseButton;
    public Button autoPollButton;
    private int lineNo = 0;


    //------------------------------------------------------------
    // initialize
    public void initialize(String id, AppKickstarter appKickstarter, Logger log, SensorEmulator sensorEmulator) {
        this.id = id;
        this.appKickstarter = appKickstarter;
		this.log = log;
		this.sensorEmulator = sensorEmulator;
		this.gateMBox = appKickstarter.getThread("GateHandler").getMBox();
    } // initialize

    //------------------------------------------------------------
    // appendTextArea
    public void appendTextArea(String status) {
		Platform.runLater(() -> gateTextArea.appendText(String.format("[%04d] %s\n", ++lineNo, status)));
    } // appendTextArea
} // SensorEmulatorController

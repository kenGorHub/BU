package PCS.SensorHandler.Emulator;

import PCS.SensorHandler.Floor;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.MBox;
import AppKickstarter.misc.Msg;
import java.util.logging.Logger;

import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.control.Label;
import javafx.scene.control.ComboBox;


//======================================================================
// SensorEmulatorController
public class SensorEmulatorController {
    private String id;
    private AppKickstarter appKickstarter;
    private Logger log;
    private SensorEmulator sensorEmulator;
    private MBox sensorMBox;
	private int lineNo = 0;
	private Floor[] clonedfloor;
	private int n;
    public Label comboBoxLabel;
    public ComboBox comboBox;
	public TextArea sensorTextArea;
	public Button up;
	public Button down;
    
    //------------------------------------------------------------
    // initialize
    public void initialize(String id, AppKickstarter appKickstarter, Logger log, SensorEmulator sensorEmulator, int n, Floor[] clonedfloor) {
        this.id = id;
        this.appKickstarter = appKickstarter;
		this.log = log;
		this.sensorEmulator = sensorEmulator;
		this.clonedfloor = clonedfloor;
		this.sensorMBox = appKickstarter.getThread("SensorHandler").getMBox();
		this.n = n;
		//this items are for confirguring the ComboBox
		for(int i = 0; i <= n; i++){
			comboBox.getItems().add(i);
		}
		down.setVisible(false);
		up.setVisible(false);
    } // initialize

	public void buttonPressed(ActionEvent actionEvent) {
        Button btn = (Button) actionEvent.getSource();

        switch (btn.getText()) {
            case "Car is going up":
				appendTextArea("Send going up request");
                sensorMBox.send(new Msg(id, null, Msg.Type.GoingUp, "Car is going up"));
				comBoxWasUpdated();
                break;
            case "Car is going down":
				appendTextArea("Send going up request");
                sensorMBox.send(new Msg(id, null, Msg.Type.GoingDown, "Car is going down"));
				comBoxWasUpdated();
                break;

            default:
                log.warning(id + ": unknown button: [" + btn.getText() + "]");
                break;
        }
    }

    //------------------------------------------------------------
    // appendTextArea
    public void appendTextArea(String status) {
		Platform.runLater(() -> sensorTextArea.appendText(String.format("[%04d] %s\n", ++lineNo, status)));
    } // appendTextArea
	
	public void comBoxWasUpdated(){
		up.setVisible(true);
		down.setVisible(true);
		int floornumber = Integer.parseInt(comboBox.getValue().toString());
		int carNum = clonedfloor[floornumber].getCarNum();
		Platform.runLater(() -> sensorMBox.send(new Msg(id, null, Msg.Type.FloorNumber, comboBox.getValue().toString())));
		Platform.runLater(() -> comboBoxLabel.setText("Car Number: " + carNum));
		if(floornumber == 0){
			down.setVisible(false);
			appendTextArea("vehicle quantity of the upper floor:" + clonedfloor[floornumber+1].getCarNum());
			appendTextArea("vehicle quantity of the current floor:" + carNum);
		}
		else if(floornumber == n){
			up.setVisible(false);
			appendTextArea("vehicle quantity of the current floor:" + carNum);
			appendTextArea("vehicle quantity of the lower floor:" + clonedfloor[floornumber-1].getCarNum());
		}
		else{
			appendTextArea("vehicle quantity of the upper floor:" + clonedfloor[floornumber+1].getCarNum());
			appendTextArea("vehicle quantity of the current floor:" + carNum);
			appendTextArea("vehicle quantity of the lower floor:" + clonedfloor[floornumber-1].getCarNum());
		}
		if(carNum <= 0){
			up.setVisible(false);
			down.setVisible(false);
		}
		
	}
} // SensorEmulatorController

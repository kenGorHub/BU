package PCS.PayMachineHandler.Emulator;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.MBox;
import AppKickstarter.misc.Msg;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.Logger;

import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;


//======================================================================
// PayMachineEmulatorController
public class PayMachineEmulatorController {
    private String id;
    private AppKickstarter appKickstarter;
    private Logger log;
    private PayMachineEmulator paymachineEmulator;
    private MBox paymachineMBox;
    public TextArea gateTextArea;
    public Button auto_Read_CNR_Button;
	public Button auto_Read_AP_Button;
	public Button auto_Read_UP_Button;
    public Button autoCloseButton;
    public Button autoPollButton;
    private int lineNo = 0;
	private DateFormat datetimeFormat;
	private String Enter;


    //------------------------------------------------------------
    // initialize
    public void initialize(String id, AppKickstarter appKickstarter, Logger log, PayMachineEmulator paymachineEmulator) {
        this.id = id;
        this.appKickstarter = appKickstarter;
		this.log = log;
		this.paymachineEmulator = paymachineEmulator;
		this.paymachineMBox = appKickstarter.getThread("PayMachineHandler").getMBox();
		datetimeFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
	} // initialize


    //------------------------------------------------------------
    // buttonPressed
    public void buttonPressed(ActionEvent actionEvent) {
	Button btn = (Button) actionEvent.getSource();

	switch (btn.getText()) {
	    case "Reading Ticket Request can_not_read":
		Enter = datetimeFormat.format(new Date());
		paymachineMBox.send(new Msg(id, null, Msg.Type.ReadingTicketRequest, "can_not_read "+Enter));
		break;

		case "Reading Ticket Request already_paid":
		Enter = datetimeFormat.format(new Date());
		paymachineMBox.send(new Msg(id, null, Msg.Type.ReadingTicketRequest, "already_paid "+Enter));
		break;

		case "Reading Ticket Request unpaid":
		Enter = datetimeFormat.format(new Date());
		paymachineMBox.send(new Msg(id, null, Msg.Type.ReadingTicketRequest, "unpaid "+Enter));
		break;

	    case "Paying Fee Request enought balance":
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayingFeeRequest, "9999"));
		break;

		case "Paying Fee Request not enought balance":
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayingFeeRequest, "-50"));
		break;

	    case "Return Ticket Reply":
		paymachineMBox.send(new Msg(id, null, Msg.Type.ReturnTicketReply, ""));
		break;

	    case "Auto Reading Ticket Request can_not_read: On":
		Platform.runLater(() -> auto_Read_CNR_Button.setText("Auto Read_CNR: Off"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoRead_CNRToggle, "ToggleAutoRead_CNR"));
		break;

	    case "Auto Reading Ticket Request can_not_read: Off":
		Platform.runLater(() -> auto_Read_CNR_Button.setText("Auto Read_CNR: On"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoRead_CNRToggle, "ToggleAutoRead_CNR"));
		break;

	    case "Auto Reading Ticket Request already_paid: On":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_AP: Off"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoRead_APToggle, "ToggleAutoRead_AP"));
		break;

	    case "Auto Reading Ticket Request already_paid: Off":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_AP: On"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoRead_APToggle, "ToggleAutoRead_AP"));
		break;

		case "Auto Reading Ticket Request unpaid: On":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_UP: Off"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoRead_UPToggle, "ToggleAutoRead_UP"));
		break;

		case "Auto Reading Ticket Request unpaid: Off":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_UP: On"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoRead_UPToggle, "ToggleAutoRead_UP"));
		break;

		case "Auto Paying Fee Request enought balance: On":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_UP: Off"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoPay_SToggle, "ToggleAutoPay_S"));
		break;

		case "Auto Paying Fee Request enought balance: Off":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_UP: On"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoPay_SToggle, "ToggleAutoPay_S"));
		break;

		case "Auto Reading Ticket Request unpaid: On":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_UP: Off"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoPay_FToggle, "ToggleAutoPay_F"));
		break;

		case "Auto Reading Ticket Request unpaid: Off":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_UP: On"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoPay_FToggle, "ToggleAutoPay_F"));
		break;

		case "Auto Return Ticket: On":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_UP: Off"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoReturnToggle, "ToggleAutoReturn"));
		break;

		case "Auto Return Ticket: Off":
		Platform.runLater(() -> auto_Read_AP_Button.setText("Auto Read_UP: On"));
		paymachineMBox.send(new Msg(id, null, Msg.Type.PayMachineEmulatorAutoReturnToggle, "ToggleAutoReturn"));
		break;

	    default:
	        log.warning(id + ": unknown button: [" + btn.getText() + "]");
		break;
	}
    } // buttonPressed


    //------------------------------------------------------------
    // appendTextArea
    public void appendTextArea(String status) {
	Platform.runLater(() -> gateTextArea.appendText(String.format("[%04d] %s\n", ++lineNo, status)));
    } // appendTextArea
} // GateEmulatorController

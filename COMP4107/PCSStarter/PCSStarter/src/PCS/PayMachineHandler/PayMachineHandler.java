package PCS.PayMachineHandler;

import AppKickstarter.AppKickstarter;
import AppKickstarter.misc.*;


//======================================================================
// PayMachineHandler
public class PayMachineHandler extends AppThread {
    protected final MBox pcsCore;
    private Double ParkingFee;
    private PayMachineStatus paymachineStatus;

    //------------------------------------------------------------
    // PayMachineHandler
    public PayMachineHandler(String id, AppKickstarter appKickstarter) {
		super(id, appKickstarter);
		pcsCore = appKickstarter.getThread("PCSCore").getMBox();
		paymachineStatus = PayMachineStatus.PM_idle;
    } // PayMachineHandler


    //------------------------------------------------------------
    // run
    public void run() {
		Thread.currentThread().setName(id);
		log.info(id + ": starting...");

		for (true) {
			Msg msg = mbox.receive();

			log.fine(id + ": message received: [" + msg + "].");

			processMsg(msg);
		}

		// declaring our departure
		appKickstarter.unregThread(this);
		log.info(id + ": terminating...");
    } // run


    //------------------------------------------------------------
    // processMsg
    protected void processMsg(Msg msg) {

		switch (msg.getType()) {
			case ReadingTicketRequest:  	handleReadingTicketRequest(msg.toString());  break;
			case DisplayFeeReply: 			handleDisplayFeeReply(Double.valueOf(msg.toString())); break;
			case PayingFeeRequest:	   		handlePayingFeeRequest(Double.valueOf(msg.toString())); break;
			case PrintingTicketReply:	   	handlePrintingTicketReply(msg.toString()); break;
			case ReturnTicketReply:	   		handleReturnTicketReply(); break;
			default:
			log.warning(id + ": unknown message type: [" + msg + "]");
		}
    } // processMsg


    //------------------------------------------------------------
    // handleReadingTicketRequest
    protected final void handleReadingTicketRequest(String Ticket) {
		log.info(id + ": Reading Ticket");

		String[] Ticket_inf = string.split(" ");
		String TicketStatus = Ticket_inf[0];
		String Enter_Time = Ticket_inf[1];

        switch (TicketStatus) {
			case can_not_read:
			log.warning(id + ": can not read ticket ,please try again !!  Ignore request.");
			break;

			case already_paid:
			log.warning(id + ": ticket is already paid!!  Ignore request.");
			break;

			case unpaid:
			log.info(id + ": ticket is unpaid.  Send information to PCS.");
			pcsCore.send(new Msg(id, mbox, Msg.Type.GetParkingFeeReply, Enter_Time));
			paymachineStatus = PayMachineStatus.Waiting_PCS;
			break;
		}
    } // handleReadingTicketRequest


    //------------------------------------------------------------
    // handleDisplayFeeReply
    protected final void handleDisplayFeeReply(Double PF) {
		log.info(id + ": Parking fee received");
		ParkingFee = PF;
		log.info("The parking fee is $"+ParkingFee);
    } // handleDisplayFeeReply


    //------------------------------------------------------------
    // handlePayingFeeRequest()
    protected final void handlePayingFeeRequest(double balance) {
		log.info(id + ": octopus card scanned");

		if(balance<ParkingFee){
			log.warning("remain value not enought!! Ignore request.");
		}
		else{
			log.info(id + ": payment success");
			log.info(id + "Printing_ticket");
			pcsCore.send(new Msg(id, mbox, Msg.Type.handlePrintingTicketReply, ""));
		}
	}

    } // handlePayingFeeRequest()


    //------------------------------------------------------------
    // handlePrintingTicketReply
    protected final void handlePrintingTicketReply(String LeaveDateTime) {
		log.info(id + ": leave information received");

		log.info(id + "leave datetime:"+LeaveDateTime);
		log.info(id + "parking fee:"+ParkingFee);
    } // handlePrintingTicketReply


    //------------------------------------------------------------
    // handleReturnTicketReply
    protected final void handleReturnTicketReply() {
		log.info(id + ": ticket removed");
		paymachineStatus = PayMachineStatus.PM_idle;
    } // handleReturnTicketReply

    //------------------------------------------------------------
    // Pay Machine Status
    private enum PayMachineStatus {
		PM_idle,
		reading_ticket,
		Waiting_PCS,
		Display_fee,
		Paying_fee,
		Pay_fail,
		Pay_success,
		Printing_ticket,
		Return_ticket
    }
} // PayMachineHandler

package PCS.SensorHandler;

public class TopFloor extends Floor{
	protected int carNum;
	
	public TopFloor(){type = "TopFloor";}

	public void movement(String msg){
		carNum--;
		int lowerFloor = getFloorNum()-1;
		floorMap.get(lowerFloor).increaseCar();
	}
	public int getCarNum(){return carNum;}
	public void increaseCar(){carNum++;}
}
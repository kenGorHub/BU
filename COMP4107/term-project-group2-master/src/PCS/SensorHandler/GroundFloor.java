package PCS.SensorHandler;

public class GroundFloor extends Floor{
	protected int carNum = 5;
	
	public GroundFloor(){type = "GroundFloor";}
	public void movement(String msg){
		carNum--;
		int lowerFloor = getFloorNum()+1;
		floorMap.get(lowerFloor).increaseCar();
	}
	public void increaseCar(){carNum++;}
	public int getCarNum(){return carNum;}
}
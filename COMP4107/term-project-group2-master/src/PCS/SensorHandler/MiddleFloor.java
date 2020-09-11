package PCS.SensorHandler;

public class MiddleFloor extends Floor{
	protected int carNum;
	
	public MiddleFloor(){type = "MiddleFloor";}
	public void movement(String msg){
		carNum--;
		if(msg == "Car is going up"){
			int upperFloor = getFloorNum()+1;
			floorMap.get(upperFloor).increaseCar();
		}else if(msg == "Car is going down"){
			int lowerFloor = getFloorNum()-1;
			floorMap.get(lowerFloor).increaseCar();
		}
	}
	public void increaseCar(){carNum++;}
	public int getCarNum(){return carNum;}
}





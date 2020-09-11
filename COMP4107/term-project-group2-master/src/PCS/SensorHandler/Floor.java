package PCS.SensorHandler;

import java.util.Hashtable;

public abstract class Floor implements Cloneable{
	private int floorNum;
	protected Hashtable<Integer,Floor> floorMap;
	protected String type;
	protected int carNum;
	
	public int getFloorNum(){return floorNum;}
	public void setFloorNum(int floorNum){this.floorNum = floorNum;}
	public void setFloorMap(Hashtable<Integer,Floor> floorMap){this.floorMap = floorMap;}
	public String getType(){return type;}
	public abstract int getCarNum();
	public abstract void movement(String msg);
	public abstract void increaseCar();
	
}
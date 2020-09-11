package PCS.SensorHandler;

import java.util.Hashtable;

class FloorFactory{
	private static Hashtable<Integer,Floor> floorMap = new Hashtable<Integer,Floor>();
	
	public static Floor makeFloor(int floorNum){
		return (Floor) floorMap.get(floorNum);
	}
	
	public static void floorStructure(int n){
		
		GroundFloor gf = new GroundFloor();
		gf.setFloorNum(0);
		gf.setFloorMap(floorMap);
		floorMap.put(gf.getFloorNum(),gf);
		
		for(int i = 1; i < n ; i++){
			MiddleFloor mf = new MiddleFloor();
			mf.setFloorNum(i);
			mf.setFloorMap(floorMap);
			floorMap.put(mf.getFloorNum(),mf);
		}
		
		
		TopFloor tf = new TopFloor();
		tf.setFloorNum(n);
		tf.setFloorMap(floorMap);
		floorMap.put(tf.getFloorNum(),tf);
	}
}
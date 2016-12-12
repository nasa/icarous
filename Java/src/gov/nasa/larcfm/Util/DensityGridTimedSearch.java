/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.List;

public interface DensityGridTimedSearch {
	/**
	 * Perform a search on the given timed density grid, assuming a constant groundspeed.  Polygons may be static or moving.
	 * @param dg Timed density grid with polygon and initial weight information.  Polygons may be moving or static.
	 * @param startPos Start position for search.
	 * @param endPos Ending Position for search.
	 * @param startTime Absolute time to start the search at startPos
	 * @param gs ground speed of aircraft
	 * @return List of grid coordinates for a successful path, or null if no path found.
	 */
	public List<Pair<Integer,Integer>> search(DensityGridTimed dg, Position startPos, Position endPos, double startTime, double gs);

	public List<Pair<Integer,Integer>> optimalPath(DensityGridTimed dg);
	
//	/**
//	 * Perform a search on the given timed density grid assuming a constant groundspeed.  Polygons may be static or moving.  
//	 * This search includes a user-specified time buffer around the given path, meaning the path will be will be clear not only 
//	 * for the given ownship trajectory, but also for a user-specified time before and after (along the same trajectory).
//	 * @param dg Timed density grid with polygon and initial weight information.  Polygons may be moving or static.
//	 * @param startPos Start position for search.
//	 * @param endPos Ending Position for search.
//	 * @param startTime Absolute time to start the search at startPos
//	 * @param gs ground speed of aircraft
//	 * @param timeBefore positive, relative time before the ownship's path that must remain clear (absolute times before zero are ignored)
//	 * @param timeAfter positive, relative time after the ownship's path that must remain clear
//	 * @param interval polling interval for checks with the time range (suggest 1-5 mins for normal weather sizes and speeds)
//	 * @return List of grid coordinates for a successful path, or null if no path found.
//	 */
//	public List<Pair<Integer, Integer>> search(DensityGridTimed dg, Position startPos, Position endPos, double startTime, double gs, double timeBefore, double timeAfter, double interval);

}

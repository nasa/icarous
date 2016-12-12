/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;

public interface DensityGridSearch {
	/**
	 * Perform a search through the given DensitGrid, starting at startPos and ending at endPos.
	 * This assumes the grid has a set of static polygons
	 * @param dg density grid with polygon and initial weight information.  Polygons must be static.
	 * @param startPos Start position for search.
	 * @param endPos Ending Position for search.
	 * @return
	 */
	public List<Pair<Integer,Integer>> search(DensityGrid dg, Position startPos, Position endPos);
	
	public List<Pair<Integer,Integer>> optimalPath(DensityGrid dg);
}

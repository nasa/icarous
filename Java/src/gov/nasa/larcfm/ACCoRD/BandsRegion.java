/* 
 * Classification of Regions
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

/**
 * This enumeration provides constants to indicate the type of a
 * region of conflict prevention information.
 */
public enum BandsRegion { 
	/* UNKNOWN : Invalid band
	 * NONE: No band 
	 * RECOVERY: Band for violation recovery
	 * NEAR: Near conflict band 
	 * MID: Mid conflict bands 
	 * FAR: Far conflict band
	 */

	UNKNOWN("UNKNOWN"), NONE("NONE"), RECOVERY("RECOVERY"), NEAR("NEAR"), MID("MID"), FAR("FAR");

	private String name;

	BandsRegion(String nm) {
		name = nm;
	}

	public String toString() {
		return name;
	}

	boolean isValidBand() {
		return this != UNKNOWN;
	}

	boolean isResolutionBand() {
		return this == NONE || this == RECOVERY;
	}

	boolean isConflictBand() {
		return isValidBand() && !isResolutionBand();  
	}

	// RECOVERY=NONE < FAR < MID < NEAR
	int order() {
		if (isResolutionBand()) {
			return 0;
		}
		if (this == FAR) {
			return 1;
		}
		if (this == MID) {
			return 2;
		}
		if (this == NEAR) {
			return 3;
		}
		return -1;
	}

}

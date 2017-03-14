/*
 * Implementation of M of N algorithm for alerting
 * Contact: Cesar A. Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import java.util.LinkedList;
import java.util.Queue;

public class AlertingMofN {

	private int m_;
	private int n_;
	private int max_;
	private Queue<Integer> queue_;

	/* 
	 * Creates an empty M of N object
	 */
	public AlertingMofN() {
		m_ = 1;
		n_ = 0;
		max_ = 0;
		queue_ = new LinkedList<Integer>();
	}
	
	/*
	 * Creates a M of N object for given values of m, n, and maximum alert level. 
	 * Assumes  m >= 1 and m <= n
	 */
	public AlertingMofN(int m, int n, int max_alert_level) {
		m_ = m;
		n_ = n;
		max_ = max_alert_level;
		queue_ = new LinkedList<Integer>();
		for (int i=0;i<n_;++i) {
			queue_.add(0);
		}
	}

	/*
	 * Set new values for m and n.
	 * Assumes  m >= 1 and m <= n
	 */
	public void setMofN(int m, int n) {
		m_ = m;
		n_ = n;
		reset();
	}

	/*
	 * Set new values for maximum alert level
	 * Assumes max_alert_level >= 1
	 */
	public void setMaxAlertLevel(int max_alert_level) {
		max_ = max_alert_level;
		reset();
	}

	/*
	 * Reset M of N object.
	 */
	public void reset() {
		queue_.clear();
		for (int i=0;i<n_;++i) {
			queue_.add(0);
		}
	}

	/* 
	 * Return M of N value for a given alert level
	 */
	public int m_of_n(int alert_level) {
		if (queue_.isEmpty() || alert_level < 0 || alert_level > max_ || m_ <= 1 || max_ <= 0) {
			return alert_level;
		} 
		queue_.poll();
		queue_.add(alert_level);
		int[] count = new int[max_];
		for (int i=0; i < max_; ++i) {
			count[i]=0;
		}
		for (int alert : queue_) {
			for (int i=alert-1;i >= 0;--i) {
				count[i]++;
			}
		}	
		for (int i=max_-1; i >= 0; --i) {
			if (count[i] >= m_) {
				return i+1;
			}
		}
		return 0;
	}

}

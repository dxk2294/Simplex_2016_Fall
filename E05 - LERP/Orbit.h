/*----------------------------------------------
Programmer: D. James Kelly (dxk2294@rit.edu)
Date: 2017/10

Orbit contains data necessary to linearly interpolate
between various points along the edge of an orbit
of a given radius and resolution
----------------------------------------------*/
#ifndef __ORBIT_H_
#define __ORBIT_H_

struct Orbit {
	int m_numSides;
	int m_radius;
	int curIndex;
	int nextIndex;

	// Precondition: numSides > 2
	Orbit(int numSides, float radius) {
		m_numSides = numSides;
		m_radius = radius;
		curIndex = 0;
		nextIndex = 1;
	}

	void incrementIndex() {
		curIndex = (curIndex + 1) % m_numSides;
		nextIndex += (nextIndex + 1) % m_numSides;
	}
};

#endif //__ORBIT_H_


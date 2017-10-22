/*----------------------------------------------
Programmer: D. James Kelly (dxk2294@rit.edu)
Date: 2017/10
----------------------------------------------*/
#ifndef __ORBIT_H_
#define __ORBIT_H_

/*----------------------------------------------
Orbit contains data necessary to linearly interpolate
between various points along the edge of an orbit
of a given radius and resolution
----------------------------------------------*/
struct Orbit {
	int m_numSides; // represents the "resolution" of the orbit
	float m_radius; // radius of the orbit
	int curIndex; // the current position we are lerping between
	int nextIndex; // the next position we are lerping between

	// Precondition: numSides > 2
	// Constructor
	Orbit(int numSides, float radius) {
		m_numSides = numSides;
		m_radius = radius;
		curIndex = 0;
		nextIndex = 1;
	}

	// increments cur and next index within the bounds of the orbit size
	void incrementIndex() {
		curIndex = (curIndex + 1) % m_numSides;
		nextIndex = (nextIndex + 1) % m_numSides;
	}

	// returns the current position of the orbit
	vector3 curVector() {
		return vectorAt(curIndex);
	}

	// returns the next position of the orbit
	vector3 nextVector() {
		return vectorAt(nextIndex);
	}

	// returns a vector along the circle at a given index
	vector3 vectorAt(int index) {
		// error checking, this should never happen
		if (index < 0 || index >= m_numSides) {
			return ZERO_V3;
		}

		// calculate angle around the orbit given index and num sides
		float angle = (float)index / (float)m_numSides * PI * 2.0f;
		return vector3(m_radius * cos(angle), m_radius * sin(angle), 0.0f);
	}
};

#endif //__ORBIT_H_


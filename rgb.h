#ifndef RGB_H
#define RGB_H

// an example -- you would need to add more functions

struct RGB {
	int r, g, b;
	RGB() :r(255), g(255), b(255) {}
	RGB(int R, int G, int B) : r(R), g(G), b(B) {}
	RGB& operator=(RGB val) {
		r = val.r;
		g = val.g;
		b = val.b;
		return *this;
	}
};

#endif
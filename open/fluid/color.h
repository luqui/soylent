#ifndef __COLOR_H__
#define __COLOR_H__

class Color {
public:
	float R, G, B;

	Color(float red, float green, float blue): R(red), G(green), B(blue) { }

	void set_color() const {
		glColor3f(R, G, B);
	}

	void set_color_a(float alpha) const {
		glColor4f(R, G, B, alpha);
	}
};

#endif

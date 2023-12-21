#pragma once

/*
*  drawings.h
* 
*  Useful tools for drawing things, including color mapping, shapes, etc.
*  Needs to be improved as more features are added
*/

namespace draw {

	void draw_paramecium(double x, double y, double r, double size, double back = 0) {
		fl_color(fl_rgb_color(0x33 + 0x99 * back, 0xcc - 0x99 * back, 0x66));
		fl_begin_polygon();
		double dx1 = cos(r + 0.5) * size, dx2 = cos(r - 0.5) * size, dy1 = sin(r + 0.5) * size, dy2 = sin(r - 0.5) * size;
		fl_vertex(x + dx1, y + dy1);
		fl_vertex(x + dx2, y + dy2);
		fl_vertex(x - dx1, y - dy1);
		fl_vertex(x - dx2, y - dy2);
		fl_end_polygon();
	}

	Fl_Color path_linear_gradient(double s) {
		return fl_rgb_color(255 * s, 255 * s, 255);
	}

	Fl_Color rainbow_linear_gradient(double s) {
		if (s < 0.25) {
			return fl_rgb_color(255, 510 * s + 128, 128);
		} else if (s < 0.5) {
			return fl_rgb_color(510 * (0.5 - s) + 128, 255, 128);
		} else if (s > 0.75) {
			return fl_rgb_color(128, 510 * (1 - s) + 128, 255);
		} else {
			return fl_rgb_color(128, 255, 510 * (s - 0.5) + 128);
		}
	}

	Fl_Color rainbow_linear_gradient_base(double s) {
		if (s < 0.25) {
			return fl_rgb_color(255, 1020 * s, 0);
		} else if (s < 0.5) {
			return fl_rgb_color(1020 * (0.5 - s), 255, 0);
		} else if (s > 0.75) {
			return fl_rgb_color(0, 1020 * (1 - s), 255);
		} else {
			return fl_rgb_color(0, 255, 1020 * (s - 0.5));
		}
	}
} // namespace draw
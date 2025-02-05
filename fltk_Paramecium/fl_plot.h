#pragma once

/*
 *  fl_plot.h
 *
 *  Drawing 2D plot of adaptation score and other simulation results
 */

#include "plot.h"

namespace paramecium {

	class Fl_Plot : public Fl_Widget, public Plot {
	public:
		Fl_Offscreen oscr_plot;
		Fl_Image* bg_white_transparent, * bg_white_indicator;
		Fl_RGB_Image* plot_image = nullptr;

		Fl_Button* confirm_plot_bt;
		Fl_Hor_Value_Slider* samples_ip, * total_energy_ip, * step_len_ip, * rotate_rad_ip, * deviation_m_ip, * deviation_v_ip;

		Fl_Plot(int x_, int y_, int w_, int h_, int g_size) : Fl_Widget(x_, y_, w_, h_), Plot(w_, h_, g_size) {
			oscr_plot = fl_create_offscreen(w(), h());
			uchar data[4]{ 255,255,255,80 };
			Fl_RGB_Image img(data, 1, 1, 4);
			bg_white_transparent = img.copy(w_, h_);
			data[3] = 200;
			Fl_RGB_Image img2(data, 1, 1, 4);
			bg_white_indicator = img2.copy(7 * g_size, 6 * g_size);
		}

		void draw() {
			if (!finished) {
				bool g_fin = simulate_grid();
				if (g_fin && curr_x == grid_w - 1) {
					if (curr_y == grid_h - 1) {
						finished = true;
						curr_x = curr_y = -1;
						gen_image();
					} else {
						curr_x = 0, curr_y++;
					}
				} else if (g_fin) {
					curr_x++;
				}
			}
			if (!finished) {
				draw_grid();
			} else if (redraw_flag) {
				// Drectly use pre-generated image
				fl_begin_offscreen(oscr_plot);
				plot_image->draw(0, 0);
				display_detail();
				fl_end_offscreen();
				fl_copy_offscreen(x(), y(), w(), h(), oscr_plot, 0, 0);
				redraw_flag = false;
			}
		}

		void display_detail() {
			if (curr_x < 0 || curr_y < 0) {
				return;
			}
			auto& d = data[curr_x][curr_y];
			double score = d.score_sum / d.total, rate = (double)d.arrived / d.total;
#ifdef _DEBUG
			std::cout << "Pointing at (" << curr_x << ", " << curr_y << "), score = " << score << ", rate = " << rate << std::endl;
#endif // _DEBUG
			auto grid_color = draw::rainbow_linear_gradient_base((sum_max - d.score_sum) / std::max(sum_max - sum_min, 1.0));
			fl_rectf(curr_x * pixels_per_grid, curr_y * pixels_per_grid, pixels_per_grid, pixels_per_grid, grid_color);
			bool align = curr_x <= 6;
			double gx = (align ? curr_x + 1.6 : curr_x - 6.6), px = (align ? curr_x + 1 : curr_x - 7), gy = (curr_y > grid_h - 6 ? curr_y - 5 : curr_y);
			auto tag_color = fl_darker(grid_color);
			bg_white_transparent->draw(0, 0);
			bg_white_indicator->draw(px * pixels_per_grid, gy * pixels_per_grid);
			display_tag_num(get_val_name(val1), d.val1, gx, gy + 0.6, 3 * pixels_per_grid, pixels_per_grid, tag_color, align);
			display_tag_num(get_val_name(val2), d.val2, gx + 3, gy + 0.6, 3 * pixels_per_grid, pixels_per_grid, tag_color, align);
			display_tag_num("Adaptation Score", score, gx, gy + 1.85, 6 * pixels_per_grid, 1.8 * pixels_per_grid, grid_color, align);
			display_tag_num("Arrival Rate", rate, gx, gy + 3.8, 6 * pixels_per_grid, 1.8 * pixels_per_grid, grid_color, align);
		}
		void display_tag_num(std::string tag, double num, double gx, double gy, int w, int h, Fl_Color tag_color, bool align) {
			fl_push_clip(gx * pixels_per_grid, gy * pixels_per_grid, w, h);
			fl_color(tag_color);
			fl_font(FL_BOLD, h * 0.4);
			fl_draw(tag.c_str(), gx * pixels_per_grid, gy * pixels_per_grid, w, h * 0.4, (align ? FL_ALIGN_LEFT : FL_ALIGN_RIGHT));
			fl_color(FL_BLACK);
			fl_font(FL_BOLD, h * 0.6);
			auto n = std::to_string(num);
			if (n.size() >= 10) {
				n = n.substr(0, 10);
			}
			fl_draw(n.c_str(), gx * pixels_per_grid, gy * pixels_per_grid + h * 0.4, w, h * 0.6, (align ? FL_ALIGN_LEFT : FL_ALIGN_RIGHT));
			fl_pop_clip();
		}

		void gen_image() {
			fl_begin_offscreen(oscr_plot);
			fl_rectf(0, 0, w(), h(), FL_WHITE);
			draw_grid(true);
			auto data = fl_read_image(nullptr, 0, 0, w(), h());
			if (plot_image) {
				delete plot_image;
			}
			plot_image = new Fl_RGB_Image(data, w(), h());
			fl_end_offscreen();
			plot_image->draw(x(), y());
		}

		void draw_grid(bool in_offscreen = false) {
			for (int i = 0; i < grid_w; i++) {
				for (int j = 0; j < grid_h; j++) {
					if (data[i][j].total == 0) {
						fl_color(FL_WHITE);
					} else {
						double l = (sum_max - data[i][j].score_sum) / std::max(sum_max - sum_min, 1.0);
						fl_color(draw::rainbow_linear_gradient(l));
					}
					if (in_offscreen) {
						fl_rectf(i * pixels_per_grid, j * pixels_per_grid, pixels_per_grid, pixels_per_grid);
					} else {
						fl_rectf(x() + i * pixels_per_grid, y() + j * pixels_per_grid, pixels_per_grid, pixels_per_grid);
					}
				}
			}
		}

		int handle_show_detail(int event, double grid_x, double grid_y) {
			if (!finished) {
				return Fl_Widget::handle(event);
			}
			switch (event) {
			case FL_ENTER: {
				return 1;
			}
			case FL_MOVE: {
				if (std::floor(grid_x / 2) == curr_x && std::floor(grid_y / 2) == curr_y) {
					return 0;
				}
				curr_x = std::floor(grid_x / 2);
				curr_y = std::floor(grid_y / 2);
				return 1;
			}
			case FL_LEAVE: {
				curr_x = -1;
				curr_y = -1;
				return 1;
			}
			default:
				break;
			}
			return Fl_Widget::handle(event);
		}

		void update_sliders() {
			total_energy_ip->value(o_erg);
			step_len_ip->value(o_slen);
			rotate_rad_ip->value(o_rotr);
			deviation_m_ip->value(o_devm);
			deviation_v_ip->value(o_devv);
		}
		void update_args() {
			samples = samples_ip->value();
			args[0] = o_erg = total_energy_ip->value();
			args[1] = o_slen = step_len_ip->value();
			args[2] = o_rotr = rotate_rad_ip->value();
			args[3] = o_devm = deviation_m_ip->value();
			args[4] = o_devv = deviation_v_ip->value();
		}
	};

	static void confirm_plot_cb(Fl_Widget* o, void* v) {
		Fl_Plot* p = (Fl_Plot*)v;
		p->update_args();
		p->clear_status();
		p->sync_back();
	}
} // namespace paramecium
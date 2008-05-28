#ifndef __FLUIDGRID_H__
#define __FLUIDGRID_H__

#include "vec.h"
#include "util.h"
#include <cstdlib>
#include <cmath>

inline float clamp(float x, float lo, float hi) {
	if (x <= lo) return lo;
	if (x >= hi) return hi;
	return x;
}

const int DIFFUSE_RELAXATION = 1;
const int PROJECT_RELAXATION = 15;

class FluidUtils {
protected:
	typedef float** Scr;

	FluidUtils(int w, int h) : W(w), H(h) { }
	const int W, H;

	Scr make_scr() const {
		float* chunk = new float[W*H];
		float** addrs = new float*[W];
		for (int i = 0; i < W; i++) {
			addrs[i] = &chunk[i*H];
		}
		return addrs;
	}

	void delete_scr(Scr s) const {
		delete[] s[0];
		delete[] s;
	}

public:

	enum BoundComputation { NONE, UNORM, VNORM };

#   define INBOUND_LOOP(i,j) for (int i = 1; i < W-1; i++) for (int j = 1; j < H-1; j++)

	void clear(Scr x) {
		for (int i = 0; i < W; i++) {
			for (int j = 0; j < H; j++) {
				x[i][j] = 0;
			}
		}
	}

	void set_boundary(BoundComputation comp, Scr x) {
		//return;//XXX
		for (int i = 1; i < W-1; i++) {
		// Bounded		
			x[i][0]   = comp == VNORM ? -x[i][1]   : x[i][1];
			x[i][H-1] = comp == VNORM ? -x[i][H-2] : x[i][H-2];
		
		// Toroidal
		/*	float avg = (x[i][1] + x[i][H-2])/2;
			x[i][0]   = x[i][1]   = avg;
			x[i][H-1] = x[i][H-2] = avg; */
		}
		for (int j = 1; j < H-1; j++) {
		// Bounded		
			x[0][j]   = comp == UNORM ? -x[1][j]   : x[1][j];
			x[W-1][j] = comp == UNORM ? -x[W-2][j] : x[W-2][j];
		
		// Toroidal
		/*	float avg = (x[1][j] + x[W-2][j])/2;
			x[0][j]   = x[1][j]   = avg;
			x[W-1][j] = x[W-2][j] = avg; */
		}
		x[0][0]   = (x[1][0] + x[0][1]) * 0.5;
		x[W-1][0] = (x[W-2][0] + x[W-1][1]) * 0.5;
		x[0][H-1] = (x[1][H-1] + x[0][H-2]) * 0.5;
		x[W-1][H-1] = (x[W-2][H-1] + x[W-1][H-2]) * 0.5;
	}

	void diffuse(BoundComputation comp, Scr x, Scr x0, float diffusion) {
		float da = DT * diffusion * (W-2)*(H-2);

		if (diffusion == 0) {
			INBOUND_LOOP(i,j) {
				x[i][j] = x0[i][j];
			}
			return;  // Skip the expensive relaxation procedure
		}

		float mult = 1 / (1+4*da);
		for (int k = 0; k < DIFFUSE_RELAXATION; k++) {
			INBOUND_LOOP(i,j) {
				x[i][j] = (x0[i][j] + da * (x[i-1][j] + x[i+1][j]
										  + x[i][j-1] + x[i][j+1])) * mult;
			}
			set_boundary(comp, x);
		}
	}

	void advect(BoundComputation comp, Scr d, Scr d0, Scr u, Scr v) {
		INBOUND_LOOP(i,j) {
			float x = i - DT*W*u[i][j];
			float y = j - DT*H*v[i][j];
			int i0 = int(clamp(x, 0.5, W-1.5));
			int i1 = i0+1;
			int j0 = int(clamp(y, 0.5, H-1.5));
			int j1 = j0+1;
			float s1 = x - i0;  float s0 = 1 - s1;
			float t1 = y - j0;  float t0 = 1 - t1;
			d[i][j] = s0*(t0*d0[i0][j0] + t1*d0[i0][j1])
					+ s1*(t0*d0[i1][j0] + t1*d0[i1][j1]);
		}
		set_boundary(comp, d);
	}

	void project(Scr u, Scr v, Scr p, Scr div) {
		float hx = 1.0/W;
		float hy = 1.0/H;

		INBOUND_LOOP(i,j) {
			div[i][j] = -0.5 *( hx * (u[i+1][j] - u[i-1][j])
							  + hy * (v[i][j+1] - v[i][j-1]));
			p[i][j] = 0;
		}
		set_boundary(NONE, div);
		set_boundary(NONE, p);

		for (int k = 0; k < PROJECT_RELAXATION; k++) {
			INBOUND_LOOP(i,j) {
				p[i][j] = (div[i][j] + p[i-1][j] + p[i+1][j]
									 + p[i][j-1] + p[i][j+1]) * 0.25;
			}
			set_boundary(NONE, p);
		}

		INBOUND_LOOP(i,j) {
			u[i][j] -= 0.5 * (p[i+1][j] - p[i-1][j]) * W;
			v[i][j] -= 0.5 * (p[i][j+1] - p[i][j-1]) * H;
		}
		set_boundary(UNORM, u);
		set_boundary(VNORM, v);
	}

#   undef INBOUND_LOOP
};

class FluidGrid : private FluidUtils {
public:
	typedef FluidUtils::Scr Scr;
	
	FluidGrid(int w, int h, vec ll, vec ur, float viscosity)
		: FluidUtils(w,h), ll_(ll), ur_(ur), viscosity_(viscosity),
		  U_(make_scr()), V_(make_scr()), U_back_(make_scr()), V_back_(make_scr())
	{
		clear(U_);
		clear(V_);
		clear(U_back_);
		clear(V_back_);
	}

	~FluidGrid() {
		delete_scr(U_);
		delete_scr(V_);
		delete_scr(U_back_);
		delete_scr(V_back_);
	}

	bool in_range(vec v) const {
		return ll_.x <= v.x && v.x < ur_.x
			&& ll_.y <= v.y && v.y < ur_.y;
	}
	
	void add_velocity(vec x, vec v) {
		const int xx = ix_x(x.x), yy = ix_y(x.y);
		U_[xx][yy] += DT * v.x;
		V_[xx][yy] += DT * v.y;
	}

	vec get_velocity(vec x) const {
		const int xx = ix_x(x.x), yy = ix_y(x.y);
		return vec(U_[xx][yy], V_[xx][yy]);
	}

	void step_velocity() {
		internal_step(U_, V_, U_back_, V_back_);
	}

	vec get_velocity_direct(int x, int y) const {
		return vec(U_[x][y],V_[x][y]);
	}

	void set_velocity_direct(int x, int y, vec amt) {
		U_[x][y] = amt.x;
		V_[x][y] = amt.y;
	}

protected:
	void internal_step(Scr u, Scr v, Scr u0, Scr v0) {
		diffuse(FluidUtils::UNORM, u0, u, viscosity_);
		diffuse(FluidUtils::VNORM, v0, v, viscosity_);
		project(u0, v0, u, v);
		advect(FluidUtils::UNORM, u, u0, u0, v0);
		advect(FluidUtils::VNORM, v, v0, u0, v0);
		project(u, v, u0, v0);
	}
	
	bool in_range_i(int x, int y) const {
		return 0 <= x && x < W && 0 <= y && y < H;
	}

	int ix_x(float x) const {
		int ret = int((x - ll_.x) / (ur_.x - ll_.x) * W);
		if (ret < 0) return 0;
		if (ret > W-1) return W-1;
		return ret;
	}

	int ix_y(float y) const {
		int ret = int((y - ll_.y) / (ur_.y - ll_.y) * H);
		if (ret < 0) return 0;
		if (ret > H-1) return H-1;
		return ret;
	}

	void erase() {
		clear(U_);
		clear(V_);
		clear(U_back_);
		clear(V_back_);
	}

	const vec ll_, ur_;
	float viscosity_;
	
	Scr U_, V_;  // x- and y-components of velocity
	Scr U_back_, V_back_;
};


class DensityGrid : private FluidUtils {
public:
	typedef FluidUtils::Scr Scr;

	DensityGrid(int w, int h, vec ll, vec ur, float diffusion) 
		: FluidUtils(w,h), ll_(ll), ur_(ur), diffusion_(diffusion),
		  total_density_(0), D_(make_scr()), D_back_(make_scr())
	{
		clear(D_);
		clear(D_back_);
	}

	~DensityGrid() {
		delete_scr(D_);
		delete_scr(D_back_);
	}
	
	bool in_range(vec v) const {
		return ll_.x <= v.x && v.x < ur_.x
			&& ll_.y <= v.y && v.y < ur_.y;
	}

	void add_density(vec x, float amt) {
		const int xx = ix_x(x.x), yy = ix_y(x.y);
		total_density_ += DT * amt;
		D_[xx][yy] += DT * amt;
	}

	void set_density(vec x, float amt) {
		const int xx = ix_x(x.x), yy = ix_y(x.y);
		total_density_ += amt - D_[xx][yy];
		D_[xx][yy] = amt;
	}

	float get_density(vec x) const {
		const int xx = ix_x(x.x), yy = ix_y(x.y);
		return D_[xx][yy];
	}

	void step_density(Scr u, Scr v) {
		diffuse(FluidUtils::NONE, D_back_, D_, diffusion_);
		advect(FluidUtils::NONE, D_, D_back_, u, v);
		
		float postotal = 0;
		float negtotal = 0;
		for (int i = 0; i < W; i++) {
			for (int j = 0; j < H; j++) {
				if (D_[i][j] > 0) postotal += D_[i][j];
				else              negtotal += D_[i][j];
			}
		}
		float posfact = (total_density_ + sqrt(total_density_*total_density_ - 4*postotal*negtotal)) / (2*postotal);
		float negfact = 1/posfact;
		if (is_a_number(negfact) && is_a_number(posfact)) {
			for (int i = 1; i < W-1; i++) {
				for (int j = 1; j < H-1; j++) {
					if (D_[i][j] > 0) {
						D_[i][j] *= posfact;
					}
					else {
						D_[i][j] *= negfact;
					}
				}
			}
		}
	}

	float get_density_direct(int x, int y) const {
		return D_[x][y];
	}

	void set_density_direct(int x, int y, float amt) {
		total_density_ += amt - D_[x][y];
		D_[x][y] = amt;
	}

	float get_balance() const {
		return total_density_;
	}

	void alter_balance(float amt) {
		total_density_ += amt;
	}

	void erase()
	{
		clear(D_);
		clear(D_back_);
	}
	
protected:
	int ix_x(float x) const {
		int ret = int((x - ll_.x) / (ur_.x - ll_.x) * W);
		if (ret < 0) return 0;
		if (ret > W-1) return W-1;
		return ret;
	}

	int ix_y(float y) const {
		int ret = int((y - ll_.y) / (ur_.y - ll_.y) * H);
		if (ret < 0) return 0;
		if (ret > H-1) return H-1;
		return ret;
	}

	const vec ll_, ur_;
	float diffusion_;
	float total_density_;

	Scr D_;
	Scr D_back_;
};

class FluidDensityGrid : public FluidGrid, public DensityGrid {
public:
	typedef FluidGrid::Scr Scr;
	
	FluidDensityGrid(int w, int h, vec ll, vec ur, float diffusion, float viscosity)
		: FluidGrid(w, h, ll, ur, viscosity), DensityGrid(w, h, ll, ur, diffusion)
	{ }

	void step_density() {
		DensityGrid::step_density(FluidGrid::U_, FluidGrid::V_);
	}

	void step_velocity() {
		FluidGrid::step_velocity();
	}

	bool in_range(const vec& v) {
		return FluidGrid::in_range(v);
	}

	void erase() {
		FluidGrid::erase();
		DensityGrid::erase();
	}
};

#endif

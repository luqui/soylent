#include <soy/init.h>
#include <soy/util.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <ctime>
#include <Cassert>
#include <soy/Timer.h>

const int NUM_SIDES = 6;
SoyInit INIT;

const int NUM_PERMS = 1 << NUM_SIDES;
const float CENTER_DIST = float(1.0 + sqrt(3.0) / 2.0);

const int SIZE_X = 60, SIZE_Y = 60;

struct Permutation {
	Permutation() : index(-1), rotate(0) { }
	Permutation(int index, int rotate) : index(index), rotate(rotate) { }

	int index;
	int rotate;
};

Permutation PERM_MAP[NUM_PERMS];
int GENOME_SIZE = -1;

const int SIDES[NUM_SIDES][2] = {
	{  0, -1 },
	{  1, -1 },
	{  1,  0 },
	{  0,  1 },
	{ -1,  1 },
	{ -1,  0 },
};

int rotate_sides(int in, int amt) {
	return ((in << amt) | (in >> (NUM_SIDES - amt))) & ((1 << NUM_SIDES) - 1);
}

void draw_hex(float x, float y)
{
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (int i = 0; i <= 6; i++) {
		float theta = float(i * 2.0 * M_PI / 6.0);
		glVertex2f(x-sin(theta), y+cos(theta));
	}
	glEnd();
}

struct Color {
	Color() : r(0), g(0), b(0) { }
	Color(float r, float g, float b) : r(r), g(g), b(b) { }
	float r,g,b;

	void set() const {
		glColor3f(r,g,b);
	}
};

class Cell {
public:
	Cell() : value(false), spawned_this_frame(false) {
		genome_.resize(GENOME_SIZE);
	}

	bool value;
	Color color;
	bool spawned_this_frame;

	int rule(int neighbor_map) const {
		const Permutation& p = PERM_MAP[neighbor_map];
		return rotate_sides(genome_[p.index], p.rotate);
	}

	void randomize_genome() {
		/*
		for (int i = 0; i < int(genome_.size()); i++) {
			if (rand() % 3 == 0) {
				genome_[i] |= 1 << (rand() % NUM_SIDES);
				genome_[i] |= 1 << (rand() % NUM_SIDES);
			}
		}
		*/
		value = rand() % 8 == 0;
		color.r = randrange(0,1);
		color.g = randrange(0,1);
		color.b = randrange(0,1);
	}

	void mutate() {
		int bit = rand() % NUM_SIDES;
		int idx = rand() % genome_.size();
		genome_[idx] ^= 1 << bit;

		color.r += randrange(-0.1,0.1);
		color.g += randrange(-0.1,0.1);
		color.b += randrange(-0.1,0.1);
	}

private:
	std::vector<int> genome_;
};


class Grid {
public:
	Grid(int width, int height)
		: width_(width), height_(height), backbuf_(0)
	{
		grid_.resize(width_);

		for (int i = 0; i < width; i++) {
			grid_[i].resize(height_);
		}
	}

	Grid& operator = (const Grid& o) {
		assert(height_ == o.height_);
		assert(width_ == o.width_);
		for (int i = 0; i < width_; i++) {
			for (int j = 0; j < height_; j++) {
				grid_[i][j] = o.grid_[i][j];
			}
		}
		return *this;
	}

	~Grid() {
		delete backbuf_;
	}

	void randomize() {
		for (int i = 0; i < width_; i++) {
			for (int j = 0; j < height_; j++) {
				grid_[i][j].randomize_genome();
			}
		}
	}

	void mutate() {
		int i = rand() % width_;
		int j = rand() % height_;
		grid_[i][j].mutate();
	}

	bool in_range(int x, int y) const {
		return 0 <= x && x < width_ && 0 <= y && y < height_;
	}

	int neighbor_map(int x, int y) const {
		int result = 0;
		for (int i = 0; i < NUM_SIDES; i++) {
			int xx = x + SIDES[i][0];
			int yy = y + SIDES[i][1];

			if (in_range(xx,yy)) {
				result |= int(value_at(xx,yy).value) << i;
			}
		}

		return result;
	}

	void flip_neighbors(const Cell& src, int x, int y, int rule) {
		for (int i = 0; i < NUM_SIDES; i++) {
			int xx = x + SIDES[i][0];
			int yy = y + SIDES[i][1];

			if ((rule & (1 << i)) && in_range(xx,yy)) {
				Cell& c = backbuf_->value_at(xx,yy);
				Cell& o = value_at(xx,yy);
				if (c.value) {
					o.value = false;
				}
				else {
					if (o.spawned_this_frame) {
						o.value = false;
					}
					else {
						o = src;
						o.spawned_this_frame = true;
					}
				}
			}
		}
	}

	Cell& value_at(int x, int y) {
		return grid_[x][y];
	}

	const Cell& value_at(int x, int y) const {
		return grid_[x][y];
	}

	void adjudicate() {
		if (!backbuf_) { backbuf_ = new Grid(width_, height_); }
		*backbuf_ = *this;
		Grid& back = *backbuf_;

		for (int i = 0; i < width_; i++) {
			for (int j = 0; j < height_; j++) {
				value_at(i,j).spawned_this_frame = false;
			}
		}

		for (int i = 1; i < width_ - 1; i++) {
			for (int j = 1; j < height_ - 1; j++) {
				const Cell& c = back.value_at(i,j);
				if (c.value) {
					flip_neighbors(c, i, j, c.rule(back.neighbor_map(i,j)));
				}
			}
		}
	}

	void draw() const {
		for (int i = 0; i < width_; i++) {
			for (int j = 0; j < height_; j++) {
				if (value_at(i,j).value) {
					value_at(i,j).color.set();
					draw_hex(float(2*i + j), float(j) * CENTER_DIST);
				}
			}
		}
	}

private:
	int width_, height_;
	std::vector< std::vector<Cell> > grid_;

	Grid* backbuf_;
};


void generate_mapping()
{
	int counter = 0;
	for (int i = 0; i < NUM_PERMS; i++) {
		int sig = i;
		bool incctr = false;
		for (int j = 0; j < NUM_SIDES; j++) {
			if (PERM_MAP[sig].index == -1) {
				PERM_MAP[sig] = Permutation(counter, j);
				incctr = true;
			}

			// cycle the permutation left
			sig = rotate_sides(sig, 1);
		}

		if (incctr) counter++;
	}
	
	GENOME_SIZE = counter;
}

void init() {
	srand(time(NULL));
	INIT.init();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 2*SIZE_X + SIZE_Y, CENTER_DIST * SIZE_Y, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void events() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
			exit(0);
		}
		//if (e.type == SDL_KEYDOWN) { return; }
	}
}

int main(int argc, char** argv) {
	init();
	generate_mapping();

	Grid grid(SIZE_X, SIZE_Y);
	grid.randomize();

	FrameRateLockTimer timer(1/10.0);

	while (true) {
		timer.lock();
		events();
		grid.adjudicate();
		for (int i = 0; i < 10; i++) {
			grid.mutate();
		}

		glClear(GL_COLOR_BUFFER_BIT);
		grid.draw();
		SDL_GL_SwapBuffers();
	}

	return 0;
}
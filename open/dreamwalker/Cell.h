#ifndef __CELL_H__
#define __CELL_H__

#include "config.h"
#include "soy/vec2.h"
#include "Graphics.h"
#include <iostream>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>

using std::vector;

class Cell {
public:
	virtual ~Cell() { }
	virtual void draw(vec2 ll) const = 0;
	virtual void trigger() { }
	virtual bool concrete() const { return true; }
	virtual float elasticity() const { return 0; }
	virtual void step() { }
	virtual float jump_power(float in) const { return in; }
};


class CellBoard {
public:
	CellBoard(int w, int h) : width_(w), height_(h) {
		cells_.resize(w);
		for (int i = 0; i < w; i++) {
			cells_[i].resize(h);
		}
	}

	~CellBoard() {
		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++) {
				delete cells_[x][y];
			}
		}
	}

	int width()  const { return width_; }
	int height() const { return height_; }

	Cell* get(int x, int y) {
		if (0 <= x && x < width() && 0 <= y && y < height()) {
			return cells_[x][y];
		}
		else {
			return 0;
		}
	}

	Cell* set(int x, int y, Cell* cell) {
		if (0 <= x && x < width() && 0 <= y && y < height()) {
			cells_[x][y] = cell;
		}
		else {
			abort();
		}
	}

	void draw() const {
		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++) {
				if (cells_[x][y]) 
					cells_[x][y]->draw(vec2(x,y));
			}
		}
	}

	void step() {
		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++) {
				if (cells_[x][y]) 
					cells_[x][y]->step();
			}
		}
	}
private:
	vector< vector<Cell*> > cells_;
	int width_, height_;
};

class CellLevel {
public:
	CellLevel(int bw, int bh, int w, int h) 
		: boardwidth_(bw), boardheight_(bh),
		  width_(w), height_(h) {
			  boards_.resize(w);
			  for (int i = 0; i < w; i++) {
				  boards_[i].resize(h);
			  }
	}
	
	~CellLevel() {
		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++) {
				delete boards_[x][y];
			}
		}
	}

	int width() const { return boardwidth_ * width_; }
	int height() const { return boardheight_ * height_; }

	Cell* get(int x, int y) const {
		if (0 <= x && x < boardwidth_ * width_ &&
		    0 <= y && y < boardheight_ * height_) {
			int boardx = x / boardwidth_,
				boardy = y / boardheight_;
			if (boards_[boardx][boardy]) {
				return boards_[boardx][boardy]->get(x % boardwidth_, y % boardheight_);
			}
		}
		return 0;
	}

	void set_board(int bx, int by, CellBoard* board) {
		if (0 <= bx && bx < width_ &&
		    0 <= by && by < height_) {
			boards_[bx][by] = board;
		}
		else {
			abort();
		}
	}

	void draw() const {
		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++) {
				if (boards_[x][y]) {
					glPushMatrix();
					glTranslatef(x*boardwidth_, y*boardheight_, 0);
					boards_[x][y]->draw();
					glPopMatrix();
				}
			}
		}
	}

	void step() const {
		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++) {
				if (boards_[x][y]) {
					boards_[x][y]->step();
				}
			}
		}
	}
	
private:
	int width_, height_;
	int boardwidth_, boardheight_;
	vector< vector< CellBoard* > > boards_;
};


class BoxCell : public Cell {
public:
	BoxCell(float elastic = 0, Color color = Color(1,1,1)) 
		: elastic_(elastic), color_(color) 
	{ }
	
	void draw(vec2 ll) const {
		color_.set();
		glBegin(GL_QUADS);
			glVertex2f(ll.x,   ll.y);
			glVertex2f(ll.x+1, ll.y);
			glVertex2f(ll.x+1, ll.y+1);
			glVertex2f(ll.x,   ll.y+1);
		glEnd();
	}

	float elasticity() const { return elastic_; }
private:
	float elastic_;
	Color color_;
};


class InvisiBox : public Cell {
public:
	InvisiBox() : alpha_(0), target_(0) { }

	void draw(vec2 ll) const { 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1, 1, 1, alpha_);
		glBegin(GL_QUADS);
			glVertex2f(ll.x,   ll.y);
			glVertex2f(ll.x+1, ll.y);
			glVertex2f(ll.x+1, ll.y+1);
			glVertex2f(ll.x,   ll.y+1);
		glEnd();
		glDisable(GL_BLEND);
	}

	void step() { 
		alpha_ = (alpha_ + DT*target_) / (1+DT);
		target_ = 0;
	}

	void trigger() {
		target_ = 1;
	}

private:
	float alpha_;
	float target_;
};


class HintChain : public Cell {
public:
	HintChain() : hint_time_(HUGE_VAL), stay_time_(0), next_(0) { }

	void set_next(HintChain* next) { 
		next_ = next;
	}
	
	void draw(vec2 ll) const { 
		if (stay_time_ > 0) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1, 1, 1, 0.25);
			glBegin(GL_QUADS);
				glVertex2f(ll.x,   ll.y);
				glVertex2f(ll.x+1, ll.y);
				glVertex2f(ll.x+1, ll.y+1);
				glVertex2f(ll.x,   ll.y+1);
			glEnd();
			glDisable(GL_BLEND);
		}
	}

	void step() { 
		stay_time_ -= DT;
		if (stay_time_ < 0) stay_time_ = 0;
		hint_time_ -= DT;
		if (hint_time_ <= 0) {
			hint_time_ = HUGE_VAL;
			if (next_) next_->hint_time_ = 1;
			stay_time_ = 0.05;
		}
	}

	void trigger() { 
		hint_time_ = -1;
	}

private:
	float hint_time_;
	float stay_time_;
	HintChain* next_;
};


class HighJumpBox : public Cell {
public:
	HighJumpBox(float coeff) : coeff_(coeff) { }

	void draw(vec2 ll) const {
		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glVertex2f(ll.x,   ll.y);
			glVertex2f(ll.x+1, ll.y);
			glVertex2f(ll.x+1, ll.y+1);
			glVertex2f(ll.x,   ll.y+1);
		glEnd();
	}

	float jump_power(float in) const { 
		return coeff_;
	}
private:
	float coeff_;
};


class WinBox : public Cell {
public:
	WinBox() { }
	
	void draw(vec2 ll) const {
		glColor3f(1,0,0);
		glBegin(GL_QUADS);
			glVertex2f(ll.x,   ll.y);
			glVertex2f(ll.x+1, ll.y);
			glVertex2f(ll.x+1, ll.y+1);
			glVertex2f(ll.x,   ll.y+1);
		glEnd();
	}

	void trigger() {
		reset();
	}
};


#endif

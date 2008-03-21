#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include <vector>
#include <string>
#include <map>
#include <soy/Font.h>
#include <soy/ptr.h>

using std::vector;
using std::string;
using std::map;

const double DRAW_BORDER = 2;

template<class Rule> 
class Pattern
{
public:
	Pattern(vec2 size) : size_(size) {
		pattern_.resize(int(size.x));
		for (int i = 0; i < int(size.x); i++) {
			pattern_[i].resize(int(size.y));
		}
	}

	void set_at(vec2 place, typename Rule::State value) {
		pattern_[int(place.x)][int(place.y)] = value;
	}

	ptr< Pattern<Rule> > rotate() const {
		ptr<Pattern> ret = new Pattern(vec2(size_.y, size_.x));
		for (int i = 0; i < int(size_.x); i++) {
			for (int j = 0; j < int(size_.y); j++) {
				ret->set_at(vec2(j, size_.x - i - 1), get_at(vec2(i,j)));
			}
		}
		return ret;
	}
	
	typename Rule::State get_at(vec2 place) const {
		return pattern_[int(place.x)][int(place.y)];
	}

	vec2 get_size() const {
		return size_;
	}

    void draw_with_border(int X, int Y) {
        const float border = DRAW_BORDER;
        
        glColor4f(0,0,0, ALPHA_FACTOR);
        glBegin(GL_QUADS);
            glVertex2f(X,                      Y);
            glVertex2f(X + size_.x + 2*border, Y);
            glVertex2f(X + size_.x + 2*border, Y + size_.y + 2*border);
            glVertex2f(X,                      Y + size_.y + 2*border);
        glEnd();
        
        draw(X + 2, Y + 2);

		glColor4f(1.0, 1.0, 1.0, ALPHA_FACTOR);
        glLineWidth(2.0);
		glBegin(GL_LINE_LOOP);
			glVertex2f(X,                      Y);
			glVertex2f(X + size_.x + 2*border, Y);
			glVertex2f(X + size_.x + 2*border, Y + size_.y + 2*border);
			glVertex2f(X,                      Y + size_.y + 2*border);
		glEnd();
    }

	void draw(int X, int Y) const {
		for (int i = 0; i < size_.x; i++) {
			for (int j = 0; j < size_.y; j++) {
				Rule::draw_scalable(X + i + 0.5, Y + j + 0.5, pattern_[i][j], true);
			}
		}
	}

private:
	typedef vector<vector<typename Rule::State> > pattern_t;
	pattern_t pattern_;

	vec2 size_;
};

const double SHOW_DELAY = 1;

template<class Rule>
class Library
{
public:
	enum DrawPlace {TOP, BOTTOM};

	Library() 
		: place_(BOTTOM), show_delay_(0)
	{ }

	void add(ptr< Pattern<Rule> > pattern, string name = "") 
	{
		if (name == "") {
			std::stringstream ss;
			int number = 0;
			do {
				ss.str("");
				number++;
				ss<<"Pattern"<<number;
			} while (library_.find(ss.str()) != library_.end());

			name = ss.str();
		}
		library_[name] = pattern;
	}

	ptr< Pattern<Rule> > get(string name) {
		return library_[name];
	}

	void rename(string oldName, string newName) {
		if (oldName == newName) return;
		if (library_.find(oldName) == library_.end()) DIE("Renaming a pattern which doesn't exist.");

		library_[newName] = library_[oldName];
		library_.erase(oldName);
	}

	void remove(string name) {
		library_.erase(name);
	}

	void show_library() {
		show_delay_ = SHOW_DELAY;
	}

	void set_place(DrawPlace place) {
		place_ = place;
	}

    string select(vec2 sel) const {
		if (show_delay_ <= 0) { return ""; }
		sel = WORLDVIEW->mouse_to_world(sel);
		int start = 20;
		for (typename library_t::const_iterator i = library_.begin(); i != library_.end(); i++) {
            if (start <= sel.x && sel.x <= start + i->second->get_size().x + 2*DRAW_BORDER) {
                if (place_ == BOTTOM) {
                    if (3 <= sel.y && sel.y <= 3 + i->second->get_size().y + 2*DRAW_BORDER) {
                        return i->first;
                    }
                }
                else {
                    if (H - i->second->get_size().y - 8 <= sel.y && sel.y <= H - 8 + 2*DRAW_BORDER) {
                        return i->first;
                    }
                }
            }
            
			int addWidth = int(i->second->get_size().x);
			if (addWidth < 20) addWidth = 20;
            start += addWidth + 5;
        }

        return "";
    }

	void step() {
		show_delay_ -= DT;

		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (place_ == BOTTOM) {
			if (my > YRES - 75) {
				show_delay_ = SHOW_DELAY;
				return;
			}
		}
		if (place_ == TOP) {
			if (my < 75) {
				show_delay_ = SHOW_DELAY;
				return;
			}
		}

		if (select(vec2(mx,my)) != "") {
			show_delay_ = SHOW_DELAY;
		}
	}

	void draw() const {
		if (show_delay_ <= 0) { return; }
		//TTFont* font = new TTFont("/usr/share/fonts/TTF/Vera.ttf", 16);
        GlutFont* font = new GlutFont;

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, W, 0, H);
        glMatrixMode(GL_MODELVIEW);

		double old_afactor = ALPHA_FACTOR;
		if (show_delay_ < 1) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			ALPHA_FACTOR *= show_delay_;
		}


		int start = 20;
		for (typename library_t::const_iterator i = library_.begin(); i != library_.end(); i++) {
			glColor4f(1.0, 1.0, 1.0, ALPHA_FACTOR);
			if (place_ == BOTTOM) {
				font->draw_string(vec3(start,.06,0), 1, i->first);
				i->second->draw_with_border(start, 3);
			}
			else {
				font->draw_string(vec3(start, H-3.06,0), 1, i->first);
				i->second->draw_with_border(start, int(H - i->second->get_size().y - 8));
			}
			int addWidth = int(i->second->get_size().x);
			if (addWidth < 20) addWidth = 20;
			start += addWidth + 5;
		}

		delete font;

		glDisable(GL_BLEND);
		ALPHA_FACTOR = old_afactor;

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
	}

private:
	DrawPlace place_;
	double show_delay_;
	typedef map<string, ptr<Pattern<Rule> > > library_t;
	library_t library_;
};

template<class Rule>
class LibraryManager
{
public:
	LibraryManager (Library<Rule>* library) : library_(library) { }
    
    string select(vec2 sel) const {
        return library_->select(sel);
    }

	void add_pattern(ptr< Pattern<Rule> > pattern, typename Rule::Type player) {
		for (int i = 0; i < pattern->get_size().x; i++) {
			for (int j = 0; j < pattern->get_size().y; j++) {
				if (pattern->get_at(vec2(i, j)).player != player) {//XXX Rules aren't required to have players
					pattern->set_at(vec2(i, j), Rule::dead_state());
				}
			}
		}
        ptr< Pattern<Rule> > cropped = crop_pattern(pattern);
        if (cropped.valid()) {
            library_->add(crop_pattern(pattern));
        }
	}

	void draw() const {
		library_->draw();
	}

private:
	ptr< Pattern<Rule> > crop_pattern(ptr< Pattern<Rule> > pattern) {

		int minI = int(pattern->get_size().x) - 1;
		int maxI = 0;
		int minJ = int(pattern->get_size().y) - 1;
		int maxJ = 0;

        bool sawSomething = false;
		for (int i = 0; i < pattern->get_size().x; i++) {
			for (int j = 0; j < pattern->get_size().y; j++) {
				if (pattern->get_at(vec2(i, j)) != Rule::dead_state()) {
					if (i < minI) minI = i;
					if (i > maxI) maxI = i;
					if (j < minJ) minJ = j;
					if (j > maxJ) maxJ = j;
                    sawSomething = true;
				}
			}
		}

        if (!sawSomething) {
            return 0;
        }

		vec2 newSize = vec2(maxI - minI + 1,
						    maxJ - minJ + 1);
		ptr< Pattern<Rule> > newPattern = new Pattern<Rule>(newSize);

		for (int i = minI; i < maxI + 1; i++) {
			for (int j = minJ; j < maxJ + 1; j++) {
				newPattern->set_at(vec2(i - minI, j - minJ), pattern->get_at(vec2(i, j)));
			}
		}
		return newPattern;
	}
	
	Library<Rule>* library_;
};

#endif

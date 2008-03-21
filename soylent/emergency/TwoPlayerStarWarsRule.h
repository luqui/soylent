#ifndef __TWOPLAYERSTARWARSRULE_H__
#define __TWOPLAYERSTARWARSRULE_H__

#include "config.h"
#include <cassert>
#include "Utils.h"
#include <soy/util.h>

class TwoPlayerStarWarsRule
{
public:

	static const int C;
    static const int EXPLODEC;
	
    enum Type { DEAD, PLAYER1, PLAYER2, EXPLODY, NUM_TYPES };
    
    static Type other_player(Type in) {
        if (in == PLAYER1) return PLAYER2;
        if (in == PLAYER2) return PLAYER1;
        return in;
    }
    
	struct State {
		State(Type player = DEAD, int life = 0) : player(player), life(life) { }
		Type player;
		int life;

		bool operator == (const State& state) const {
			return player == state.player && life == state.life;
		}

		bool operator != (const State& state) const {
			return !(*this == state);
		}
	};

	struct StateBag {
		StateBag() 
		{
			for (int i = 0; i < NUM_TYPES; i++) {
				count[i] = 0;
			}
		}

		void add(State s) {
            if (s.player == DEAD)  count[DEAD]++;
            else if (s.player == EXPLODY && s.life == EXPLODEC) {
                count[EXPLODY]++;
            }
            else if (s.life == C) count[s.player]++;
		}

        void remove(State s) {
            if (s.player == DEAD)  count[DEAD]--;
            else if (s.player == EXPLODY && s.life == EXPLODEC) {
                count[EXPLODY]--;
            }
            else if (s.life == C) count[s.player]--;
        }
													  
		int count[NUM_TYPES];
	};

	static StateBag make_state_bag() {
		return StateBag();
	}

	static State next_state(State state, StateBag count) {
        switch (state.player) {
            case DEAD: {
                if (state.player != EXPLODY) {
                    if (count.count[EXPLODY] > 0) {
                        int neighbors = count.count[EXPLODY];
                        if (neighbors == 3 || neighbors == 4
                         || neighbors == 6 || neighbors == 8) {
                            return State(EXPLODY, EXPLODEC);
                        }
                    }
                }


                if (count.count[PLAYER1] == 2 && count.count[PLAYER2] == 0) {
                    return State(PLAYER1, C);
                }
                if (count.count[PLAYER1] == 0 && count.count[PLAYER2] == 2) {
                    return State(PLAYER2, C);
                }
                if (count.count[PLAYER1] >= 1 && count.count[PLAYER2] >= 1) {
                    return State(EXPLODY, EXPLODEC);
                }
                return State(DEAD, 0);
            }
            break;
            
            case PLAYER1:
            case PLAYER2: {
                if (state.life == 1) {
                    return State(DEAD, 0);
                }
                if (state.life < C) {
                    return State(state.player, state.life-1);
                }
                if (count.count[EXPLODY] > 0) {
                    return State(state.player, state.life-1);
                }
                if (count.count[other_player(state.player)] != 0) {
                    return State(EXPLODY, EXPLODEC);
                }
                int neighbors = count.count[state.player];
                if (neighbors == 3 || neighbors == 4
                 || neighbors == 5) {
                    // stay put
                    return State(state.player, state.life);
                }
                return State(state.player, state.life-1);
            }
            break;

            case EXPLODY: {
                if (state.life == 1) {
                    return State(DEAD, 0);
                }
                if (state.life < EXPLODEC) {
                    return State(EXPLODY, state.life-1);
                }

                int neighbors = count.count[EXPLODY] + count.count[PLAYER1] + count.count[PLAYER2];
                
                if (neighbors == 2
                 || neighbors == 3 || neighbors == 5
                 || neighbors == 6 || neighbors == 7
                 || neighbors == 8) {
                    return State(EXPLODY, state.life);
                }
                return State(EXPLODY, state.life-1);
            }
            break;
            default: {
                abort();
            }
        }
	}

    // returns whether we should continue drawing
    // (no if it's black)
    static bool activate_color(const State& state, bool niggers = false) {
		switch (state.player) {
			case PLAYER1:
				glColor4f(float(state.life)/C, 0, 0, ALPHA_FACTOR);
				break;
			case PLAYER2:
				glColor4f(0, float(state.life)/C, 0, ALPHA_FACTOR);
				break;
			case EXPLODY: {
                float color = float(state.life)/EXPLODEC;
				glColor4f(color, color, color, ALPHA_FACTOR);
				break;
            }
            default:
                if (niggers) {
                    glColor4f(0,0,0, ALPHA_FACTOR);
                    return true;
                }
                else {
                    return false;
                }
		}
        return true;
    }

	static void draw(float X, float Y, const State& state, bool niggers = false) {
        if (!activate_color(state, niggers)) return;
        
		glBegin(GL_POINTS);
		glVertex2f(X,Y);
		glEnd();
	}

    static void draw_scalable(float X, float Y, const State& state, bool niggers = false) {
        if (!activate_color(state, niggers)) return;
        
        glBegin(GL_QUADS);
            glVertex2f(X-0.5, Y-0.5);
            glVertex2f(X+0.5, Y-0.5);
            glVertex2f(X+0.5, Y+0.5);
            glVertex2f(X-0.5, Y+0.5);
        glEnd();
    }

    static State prune(State state, StateBag population) {
        switch (state.player) {
            case PLAYER1:
            case PLAYER2: {
                if (state.life < C) return state;
                /*if (population.count[state.player] > 1000) {
                    int diff = population.count[state.player] - 1000;
                    // probability is diff / population
                    if (randrange(0,1) < float(diff) / population.count[state.player]) {
                        // kill it
                        return State(state.player, state.life - 1);
                    }
                }*/
            }
            break;
            default:
            break;
        }

        return state;
    }

	static State alive_state(Type player) {
		return State(player, C);
	}

	static State dead_state() {
		return State(DEAD, 0);
	}
};

const int TwoPlayerStarWarsRule::C = 4;
const int TwoPlayerStarWarsRule::EXPLODEC = 9;

#endif

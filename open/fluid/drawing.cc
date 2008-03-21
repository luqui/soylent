#include "drawing.h"
#include "FluidGrid.h"
#include "util.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

static float TIMING = 0;
static float TIMING1 = 0;
static float TIMING2 = 0;
static float TIMING3 = 0;
static float TIMING4 = 0;
static float TIMING5 = 0;
static float TIMING6 = 0;

void scale_color(float density, float alpha) {
	float d = 20*density;
	if (d < 0) {
		glColor4f(-TIMING1*d, -TIMING2*d, -TIMING3*d, alpha);
	}
	else {
		glColor4f(TIMING4*d, TIMING5*d, TIMING6*d, alpha);
	}
}

void set_color_at(const GPUFluid* field, int x, int y) {
	scale_color(field->get_density_direct(x,y));
}

void draw_rect(vec ll, vec ur) {
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);  glVertex2f(ll.x,ll.y);
		glTexCoord2f(1,1);  glVertex2f(ur.x,ll.y);
		glTexCoord2f(1,0);  glVertex2f(ur.x,ur.y);
		glTexCoord2f(0,0);  glVertex2f(ll.x,ur.y);
	glEnd();
}

void init_color() {
	TIMING = randrange(-10000,10000);
}

void draw_board(const GPUFluid* field, int w, int h) {
    g_pFlo->SetInkColor(g_rInkRGB);

	if (!b['p'] || b['.'])
	{
		b['.'] = false;

        // cycle ink color
        if (b['c'] && (TIMING1 > 100))
        {
			TIMING1 = 0;
            static int sColor = 0;
            sColor = (sColor + 1) % 7;
            g_rInkRGB[0] = 0.15f * (sColor & 0x1) +
                           0.05f * ((sColor & 0x2) / 2);
            g_rInkRGB[1] = 0.15f * ((sColor & 0x2) / 2) +
                           0.05f * ((sColor & 0x4) / 4);
            g_rInkRGB[2] = 0.15f * ((sColor & 0x4) / 4) +
                           0.05f * (sColor & 0x1);
            if (sColor > 699) sColor = 0;
        }

		// set parameters that may have changed
		g_pFlo->SetViscosity(g_rViscosity);
		g_pFlo->EnablePressureClear(b['C']);
		g_pFlo->SetNumPoissonSteps(g_iNumPoissonSteps);
		g_pFlo->SetInkColor(g_rInkRGB);
		g_pFlo->SetInkLongevity(g_rInkLongevity);
		g_pFlo->SetTimeStep(g_rTimestep);
		g_pFlo->SetGridScale(g_rGridScale);
		g_pFlo->SetVorticityConfinementScale(g_rVCScale);

		if (g_displayMode == Flo::DISPLAY_VORTICITY || b['v'])
			g_pFlo->EnableVorticityComputation(true);
		else
			g_pFlo->EnableVorticityComputation(false);
	}
	if (b['D'])
	{
		// Display the fluid.
        if (b['0'])
		    g_pFlo->Display(g_displayMode, b['b'], b['m'], b['o'], b['w']);
        else
            glClear(GL_COLOR_BUFFER_BIT);

        if (g_vectorDisplayMode != Flo::VECTOR_DISPLAY_NONE)
            g_pFlo->DisplayVectorField(g_vectorDisplayMode, g_iLineWidth,
                                       g_iPointSize, g_rVectorScale);
	}

	/*float norm;
	TIMING1 = sin(TIMING/(M_PI+1))+1;
	TIMING2 = sin(TIMING/(2*M_PI))+1;
	TIMING3 = sin(TIMING/(exp(float(2))))+1;
	norm = sqrt(TIMING1*TIMING1 + TIMING2*TIMING2 + TIMING3*TIMING3)/3;
	TIMING1 /= norm;  TIMING2 /= norm;  TIMING3 /= norm;
	
	TIMING4 = sin(TIMING/8)+1;
	TIMING5 = sin(TIMING/sqrt(float(10)))+1;
	TIMING6 = sin(TIMING/sqrt(float(11)))+1;
	norm = sqrt(TIMING4*TIMING4 + TIMING5*TIMING5 + TIMING6*TIMING6)/3;
	TIMING4 /= norm;  TIMING5 /= norm;  TIMING6 /= norm;
	
	float colordist = (TIMING1-TIMING4)*(TIMING1-TIMING4) 
	                + (TIMING2-TIMING5)*(TIMING2-TIMING5)
					+ (TIMING3-TIMING6)*(TIMING3-TIMING6);
	
	if (colordist < MIN_COLOR_NORM) {
		const float offs = 1/MAX_COLOR_CHANGE_RATE;
		const float mul = (1/COLOR_CHANGE_RATE - offs) / MIN_COLOR_NORM;
		TIMING += 1/(mul*colordist + offs)*DT;
	}
	else {
		TIMING += COLOR_CHANGE_RATE*DT;
	}
	
	glBegin(GL_QUADS);
	for (int i = 0; i < w-1; i++) {
		for (int j = 0; j < h-1; j++) {
			set_color_at(field,i,j);
			glVertex2f(i,j);
			set_color_at(field,i+1,j);
			glVertex2f(i+1,j);
			set_color_at(field,i+1,j+1);
			glVertex2f(i+1,j+1);
			set_color_at(field,i,j+1);
			glVertex2f(i,j+1);
		}
	}
	glEnd();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0,0,0,0.25);
	glBegin(GL_QUADS);
		glVertex2f(0,0);
		glVertex2f(W,0);
		glVertex2f(W,CLAMPH);
		glVertex2f(0,CLAMPH);

		glVertex2f(0,H-CLAMPH);
		glVertex2f(W,H-CLAMPH);
		glVertex2f(W,H);
		glVertex2f(0,H);

		glVertex2f(0,CLAMPH);
		glVertex2f(CLAMPW,CLAMPH);
		glVertex2f(CLAMPW,H-CLAMPH);
		glVertex2f(0,H-CLAMPH);

		glVertex2f(W-CLAMPW,CLAMPH);
		glVertex2f(W,CLAMPH);
		glVertex2f(W,H-CLAMPH);
		glVertex2f(W-CLAMPW,H-CLAMPH);
	glEnd();
	glDisable(GL_BLEND);*/
}

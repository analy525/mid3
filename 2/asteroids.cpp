//modified by: Analy Velazquez	
//date: February 2	
//purpose: Fix the lab
//
//program: asteroids
//author:  Gordon Griesel
//date:    2014
//mod spring 2015: added constructors
//mod spring 2016: prepare for class
//mod spring 2017: modifications for lab
//
//Objectives:
//The program does not work properly because neither bullets nor asteroids
//are being deleted properly. 
//Search for the text: "Delete bullet here"
//
//Asteroids are in a doubly-linked list.
//Fix the deleteAsteroid() function.
//
//
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "ppm.h"
#include "log.h"
extern "C" {
	#include "fonts.h"
}

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt	Matrix[4][4];

//macros
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                      (c)[1]=(a)[1]-(b)[1]; \
                      (c)[2]=(a)[2]-(b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define PI 3.141592653589793
#define ALPHA 1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;


//=======================================================================
//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;
//unsigned int upause=0;
double timeDiff(struct timespec *start, struct timespec *end) {
	return (double)(end->tv_sec - start->tv_sec ) +
			(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source) {
	memcpy(dest, source, sizeof(struct timespec));
}
//=======================================================================

int xres=1250, yres=900;

struct Ship {
	Vec dir;
	Vec pos;
	Vec vel;
	float angle;
	float color[3];
	Ship() {
		VecZero(dir);
		pos[0] = (Flt)(xres/2);
		pos[1] = (Flt)(yres/2);
		pos[2] = 0.0f;
		VecZero(vel);
		angle = 0.0;
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
	}
};

struct Bullet {
	Vec pos;
	Vec vel;
	float color[3];
	struct timespec time;
	struct Bullet *prev;
	struct Bullet *next;
	Bullet() {
		prev = NULL;
		next = NULL;
	}
};

struct Asteroid {
	Vec pos;
	Vec vel;
	int nverts;
	Flt radius;
	Vec vert[8];
	float angle;
	float rotate;
	float color[3];
	struct Asteroid *prev;
	struct Asteroid *next;
	Asteroid() {
		prev = NULL;
		next = NULL;
	}
};

const int MAX_BULLETS = 1000;
 
struct Game {
	Ship ship;
	Bullet *barr;
	Asteroid *ahead;
	int nasteroids;
	int nbullets;
	struct timespec bulletTimer;
	Game() {
		ahead = NULL;
		barr = new Bullet[MAX_BULLETS];
		nasteroids = 0;
		nbullets = 0;
	}
};
Asteroid *ahead =NULL;
int keys[65536];

//function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_resize(XEvent *e);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e);
void init(Game *g);
void init_sounds(void);
void physics(Game *game);
void render(Game *game);

int main(void)
{
	logOpen();
	initXWindows();
	init_opengl();
	Game game;
	init(&game);
	srand(time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	int done=0;
	while (!done) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_resize(&e);
			check_mouse(&e, &game);
			done = check_keys(&e);
		}
		clock_gettime(CLOCK_REALTIME, &timeCurrent);
		timeSpan = timeDiff(&timeStart, &timeCurrent);
		timeCopy(&timeStart, &timeCurrent);
		physicsCountdown += timeSpan;
		while (physicsCountdown >= physicsRate) {
			physics(&game);
			physicsCountdown -= physicsRate;
		}
		render(&game);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	cleanup_fonts();
	logClose();
	return 0;
}

void cleanupXWindows(void)
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "3350 - Asteroids template");
}

void setup_screen_res(const int w, const int h)
{
	xres = w;
	yres = h;
}

void initXWindows(void)
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
	XSetWindowAttributes swa;
	setup_screen_res(xres, yres);
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
			vi->depth, InputOutput, vi->visual,
			CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void reshape_window(int width, int height)
{
	//window has been resized.
	setup_screen_res(width, height);
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, xres, 0, yres, -1, 1);
	set_title();
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, xres, yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, xres, 0, yres, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}

void check_resize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != xres || xce.height != yres) {
		//Window size did change.
		reshape_window(xce.width, xce.height);
	}
}

void init(Game *g) {
	//build 10 asteroids...
	for (int j=0; j<10; j++) {
		Asteroid *a = new Asteroid;
		a->nverts = 8;
		a->radius = rnd()*80.0f + 40.0f;
		Flt r2 = a->radius / 2.0f;
		Flt angle = 0.0f;
		Flt inc = (PI * 2.0f) / (Flt)a->nverts;
		for (int i=0; i<a->nverts; i++) {
			a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
			a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
			angle += inc;
		}
		a->pos[0] = (Flt)(rand() % xres);
		a->pos[1] = (Flt)(rand() % yres);
		a->pos[2] = 0.0f;
		a->angle = 0.0f;
		a->rotate = rnd() * 4.0f - 2.0f;
		a->color[0] = 0.8f;
		a->color[1] = 0.8f;
		a->color[2] = 0.7f;
		a->vel[0] = (Flt)(rnd()*2.0f-1.0f);
		a->vel[1] = (Flt)(rnd()*2.0f-1.0f);
		std::cout << "asteroid" << std::endl;
		//add to front of linked list
		a->next = g->ahead;
		if (g->ahead != NULL)
			g->ahead->prev = a;
		g->ahead = a;
		g->nasteroids++;
	}
	clock_gettime(CLOCK_REALTIME, &g->bulletTimer);
	memset(keys, 0, 65536);
}

void normalize(Vec v) {
	Flt len = v[0]*v[0] + v[1]*v[1];
	if (len == 0.0f) {
		v[0] = 1.0f;
		v[1] = 0.0f;
		return;
	}
	len = 1.0f / sqrt(len);
	v[0] *= len;
	v[1] *= len;
}

void check_mouse(XEvent *e, Game *g)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
		}
		if (e->xbutton.button==3) {
			//Right button is down
		}
	}
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		//Mouse moved
		int xdiff = savex - e->xbutton.x;
		//int ydiff = savey - e->xbutton.y;
		savex = e->xbutton.x;
		savey = e->xbutton.y;

		g->ship.angle += xdiff;
		if (g->ship.angle >= 360.0f)
			g->ship.angle -= 360.0f;
		if (g->ship.angle < 0.0f)
			g->ship.angle += 360.0f;
	}
}

int check_keys(XEvent *e)
{
	//keyboard input?
	static int shift=0;
	int key = XLookupKeysym(&e->xkey, 0);
	//
	//This code maintains an array of key status values.
	if (e->type == KeyRelease) {
		keys[key]=0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift=0;
		return 0;
	}
	if (e->type == KeyPress) {
		keys[key]=1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			shift=1;
			return 0;
		}
	} else {
		return 0;
	}
	if (shift){}
	switch(key) {
		case XK_Escape:
			return 1;
		case XK_f:
			break;
		case XK_s:
			break;
		case XK_Down:
			break;
		case XK_equal:
			break;
		case XK_minus:
			break;
	}
	return 0;
}

void deleteAsteroid(Asteroid *node)
{
	//if (g) {}    //you can remove this line
	//if (node) {} //you can remove this line

	//to do:
	//Delete a node from asteroid linked-list
	
    if (node->prev ==NULL){
	if(node->next ==NULL){
	    ahead = NULL;
	} else{
	    node->next->prev = NULL;
	    ahead = node->next;
	}
    } else{
	if (node->next == NULL){
	node->prev->next = NULL;
	}else {
	    node->prev->next = node->next;
	    node->next->prev = node->prev;
	}
    }
    free(node);
    node = NULL;
}


		
	

void buildAsteroidFragment(Asteroid *ta, Asteroid *a)
{
	//build ta from a
	ta->nverts = 8;
	ta->radius = a->radius / 2.0f;
	Flt r2 = ta->radius / 2.0f;
	Flt angle = 0.0f;
	Flt inc = (PI * 2.0f) / (Flt)ta->nverts;
	for (int i=0; i<ta->nverts; i++) {
		ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
		ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
		angle += inc;
	}
	ta->pos[0] = a->pos[0] + rnd()*10.0f-5.0f;
	ta->pos[1] = a->pos[1] + rnd()*10.0f-5.0f;
	ta->pos[2] = 0.0f;
	ta->angle = 0.0f;
	ta->rotate = a->rotate + (rnd() * 4.0f - 2.0f);
	ta->color[0] = 0.8f;
	ta->color[1] = 0.8f;
	ta->color[2] = 0.7f;
	ta->vel[0] = a->vel[0] + (rnd()*2.0f-1.0f);
	ta->vel[1] = a->vel[1] + (rnd()*2.0f-1.0f);
	//std::cout << "frag" << std::endl;
}

void physics(Game *g)
{
	Flt d0,d1,dist;
	//Update ship position
	g->ship.pos[0] += g->ship.vel[0];
	g->ship.pos[1] += g->ship.vel[1];
	//Check for collision with window edges
	if (g->ship.pos[0] < 0.0f) {
		g->ship.pos[0] += (float)xres;
	}
	else if (g->ship.pos[0] > (float)xres) {
		g->ship.pos[0] -= (float)xres;
	}
	else if (g->ship.pos[1] < 0.0f) {
		g->ship.pos[1] += (float)yres;
	}
	else if (g->ship.pos[1] > (float)yres) {
		g->ship.pos[1] -= (float)yres;
	}
	//
	//
	//Update bullet positions
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	for (int i=0; i<g->nbullets; i++) {
		Bullet *b = &g->barr[i];
		//How long has bullet been alive?
		double ts = timeDiff(&b->time, &bt);
		if (ts > 2.5f) {
		//Delete bullet here
				
			continue;
		}
		//move the bullet
		b->pos[0] += b->vel[0];
		b->pos[1] += b->vel[1];
		//Check for collision with window edges
		if (b->pos[0] < 0.0f) {
			b->pos[0] += (float)xres;
		}
		else if (b->pos[0] > (float)xres) {
			b->pos[0] -= (float)xres;
		}
		else if (b->pos[1] < 0.0f) {
			b->pos[1] += (float)yres;
		}
		else if (b->pos[1] > (float)yres) {
			b->pos[1] -= (float)yres;
		}
		//b = b->next;
	}
	//
	//Update asteroid positions
	Asteroid *a = g->ahead;
	while (a) {
		a->pos[0] += a->vel[0];
		a->pos[1] += a->vel[1];
		//Check for collision with window edges
		if (a->pos[0] < -100.0f) {
			a->pos[0] += (float)xres+200;
		}
		else if (a->pos[0] > (float)xres+100.0f) {
			a->pos[0] -= (float)xres+200.0f;
		}
		else if (a->pos[1] < -100.0f) {
			a->pos[1] += (float)yres+200.0f;
		}
		else if (a->pos[1] > (float)yres+100.0f) {
			a->pos[1] -= (float)yres+200.0f;
		}
		a->angle += a->rotate;
		a = a->next;
	}
	//
	//Asteroid collision with bullets?
	//If collision detected:
	//     1. delete the bullet
	//     2. break the asteroid into pieces
	//        if asteroid small, delete it

	a = g->ahead;
	while (a) {
		//is there a bullet within its radius?
		for (int i=0; i<g->nbullets; i++) {
			Bullet *b = &g->barr[i];
			d0 = b->pos[0] - a->pos[0];
			d1 = b->pos[1] - a->pos[1];
			dist = (d0*d0 + d1*d1);
			if (dist < (a->radius*a->radius)) {
				//std::cout << "asteroid hit." << std::endl;
				//this asteroid is hit.
				if (a->radius > 20.0f) {
					//break it into pieces.
					Asteroid *ta = a;
					buildAsteroidFragment(ta, a);
					int r = rand() % 10 + 5;
					for (int k=0; k<r; k++) {
						//get the next asteroid position in the array
						Asteroid *ta = new Asteroid;
						buildAsteroidFragment(ta, a);
						//add to front of asteroid linked list
						ta->next = g->ahead;
						if (g->ahead != NULL)
							g->ahead->prev = ta;
						g->ahead = ta;
						g->nasteroids++;
					}
				} else {
					a->color[0] = 1.0f;
					a->color[1] = 0.1f;
					a->color[2] = 0.1f;
					//asteroid is too small to break up
					//delete the asteroid and bullet
					Asteroid *savea = a->next;
					deleteAsteroid(a);
					a = savea;
					g->nasteroids--;
				}
				//Delete bullet here.
				//How?
				//Move the array's last element to where this element is.	
				//Then update the array count, nbullets.
				//--like we did with water particles--
	
				
								
				if (a == NULL)
					break;
				continue;
			}
			b = b->next;
		}
		if (a == NULL)
			break;
		a = a->next;
	}
	//---------------------------------------------------
	//check keys pressed now
	if (keys[XK_Left]) {
		g->ship.angle += 4.0f;
		if (g->ship.angle >= 360.0f)
			g->ship.angle -= 360.0f;
	}
	if (keys[XK_Right]) {
		g->ship.angle -= 4.0f;
		if (g->ship.angle < 0.0f)
			g->ship.angle += 360.0f;
	}
	if (keys[XK_Up]) {
		//apply thrust
		//convert ship angle to radians
		Flt rad = ((g->ship.angle+90.0f) / 360.0f) * PI * 2.0f;
		//convert angle to a vector
		Flt xdir = cos(rad);
		Flt ydir = sin(rad);
		g->ship.vel[0] += xdir*0.02f;
		g->ship.vel[1] += ydir*0.02f;
		Flt speed = sqrt(g->ship.vel[0]*g->ship.vel[0]+
										g->ship.vel[1]*g->ship.vel[1]);
		if (speed > 10.0f) {
			speed = 10.0f;
			normalize(g->ship.vel);
			g->ship.vel[0] *= speed;
			g->ship.vel[1] *= speed;
		}
	}
	if (keys[XK_space]) {
		//a little time between each bullet
		struct timespec bt;
		clock_gettime(CLOCK_REALTIME, &bt);
		double ts = timeDiff(&g->bulletTimer, &bt);
		if (ts > 0.1) {
			timeCopy(&g->bulletTimer, &bt);
			//shoot a bullet...
			Bullet *b = &g->barr[g->nbullets];
			timeCopy(&b->time, &bt);
			b->pos[0] = g->ship.pos[0];
			b->pos[1] = g->ship.pos[1];
			b->vel[0] = g->ship.vel[0];
			b->vel[1] = g->ship.vel[1];
			//convert ship angle to radians
			Flt rad = ((g->ship.angle+90.0f) / 360.0f) * PI * 2.0f;
			//convert angle to a vector
			Flt xdir = cos(rad);
			Flt ydir = sin(rad);
			b->pos[0] += xdir*20.0f;
			b->pos[1] += ydir*20.0f;
			b->vel[0] += xdir*6.0f + rnd()*0.1f;
			b->vel[1] += ydir*6.0f + rnd()*0.1f;
			b->color[0] = 1.0f;
			b->color[1] = 1.0f;
			b->color[2] = 1.0f;
			g->nbullets++;
		}
	}
}

void render(Game *g)
{
	//float wid;
	Rect r;
	glClear(GL_COLOR_BUFFER_BIT);
	//
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ff0000, "cs335 - Asteroids");
	ggprint8b(&r, 16, 0x00ffff00, "n bullets: %i", g->nbullets);
	ggprint8b(&r, 16, 0x00ffff00, "n asteroids: %i", g->nasteroids);
	//-------------------------------------------------------------------------
	//Draw the ship
	glColor3fv(g->ship.color);
	glPushMatrix();
	glTranslatef(g->ship.pos[0], g->ship.pos[1], g->ship.pos[2]);
	//float angle = atan2(ship.dir[1], ship.dir[0]);
	glRotatef(g->ship.angle, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	//glVertex2f(-10.0f, -10.0f);
	//glVertex2f(  0.0f, 20.0f);
	//glVertex2f( 10.0f, -10.0f);
	glVertex2f(-12.0f, -10.0f);
	glVertex2f(  0.0f,  20.0f);
	glVertex2f(  0.0f,  -6.0f);
	glVertex2f(  0.0f,  -6.0f);
	glVertex2f(  0.0f,  20.0f);
	glVertex2f( 12.0f, -10.0f);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glPopMatrix();
	if (keys[XK_Up]) {
		int i;
		//draw thrust
		Flt rad = ((g->ship.angle+90.0f) / 360.0f) * PI * 2.0f;
		//convert angle to a vector
		Flt xdir = cos(rad);
		Flt ydir = sin(rad);
		Flt xs,ys,xe,ye,r;
		glBegin(GL_LINES);
		for (i=0; i<16; i++) {
			xs = -xdir * 11.0f + rnd() * 4.0f - 2.0f;
			ys = -ydir * 11.0f + rnd() * 4.0f - 2.0f;
			r = rnd()*40.0f + 40.0f;
			xe = -xdir * r + rnd() * 18.0f - 9.0f;
			ye = -ydir * r + rnd() * 18.0f - 9.0f;
			glColor3f(rnd() * 0.3f + 0.7f, rnd() * 0.3f + 0.7f, 0);
			glVertex2f(g->ship.pos[0]+xs,g->ship.pos[1]+ys);
			glVertex2f(g->ship.pos[0]+xe,g->ship.pos[1]+ye);
		}
		glEnd();
	}
	//-------------------------------------------------------------------------
	//Draw the asteroids
	Asteroid *a = g->ahead;
	while (a) {
		//Log("draw asteroid...\n");
		glColor3fv(a->color);
		glPushMatrix();
		glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
		glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
		glBegin(GL_LINE_LOOP);
		//Log("%i verts\n",a->nverts);
		for (int j=0; j<a->nverts; j++) {
			glVertex2f(a->vert[j][0], a->vert[j][1]);
		}
		glEnd();
		//glBegin(GL_LINES);
		//	glVertex2f(0,   0);
		//	glVertex2f(a->radius, 0);
		//glEnd();
		glPopMatrix();
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
		glVertex2f(a->pos[0], a->pos[1]);
		glEnd();
		a = a->next;
	}
	//-------------------------------------------------------------------------
	//Draw the bullets
	for (int i=0; i<g->nbullets; i++) {
		Bullet *b = &g->barr[i];
		//Log("draw bullet...\n");
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_POINTS);
		glVertex2f(b->pos[0],      b->pos[1]);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]);
		glVertex2f(b->pos[0],      b->pos[1]-1.0f);
		glVertex2f(b->pos[0],      b->pos[1]+1.0f);
		glColor3f(0.8f, 0.8f, 0.8f);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]-1.0f);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]+1.0f);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]-1.0f);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]+1.0f);
		glEnd();
	}
}




#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL3/SDL.h>
#include <cmath>
#include <sys/time.h>
#include <vector>

typedef struct v2d {
  double x;
  double y;
} v2d;

v2d v2dc(double x, double y) {
  v2d a;
  a.x=x;
  a.y=y;
  return a; 
}

SDL_FColor CtoF(SDL_Color c) {
  SDL_FColor col;
  col.a= (float)c.a/255;
  col.r= (float)c.r/255;
  col.g= (float)c.g/255;
  col.b= (float)c.b/255;
  return col;
}

struct clock {
  int radius;
  v2d pos;

};
double sina(double x) {
  return sin(x*M_PI/180);
}
double cosa(double x) {
  return cos(x*M_PI/180);
}
void drawfilledcircle(SDL_Renderer *rend,v2d pos, double r,double segments, SDL_Color c) {
  double dx = 0.0001;
  std::vector<SDL_Vertex> vertices;
  std::vector<int> indices;
  for(int x = 0; x < segments; x++) {
    indices.push_back(x);
  }
  for(double n = 0; n <2*M_PI;(n+=2*M_PI)/segments) {
    SDL_Vertex v = {
      .position={pos.x+cos(n)*r,pos.y+sin(n)*r},
      .color=CtoF(c),
      .tex_coord={0.0f,0.0f}
    };
    vertices.push_back(v);
  }
  SDL_RenderGeometry(rend,nullptr,vertices.data(),vertices.size(),indices.data(),segments);
}
void drawcircle(SDL_Renderer *rend,v2d pos, double r,double segments) {
  double dx = 0.0001;
  std::vector<SDL_FPoint> vertices;
  for(double n = 0; n <2*M_PI;n+=(2*M_PI)/segments) {

    double x = (pos.x+cos(n)*r);
    double y = (pos.y+sin(n)*r);
    SDL_FPoint point = {
      .x = x,
      .y = y
    };
    vertices.push_back(point);
  }
  vertices.push_back(vertices.front());
  SDL_RenderLines(rend,vertices.data(),vertices.size());
}
void clockthing(SDL_Renderer *rend,v2d pos, double rad,double inrad,double segments) {
  double dx = 0.0001;
  for(double n = 0; n <2*M_PI;n+=(2*M_PI)/segments) {

    double x = (pos.x+cos(n)*(rad-inrad));
    double y = (pos.y+sin(n)*(rad-inrad));
    
    drawcircle(rend,v2dc(x,y),inrad,10);
  }
}
int main(int argc, char**argv) {
  double wwidth = 300;
  double rad = wwidth/2;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *w = SDL_CreateWindow("CDU | Clock",wwidth,wwidth,SDL_WINDOW_ALWAYS_ON_TOP);
  SDL_Renderer *r = SDL_CreateRenderer(w,NULL);
  SDL_Event e;

  bool running = true;
  while(running) {
    while(SDL_PollEvent(&e)) {
      switch (e.type)
      {
      case SDL_EVENT_QUIT:
        running = false;
        break;
      default:
        break;
      }
    }

    time_t now = time(NULL);
    struct tm* local = localtime(&now);

    double hours =(double) local->tm_hour;
    double minutes =(double) local->tm_min;
    double seconds = (double)local->tm_sec;
    

    struct timeval tv;
    gettimeofday(&tv,NULL);
    double milliseconds = (double)tv.tv_usec / 1000;



    double hr_a = ((hours+(minutes/60))/12)*360-180;
    double min_a = ((minutes+(seconds/60))/60)*360-180;
    double sec_a = ((seconds+(milliseconds/1000))/60)*360-180;

    //Render Minute and Hour hands!!
    v2d center = v2dc(rad,rad);
    v2d hour_normalised = v2dc(sina(-hr_a),cosa(-hr_a));
    v2d min_normalised = v2dc(sina(-min_a),cosa(-min_a));
    v2d sec_normalised = v2dc(sina(-sec_a),cosa(-sec_a));

    double c_r = rad*0.7;
    //hr
    SDL_SetRenderDrawColor(r,120,120,255,0);
    SDL_RenderLine(r,rad,rad,rad+hour_normalised.x*c_r*0.6,rad+hour_normalised.y*c_r*0.3);
    //min
    SDL_SetRenderDrawColor(r,0,255,0,255);
    SDL_RenderLine(r,rad,rad,rad+min_normalised.x*c_r*0.7,rad+min_normalised.y*c_r*0.7);
    //sec
    SDL_SetRenderDrawColor(r,255,0,0,255);
    SDL_RenderLine(r,rad,rad,rad+sec_normalised.x*c_r,rad+sec_normalised.y*c_r*0.9);


    SDL_RenderPresent(r);
    SDL_SetRenderDrawColor(r,0,0,0,255);
    SDL_RenderClear(r);
    SDL_SetRenderDrawColor(r,255,255,255,255);
    drawcircle(r,v2dc(rad,rad),c_r,36);
    clockthing(r,v2dc(rad,rad),c_r,5,12);
    SDL_SetRenderDrawColor(r,255,0,0,255);

  }

}
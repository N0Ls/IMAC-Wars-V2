#ifndef MENU_H
#define MENU_H

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL_mixer.h>

static const unsigned int NUMMENU = 2;

class Menu
{

public :
    Menu();
    ~Menu();
    SDL_Surface *surface;
    int showMain(const char* title, int width, int height);
    void showPause(SDL_Surface* screen);
    bool running() { return isActiv; };

private :
    bool isActiv;
    const char *mainLabels[NUMMENU] = {"Play", "Exit"};
    GLuint textureBG[1];
    SDL_Color color[2] = {{255, 255, 255}, {255, 0, 48}};
    Mix_Music *music = NULL;

};

#endif

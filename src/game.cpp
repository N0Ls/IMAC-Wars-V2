#include "game.hpp"
#include "map.hpp"
#include "constants.hpp"
#include "texture.hpp"
#include <iostream>
#include <SDL/SDL_mixer.h>
#include "player.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "node.hpp"
#include <stack>
using namespace std;

static const unsigned int BIT_PER_PIXEL = 32;

Game::Game()
{
}

Game::~Game()
{
}

void Game::reshape(SDL_Surface** surface, unsigned int width, unsigned int height)
{

    static float aspectRatio;
    SDL_Surface* surface_temp = SDL_SetVideoMode(
        width, height, BIT_PER_PIXEL,
        SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE);
    if(NULL == surface_temp)
    {
        fprintf(
            stderr,
            "Erreur lors du redimensionnement de la fenetre.\n");
        exit(EXIT_FAILURE);
    }
    *surface = surface_temp;

    aspectRatio = width / (float) height;

    glViewport(0, 0, (*surface)->w, (*surface)->h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if( aspectRatio > 1)
    {
        gluOrtho2D(
        -GL_VIEW_SIZE / 2. * aspectRatio, GL_VIEW_SIZE / 2. * aspectRatio,
        -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.);
    }
    else
    {
        gluOrtho2D(
        -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.,
        -GL_VIEW_SIZE / 2. / aspectRatio, GL_VIEW_SIZE / 2. / aspectRatio);
    }
}

void Game::init(const char *title, int width, int height)
{
    /* Initialisation de la SDL */
    if (-1 == SDL_Init(SDL_INIT_VIDEO))
    {
        isRunning = false;
        fprintf(
            stderr,
            "Impossible d'initialiser la SDL. Fin du programme.\n");
        exit(EXIT_FAILURE);
    }

    /* Ouverture d'une fenetre et creation d'un contexte OpenGL */
    surface = SDL_SetVideoMode(
        width, height, BIT_PER_PIXEL,
        SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE);


    if (NULL == surface)
    {
        isRunning = false;
        fprintf(
            stderr,
            "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        isRunning = true;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    init_textures(this->nb_sub_groups_textures, this->nb_textures_map, this->nb_textures_units, this->textureIds_map,this->textureIds_units,this->textureLink);

    for (int i = 0; i < this->nb_players; i++) {
      initPlayer(this->players+i,i,this->nb_start_units);
    }

    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        exit(EXIT_FAILURE);
    }

    this->music = Mix_LoadMUS( "doc/test.wav" );
    this->click = Mix_LoadWAV( "doc/click.wav" );

    if( this->music == NULL || this->click == NULL)
    {
      fprintf(
          stderr,
          "Impossible de charger la musique.\n");
        exit(EXIT_FAILURE);
    }

    loadMap(this->tabMap);

    reshape(&surface, width, height);
}

void Game::placeUnits(){
  for (int i = 0; i < this->nb_players; i++) {
    for(int y=0;y <this->players[i].nbUnits; y++){
      setCoordinates(&this->players[i].units[y], rand()%9, rand()%9);
      updateDisplayCoordinates(&this->players[i].units[y]);
    }
  }
  this->unitPlaced = true;
}



void Game::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        /* Close window */
        if (e.type == SDL_QUIT)
        {
            isRunning = false;
            break;
        }

        /* Close window */
        if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_q || e.key.keysym.sym == SDLK_ESCAPE))
        {
            isRunning = false;
            break;
        }

        if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_m))
        {
          if( Mix_PlayingMusic() == 0 ){
             //On lance la musique
              Mix_PlayMusic( this->music, -1 );
              break;
          }
          else{
             //Si la musique est en pause
             if( Mix_PausedMusic() == 1 ){
                 //On enlève la pause (la musique repart où elle en était)
                 Mix_ResumeMusic();
                 break;
             }
             //Si la musique est en train de jouer
             else{
                 //On met en pause la musique
                 Mix_PauseMusic();
                 break;
             }
         }
        }

        switch (e.type)
        {
        /* Clic souris */
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(e.button.button ==SDL_BUTTON_LEFT){
              printf("clic en (%d, %d)\n", e.button.x, e.button.y);
              Mix_PlayChannel( -1, this->click, 0 );
              this->clickCheck(e.button.x, e.button.y);
            }
            break;

        /* Mouvement souris */
        case SDL_MOUSEMOTION:
            //printf("souris en (%d, %d)\n", e.button.x, e.button.y);
            break;

        /* Touche clavier */
        case SDL_KEYDOWN:
            printf("touche pressee (code = %d)\n", e.key.keysym.sym);
            break;
        case SDL_VIDEORESIZE:
            reshape(&surface, e.resize.w, e.resize.h);
            break;
        default:
            break;
        }
    }
}
void Game::draw(SDL_Surface *surface){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    fillGrid(this->textureIds_map,this->textureLink, this->tabMap);
    this->displaySelectdUnit();
    for (int g= 0; g < this->nb_players; g++) {
      for (int a = 0; a < this->players[g].nbUnits; a++) {
          displayUnit(this->players[g].units+a, this->textureIds_units);
      }
    }

    SDL_GL_SwapBuffers();
}
void Game::update()
{
    //std::cout << this->selected_unit << '\n';
    int k = 0;

    k++;


    //std::cout << "counter "<< std::endl;
}
void drawQuadsSelection(){
  glBegin(GL_QUADS);
    glColor4f(1,0,0,0.5);
      glTexCoord2f( 0 , 0);
      glVertex2f(0,0);

      glTexCoord2f( 1 , 0);
      glVertex2f(MAP_TILE_SIZE,0);

      glTexCoord2f( 1 , 1);
      glVertex2f(MAP_TILE_SIZE,MAP_TILE_SIZE);

      glTexCoord2f( 0 , 1);
      glVertex2f(0,MAP_TILE_SIZE);
  glEnd();
}
void Game::displaySelectdUnit(){
  if(this->selected_unit != NULL && this->move == false){
    glPushMatrix();
      glScalef(1,-1,1.);
      //Drawing red square on selected unit
      glPushMatrix();
        glTranslatef(this->selected_unit->displayX,this->selected_unit->displayY ,0);
        drawQuadsSelection();
      glPopMatrix();

      for(int y=0 ; y<=this->selected_unit->dexterity ; y++){
        for(int j=0; j<=this->selected_unit->dexterity-y ; j++){
          glPushMatrix();
            glTranslatef((-GL_VIEW_SIZE/2)+(this->selected_unit->x+y)*MAP_TILE_SIZE, (-GL_VIEW_SIZE/2)+(this->selected_unit->y+j)*MAP_TILE_SIZE,0);
            if(!(this->selected_unit->x+y >= MAP_SIZE || this->selected_unit->y+j >=MAP_SIZE)&&((this->tabMap[(this->selected_unit->x+y)*MAP_SIZE+ this->selected_unit->y+j]==1) || (this->tabMap[(this->selected_unit->x+y)*MAP_SIZE+ this->selected_unit->y+j]==2)))drawQuadsSelection();
          glPopMatrix();
          glPushMatrix();
            glTranslatef((-GL_VIEW_SIZE/2)+(this->selected_unit->x-y)*MAP_TILE_SIZE, (-GL_VIEW_SIZE/2)+(this->selected_unit->y-j)*MAP_TILE_SIZE,0);
            if(!(this->selected_unit->x-y < 0 || this->selected_unit->y-j <0)&&((this->tabMap[(this->selected_unit->x-y)*MAP_SIZE+ this->selected_unit->y-j]==1) || (this->tabMap[(this->selected_unit->x-y)*MAP_SIZE+ this->selected_unit->y-j]==2)))drawQuadsSelection();
          glPopMatrix();
          glPushMatrix();
            glTranslatef((-GL_VIEW_SIZE/2)+(this->selected_unit->x+y)*MAP_TILE_SIZE, (-GL_VIEW_SIZE/2)+(this->selected_unit->y-j)*MAP_TILE_SIZE,0);
            if(!(this->selected_unit->x+y >= MAP_SIZE || this->selected_unit->y-j <0) && ((this->tabMap[(this->selected_unit->x+y)*MAP_SIZE+ this->selected_unit->y-j]==1) || (this->tabMap[(this->selected_unit->x+y)*MAP_SIZE+ this->selected_unit->y-j]==2)))drawQuadsSelection();
          glPopMatrix();
          glPushMatrix();
            glTranslatef((-GL_VIEW_SIZE/2)+(this->selected_unit->x-y)*MAP_TILE_SIZE, (-GL_VIEW_SIZE/2)+(this->selected_unit->y+j)*MAP_TILE_SIZE,0);
            if(!(this->selected_unit->x-y <0 || this->selected_unit->y+j >=MAP_SIZE) && ((this->tabMap[(this->selected_unit->x-y)*MAP_SIZE+ this->selected_unit->y+j]==1) || (this->tabMap[(this->selected_unit->x-y)*MAP_SIZE+ this->selected_unit->y+j]==2)))drawQuadsSelection();
          glPopMatrix();
        }
      }
    glPopMatrix();
  }
}

bool Game::validClickMove(int x, int y){
  if((x>=0 && y>=0 && x<MAP_SIZE && y<MAP_SIZE) && (this->tabMap[(x)*MAP_SIZE+ y]==1 || this->tabMap[(x)*MAP_SIZE+ y]==2) && (abs (this->selected_unit->x - x) + abs (this->selected_unit->y - y) <= this->selected_unit->dexterity)){
    return 1;
  }
  else{
    return 0;
  }
}
void Game::clickCheck(float mouseX,float mouseY){

  float ratio = (float) this->surface->w/this->surface->h;

  float mouseXpos;
  float mouseYpos;

  int mouseTileX;
  int mouseTileY;

  if(ratio > 1){
    mouseXpos=((mouseX/this->surface->w)*ratio - 0.5 * ratio);
    mouseYpos=(mouseY/this->surface->h -0.5);
  }
  else{
    mouseXpos=(mouseX/this->surface->w -0.5);
    mouseYpos=((mouseY/this->surface->h)*1/ratio - 0.5 * 1/ratio);
  }
  float step = (float) 0.5/(MAP_SIZE/2);
  std::cout << mouseXpos/step << std::endl;


  if((mouseXpos/step < -(MAP_SIZE/2) && mouseXpos/step > -((MAP_SIZE/2)+1) )|| (mouseYpos/step < -(MAP_SIZE/2) && mouseYpos/step > -((MAP_SIZE/2)+1)) ){ //avoiding -0 symetry
    mouseTileX = MAP_SIZE/2 + mouseXpos/step - 1;
    mouseTileY = MAP_SIZE/2 + mouseYpos/step - 1;
  }else{
    mouseTileX = MAP_SIZE/2 + mouseXpos/step;
    mouseTileY = MAP_SIZE/2 + mouseYpos/step;
  }
  std::cout << "Tu as cliqué sur la case : " << mouseTileX << " ; " << mouseTileY << " : " << this->tabMap[mouseTileX*MAP_SIZE + mouseTileY]<<std::endl;
  if(mouseTileX >= 0 && mouseTileY >= 0 && mouseTileX < MAP_SIZE && mouseTileY < MAP_SIZE){
    this->lastClickX = mouseTileX;
    this->lastClickY = mouseTileY;

    for(int i=0; i < this->nb_players ; i++){
      for(int j = 0 ; j< players[i].nbUnits; j++){
        if(mouseTileX == players[i].units[j].x && mouseTileY == players[i].units[j].y){
          if(this->selected_unit!=NULL && this->selected_unit->x ==mouseTileX &&this->selected_unit->y == mouseTileY){
            std::cout << "unité désélectionnée" << '\n';
            this->selected_unit = NULL;
          }else{
            this->selected_unit = &players[i].units[j];
            std::cout << "unité cliquée"<< std::endl;
          }
        }
      }
    }
    if(this->selected_unit!=NULL && lastClickX != NULL && lastClickY!=NULL && !(lastClickX==this->selected_unit->x && lastClickY==this->selected_unit->y) && this->moving_unit==false && validClickMove(lastClickX,lastClickY)){
      this->move=true;
      if(this->move == true && this->moving_unit==false){
        stack <PathCoordinates> path;
        Node *pathNode;
        this->moving_unit = true;
        path = aStar(this->tabMap,this->selected_unit->x,this->selected_unit->y,this->lastClickX,this->lastClickY);
        // while (pathNode != NULL) {
        //   displayNode(*pathNode);
        //   //path.push(pathNode);
        //   pathNode=pathNode->parent;
        // }
        cout << endl<< endl;
        PathCoordinates print;
        while(!(path.empty())){
          print = path.top();
          cout << print.x << " " << print.y<< endl;
          deplacement(this->selected_unit , print.x, print.y);
          //displayNode(*path.top());
          path.pop();
        }
      }

    }
  }
}

void Game::changeState(bool state)
{
  isRunning = state;
}

void Game::clean()
{
  /* AJOUTER LE CLEAN DES TEXTURES*/
    Mix_FreeMusic(this->music);
    Mix_FreeChunk(this->click);
    SDL_Quit();
    glDisable(GL_TEXTURE_2D);
    exit(EXIT_SUCCESS);
}

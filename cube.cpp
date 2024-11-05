#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <vector>
#include <string.h>
#include <time.h>

const int screenWidth = 800;
const int screenHeight= 600;
void main_menu();
// Struct buat titik 3D
struct Vec3 {
    float x, y, z;
};

// Struct buat titik 2D
struct Vec2 {
    int x, y;
};

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;


// Proyeksi 2D
Vec2 projectPoint(const Vec3& point, int screenWidth, int screenHeight, float fov, float viewerDistance) {
    Vec2 projected;
    projected.x = (int)((point.x * fov) / (point.z + viewerDistance) + screenWidth / 2);
    projected.y = (int)((point.y * fov) / (point.z + viewerDistance) + screenHeight / 2);
    return projected;
}

// Buat rotasi
Vec3 rotateX(const Vec3 point,float angle){
  float cosAngle = cos(angle);
  float sinAngle = sin(angle);
  return{
    point.x,
    point.y * cosAngle - point.z * sinAngle,
    point.y * sinAngle + point.z * cosAngle
  };
}

Vec3 rotateY(const Vec3 point,float angle){
  float cosAngle = cos(angle);
  float sinAngle = sin(angle);
  return{
    cosAngle * point.x + sinAngle * point.z,
    point.y,
    -sinAngle * point.x + cosAngle * point.z
  };
}

Vec3 rotateZ(const Vec3 point, float angle){
  float cosAngle=cos(angle);
  float sinAngle=sin(angle);
  return{
    cosAngle * point.x - sinAngle * point.y,
    sinAngle * point.x + cosAngle * point.y,
    point.z
  };
}

// Bikin teks

SDL_Texture *create_text(TTF_Font *font,const char *text, SDL_Color color){
  SDL_Surface *surface = TTF_RenderText_Solid(font,text,color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer,surface);
  SDL_FreeSurface(surface);
  return texture;
}

float hitung_luas(float sisi){
  return sisi*sisi;
}

float hitung_volume(float sisi){
  return sisi*sisi*sisi;
}

int render(float sisi) {
    float fov = 1000; // Field of view atau yang disebut dengan area pandang
    const float viewerDistance = 100; // Jarak pandang!
    SDL_Color color={54,209,96};
    bool zoomin=false;
    bool zoomout=false;
                                      //
    float luas=hitung_luas(sisi*2.0);
    float volume=hitung_volume(sisi*2.0);

    sisi = sisi/2;
    char luas_caption[50];
    snprintf(luas_caption,sizeof(luas_caption),"Luas : %f",luas);
    SDL_Texture *luas_label = create_text(font,luas_caption,color);
    SDL_Rect luas_rect={
      .x=0,
      .y=screenHeight-60,
      .w=15*(int)strlen(luas_caption),
      .h=30
    };

    char volume_caption[50];
    snprintf(volume_caption,sizeof(volume_caption),"Volume : %f",volume);
    SDL_Texture *volume_label=create_text(font,volume_caption,color);
    SDL_Rect volume_rect={
      .x=0,
      .y=screenHeight-30,
      .w=15*(int)strlen(volume_caption),
      .h=30
    };

    int colors[] = {255,255,255};

    // Titik kubus
    std::vector<Vec3> cubeVertices = {
        {-sisi, -sisi, -sisi}, {sisi, -sisi, -sisi}, {sisi, sisi, -sisi}, {-sisi, sisi, -sisi}, // Depan
        {-sisi, -sisi, sisi}, {sisi, -sisi, sisi}, {sisi, sisi, sisi}, {-sisi, sisi, sisi} , {-sisi,sisi,sisi}      // Belakang
    };

    // Sisi kubus yang terhubung dengan titik
    int cubeEdges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Depan
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Belakang
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Koneksi sisisi
    };

    bool isRunning = true;
    SDL_Event event;

    double anglex=0.0,angley=0.0,anglez=0.0;
    while (isRunning) {

        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN){
              switch(event.key.keysym.scancode){
                case SDL_SCANCODE_UP:{
                  //anglex += 0.1;
                  //angley += 0.1;
                  anglez += 0.1;
                  break;
                }
                case SDL_SCANCODE_DOWN:
                  {
                    //anglex -= 0.1;
                    //angley -= 0.1;
                    anglez -= 0.1;

                    break;
                  }
                case SDL_SCANCODE_RIGHT:
                  {
                    angley += 0.1;
                    break;
                  }
                case SDL_SCANCODE_LEFT:
                  {
                    angley -= 0.1;
                    break;
                  }
                case SDL_SCANCODE_Z:
                  {
                    zoomin=true;
                    break;
                  }
                case SDL_SCANCODE_X:
                  {
                    zoomout=true;
                    break;
                  }
                case SDL_SCANCODE_C:
                  {
                    for(int i=0;i<3;i++){
                      colors[i] = rand()%256;
                    }
                    break;
                  }
                case SDL_SCANCODE_ESCAPE:
                  {
                    SDL_DestroyTexture(volume_label);
                    SDL_DestroyTexture(luas_label);
                    isRunning=false;
                    break;
                  }
                case SDL_SCANCODE_D:
                  {
                    for(int i=0;i<8;i++){
                      cubeVertices[i].x += 5;
                    }
                    break;
                  }
                case SDL_SCANCODE_A:
                  {
                    for(int i=0;i<8;i++){
                      cubeVertices[i].x -= 5;
                    }
                    break;
                  }
                default: break;
              }
            }
            else if(event.type == SDL_KEYUP){
              switch(event.key.keysym.scancode){
                case SDL_SCANCODE_Z: zoomin=false;
                case SDL_SCANCODE_X: zoomout=false;
                default: break;
              }
            }
        }

        // Clear layar
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Warna buat gambar
        SDL_SetRenderDrawColor(renderer, colors[0], colors[1], colors[2], 255);

        // Gambar sisi
        for (int i = 0; i < 12; ++i) {
            Vec3 p1rotated = rotateX(rotateY(rotateZ(cubeVertices[cubeEdges[i][0]],anglez),angley),anglex);
            Vec3 p2rotated = rotateX(rotateY(rotateZ(cubeVertices[cubeEdges[i][1]],anglez),angley),anglex);
            Vec2 p1 = projectPoint(p1rotated, screenWidth, screenHeight, fov, viewerDistance);
            Vec2 p2 = projectPoint(p2rotated, screenWidth, screenHeight, fov, viewerDistance);
            SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
            
        }

        SDL_RenderCopy(renderer,luas_label,NULL,&luas_rect);
        SDL_RenderCopy(renderer,volume_label,NULL,&volume_rect);
        //SDL_RenderClear(renderer);

        if (zoomin){
          fov += 5;
        }

        if (zoomout){
          fov -= 5;
        }
        SDL_RenderPresent(renderer);


        // Update
        SDL_Delay(1000/60);
    }

    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    main_menu();
    return 0;
}

void main_menu(){
  SDL_Event event;

  struct Text{
    SDL_Rect rect;
    SDL_Texture *texture;
    char caption[30];
  };

  SDL_Color color={66,237,112};

  struct Text text;


  struct Text label;
  label.texture = create_text(font,"Masukkan sisi : ",color);
  label.rect.w = 15 * strlen("Masukkan sisi : ");
  label.rect.h = 30;
  label.rect.x = 0;
  label.rect.y = 0;

  text.texture = create_text(font,"\0",color);
  text.rect.w = 15 * strlen("\0");
  text.rect.h = 30;
  text.rect.x= label.rect.w+15;
  text.rect.y=0;

  
  char buffer[30];
  int buffer_pos=0;
  int run=1;
  float sisi=0;
  while(run){
    while(SDL_PollEvent(&event)){
      if (event.type == SDL_QUIT){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(text.texture);
        SDL_DestroyTexture(label.texture);
        run=0;

        exit(0);
      }
      else if (event.type == SDL_TEXTINPUT){
        if (isprint(event.text.text[0])){
          buffer[buffer_pos]=event.text.text[0];
          buffer_pos++;
          SDL_DestroyTexture(text.texture);
          text.texture = create_text(font,buffer,color);
          text.rect.w = 15 * strlen(buffer);
        }
      }
      else if(event.type == SDL_KEYDOWN){
        switch(event.key.keysym.scancode){
          case SDL_SCANCODE_RETURN:
            {
                sisi=(float)atof(buffer);
                if (sisi > 50.0){
                  SDL_Log("Angka tidak boleh lebih dari 50\n");
                  break;
                }
                SDL_DestroyTexture(text.texture);
                SDL_DestroyTexture(label.texture);
                run=0;

                break;
            }
        }
      }
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,label.texture,NULL,&label.rect);
    SDL_RenderCopy(renderer,text.texture,NULL,&text.rect);
    SDL_RenderPresent(renderer);
    SDL_Delay(60);
  }
  SDL_Log("%lf",sisi);
  render(sisi/2.0f);
}

int main(){
  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO);
  SDL_StartTextInput();
  window = SDL_CreateWindow("Kubus 3D (^_^)!!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

  TTF_Init();
  font = TTF_OpenFont("font.ttf",28);

  main_menu();
  return 0;
}

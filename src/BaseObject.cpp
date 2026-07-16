#include <iostream>
#include "BaseObject.h"
#include "Profiler.h"

BaseObject::BaseObject()
{
    p_object_ = NULL;
    owns_texture_ = true;
    rect_.x = 0;
    rect_.y = 0;
    rect_.w = 0;
    rect_.h = 0;
}

BaseObject::~BaseObject()
{
    Free();
}
bool BaseObject::LoadImg(std::string path, SDL_Renderer *screen)
{
    Free();
    owns_texture_ = true;
    SDL_Texture *new_texture = NULL;

    Profiler::CountImageLoad();
    SDL_Surface *load_surface = IMG_Load(path.c_str());
    if (load_surface != NULL)
    {
        SDL_SetColorKey(load_surface, SDL_TRUE, SDL_MapRGB(load_surface->format, COLOR_KEY_R, COLOR_KEY_G, COLOR_KEY_B));
        Profiler::CountTextureCreate();
        new_texture = SDL_CreateTextureFromSurface(screen, load_surface);
        if (new_texture != NULL)
        {
            rect_.w = load_surface->w;
            rect_.h = load_surface->h;
        }
        SDL_FreeSurface(load_surface);
    }
    else
    {
        std::cout << "Unable to load" << path << "SDL Error: " << SDL_GetError() << "\n";
    }
    p_object_ = new_texture;
    return p_object_ != NULL;
}

void BaseObject::UseTexture(SDL_Texture *texture, int width, int height)
{
    if (p_object_ == texture && rect_.w == width && rect_.h == height && owns_texture_ == false)
    {
        return;
    }

    Free();
    p_object_ = texture;
    rect_.w = width;
    rect_.h = height;
    owns_texture_ = false;
}

bool BaseObject::IsVisibleInViewport(const SDL_Rect &viewport) const
{
    if (rect_.w <= 0 || rect_.h <= 0)
    {
        return false;
    }

    return SDL_HasIntersection(&rect_, &viewport) == SDL_TRUE;
}

void BaseObject::Render1(SDL_Renderer *des, const SDL_Rect *clip)
{   
    SDL_Rect renderquad = {rect_.x + minus, rect_.y, rect_.w, rect_.h};
    if(rect_.x<=-SCREEN_WIDTH)
    {
        rect_.x=0;
    }
    SDL_Rect renderquad2 = {rect_.x + SCREEN_WIDTH + minus,rect_.y, rect_.w, rect_.h};

    SDL_RenderCopy(des, p_object_, clip, &renderquad);
    SDL_RenderCopy(des, p_object_, clip, &renderquad2);
}

void BaseObject::Render(SDL_Renderer *des, const SDL_Rect *clip)
{   
    SDL_Rect renderquad = {rect_.x, rect_.y, rect_.w, rect_.h};
    SDL_RenderCopy(des, p_object_, clip, &renderquad);
}

void BaseObject::Free()
{
    if (p_object_ != NULL)
    {
        if (owns_texture_)
        {
            SDL_DestroyTexture(p_object_);
        }
        p_object_ = NULL;
        rect_.w = 0;
        rect_.h = 0;
    }
    owns_texture_ = true;
}


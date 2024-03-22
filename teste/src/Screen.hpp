#pragma once

#include "pch.hpp"
#include  "Config.hpp"

class Screen
{
    
    public: 
    
    Screen(const std::string &name)
    {
        this->name = name;
        this->width = 0;
        this->height = 0;
    }
    
    
    virtual ~Screen()
    {

    }
    virtual void Load()
    {


    }

    virtual void Close()
    {

    }

    virtual void Update(float dt)
    {
        (void)dt;
    }

    virtual void Render()
    {

    }

    void SetSize(int width, int height)
    {
        this->width = width;
        this->height = height;
    }
    std::string name;
    int width;
    int height;
   
};


class ScreenManger
{
    private:

    std::vector<Screen*> screens;
    Screen* currentScreen;
    int width;
    int height;
    
    public:
        static ScreenManger *instance;

    static ScreenManger* Instance()
    {
        return instance;
    }


    ScreenManger(int width, int height)
    {
        this->width = width;
        this->height = height;
        currentScreen = nullptr;
        ScreenManger::instance = this;  
    }

    ~ScreenManger()
    {
        for (int i = 0; i < (int)screens.size(); i++)
        {
            screens[i]->Close();
            delete screens[i];
        }
    }

    void AddScreen(Screen* screen)
    {
        screen->SetSize(width, height); 
        screens.push_back(screen);
    }


    void SetScreen(const std::string &name)
    {
        for (int i = 0; i < (int)screens.size(); i++)
        {
            if (screens[i]->name == name)
            {
                if (currentScreen != nullptr)
                {
                    currentScreen->Close();

                }
                currentScreen = screens[i];
                currentScreen->Load();
                return;
            }
        }
    }

    void Update(float dt)
    {
        if (currentScreen != nullptr)
        {
            currentScreen->Update(dt);
        }
    }

    void Render()
    {
        if (currentScreen != nullptr)
        {
            currentScreen->Render();
        }
    }

    void SetSize(int width, int height)
    {
        this->width = width;
        this->height = height;
        if (currentScreen != nullptr)
        {
            currentScreen->SetSize(width, height);
        }
    }

   
};
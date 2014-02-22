#ifndef GRAPHICSSYSTEM_H
#define GRAPHICSSYSTEM_H

#include "util/ThreadManager.h"
#include "gfx/Window.h"
#include "gfx/Camera.h"
#include "gfx/View.h"
#include <functional>

class GraphicsSystem {
public:
    GraphicsSystem(ThreadManager &tm);
    
    typedef std::function<void ()> InputCallback;
    void runRenderLoop(std::function<bool ()> input_callback);
    
    void pushView(std::unique_ptr<View> view);
    template <typename ViewT>
    ViewT &getView(unsigned int pos) {
        return *static_cast<ViewT *>(views[pos].get());
    }

    Window &getWindow() { return window; }
    
private:
    void renderFrame();
    void waitTimer();

    ThreadManager &tm;
    Window window;

    std::vector<std::unique_ptr<View>> views;
};

#endif

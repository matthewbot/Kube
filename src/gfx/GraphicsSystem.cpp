#include "gfx/GraphicsSystem.h"
#include <chrono>

#include <iostream>

GraphicsSystem::GraphicsSystem(ThreadManager &tm) :
    tm(tm),
    window(800, 600)
{ }

void GraphicsSystem::runRenderLoop(std::function<bool ()> input_callback) {
    using namespace std::chrono;
    const auto framerate = duration_cast<milliseconds>(duration<double>(1.0/60.0));
    
    while (tm.runMain(framerate)) {
	if (!input_callback()) {
	    break;
	}
	renderFrame();
    }
}

void GraphicsSystem::renderFrame() {
    window.clear();

    for (auto &viewptr : views) {
        viewptr->render(renderer, window);
    }

    window.swapBuffers();
}

void GraphicsSystem::pushView(std::unique_ptr<View> view) {
    views.push_back(std::move(view));
}

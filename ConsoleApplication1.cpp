// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define WIN32

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <chrono> 
#include <iostream> 

class MyGLWindow : public Fl_Gl_Window {
private:
    double rotationSpeed;
    bool spinning;
    double rotationAngle;
    std::chrono::high_resolution_clock::time_point startTime;  // For timing complete rotations
    std::chrono::high_resolution_clock::time_point lastUpdateTime;  // For timing between frames

public:
    MyGLWindow(int X, int Y, int W, int H, const char* L = nullptr)
        : Fl_Gl_Window(X, Y, W, H, L),
        rotationSpeed(0.5),
        spinning(true),
        rotationAngle(0.0) {
        mode(FL_RGB | FL_DOUBLE | FL_DEPTH);
        Fl::add_idle(RotateIdleCB, static_cast<void*>(this));

        // Initialize the start and last update times:
        startTime = std::chrono::high_resolution_clock::now();
        lastUpdateTime = startTime;
    }

    void setSpinning(bool s) {
        spinning = s;
        if (spinning) {
            lastUpdateTime = std::chrono::high_resolution_clock::now(); // Reset the time when restarting to avoid sudden jump when resume
            Fl::add_idle(RotateIdleCB, static_cast<void*>(this));
        }
        else {
            Fl::remove_idle(RotateIdleCB, static_cast<void*>(this));
        }
        redraw();
    }


    void setSpeed(double speed) {
        rotationSpeed = speed;
        redraw();
    }

    static void RotateIdleCB(void* data) {
        auto glWin = static_cast<MyGLWindow*>(data);

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeDelta = std::chrono::duration<double>(currentTime - glWin->lastUpdateTime).count();  // Time elapsed since last update in seconds

        glWin->rotationAngle += glWin->rotationSpeed * timeDelta * 360.0;  // Multiply by timeDelta to account for variable time between frames
        glWin->lastUpdateTime = currentTime;

        if (glWin->rotationAngle > 360.0) {
            glWin->rotationAngle -= 360.0;

            // Calculate elapsed time:
            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - glWin->startTime).count();

            std::cout << "Time taken for one complete rotation: " << elapsed / 1000.0 << " seconds." << std::endl;

            // Reset the start time for the next rotation:
            glWin->startTime = endTime;
        }

        if (glWin->spinning) {
            glWin->redraw();
        }
    }

    void draw() override {
        if (!valid()) {
            glEnable(GL_DEPTH_TEST);
            glLoadIdentity();
            glViewport(0, 0, w(), h());
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0, w() * 1.0 / h(), 1, 500);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            valid(1);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.f, 0.f, -5.f);
        glRotated(rotationAngle, 0.0, 1.0, 0.0);

        glBegin(GL_TRIANGLES);
        glVertex3f(0.f, 1.f, 0.f);
        glVertex3f(-1.f, -1.f, 0.f);
        glVertex3f(1.f, -1.f, 0.f);
        glEnd();
    }

    bool isSpinning() const {
        return spinning;
    }
};

void StartStopCB(Fl_Widget* w, void* data) {
    auto glWin = static_cast<MyGLWindow*>(data);
    if (glWin->isSpinning()) {
        glWin->setSpinning(false);
        static_cast<Fl_Button*>(w)->label("Start");
    }
    else {
        glWin->setSpinning(true);
        static_cast<Fl_Button*>(w)->label("Stop");
    }
}

void AdjustSpeedCB(Fl_Widget* w, void* data) {
    auto glWin = static_cast<MyGLWindow*>(data);
    double speed = static_cast<Fl_Value_Slider*>(w)->value();
    glWin->setSpeed(speed);
}

int main(int argc, char** argv) {
    Fl_Window win(850, 650, "Spinning Triangle with FLTK");

    MyGLWindow glwin(10, 10, 830, 530);
    win.resizable(&glwin);

    Fl_Button startStopButton(10, 550, 120, 30, "Stop");
    startStopButton.callback(StartStopCB, static_cast<void*>(&glwin));

    Fl_Value_Slider speedSlider(150, 550, 690, 30, "Speed");
    speedSlider.type(FL_HORIZONTAL);
    speedSlider.range(0.1, 5.0);
    speedSlider.value(0.5);
    speedSlider.callback(AdjustSpeedCB, static_cast<void*>(&glwin));

    win.end();
    win.show(argc, argv);
    return Fl::run();
}









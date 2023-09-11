// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32


#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <vector>


class DrawingCanvas : public Fl_Box {
private:
    int prev_x, prev_y;
    bool is_drawing;

    struct LineSegment {
        int x1, y1, x2, y2;
        LineSegment(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
    };

    std::vector<LineSegment> lines;

public:
    DrawingCanvas(int x, int y, int w, int h)
        : Fl_Box(x, y, w, h), prev_x(0), prev_y(0), is_drawing(false) {
        box(FL_FLAT_BOX);
        color(FL_WHITE);
    }

    void draw() {
        // First draw the box (our canvas)
        Fl_Box::draw();
        // Then draw all the line segments
        fl_color(FL_BLACK);
        for (const LineSegment& line : lines) {
            fl_line(line.x1, line.y1, line.x2, line.y2);
        }
    }

    int handle(int event) {
        int cur_x = Fl::event_x();
        int cur_y = Fl::event_y();
        switch (event) {
        case FL_PUSH:
            prev_x = cur_x;
            prev_y = cur_y;
            is_drawing = true;
            return 1;
        case FL_DRAG:
            if (is_drawing) {
                lines.push_back(LineSegment(prev_x, prev_y, cur_x, cur_y));
                prev_x = cur_x;
                prev_y = cur_y;
                redraw();
            }
            return 1;
        case FL_RELEASE:
            is_drawing = false;
            return 1;
        }
        return Fl_Box::handle(event);
    }
};

int main() {
    Fl_Double_Window win(800, 600, "Simple Drawing App with FLTK");
    DrawingCanvas canvas(10, 10, 780, 580);
    win.resizable(canvas);
    win.end();
    win.show();
    return Fl::run();
}
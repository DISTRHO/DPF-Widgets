
#include "Application.hpp"
#include "StandaloneWindow.hpp"

#include "../../generic/ResizeHandle.hpp"
#include "../../opengl/LVGL.hpp"

#include "demos/lv_demos.h"

class LVGLDemo : public LVGLWidget
{
public:
    LVGLDemo(Window& window)
        : LVGLWidget(window) {}

    // delay setup after window size has been set
    void setup()
    {
        lv_demo_widgets();
    }
};

class DemoWindow : public StandaloneWindow
{
    static const uint kMarginBase    = 8;
    static const uint kMarginContent = kMarginBase + 2;

    static const uint kMinWindowWidth  = 1000;
    static const uint kMinWindowHeight = 600;

    LVGLDemo lvgl;
    ResizeHandle resizeHandle;

    bool widgetsDone = false;

public:
    DemoWindow(Application& app)
        : StandaloneWindow(app),
          lvgl((Window&)*this),
          resizeHandle(this)
    {
        const double scaleFactor = getScaleFactor();
        const uint margin = kMarginContent * scaleFactor;

        setGeometryConstraints(kMinWindowWidth * scaleFactor, kMinWindowHeight * scaleFactor, false, false);
        setSize(kMinWindowWidth * scaleFactor, kMinWindowHeight * scaleFactor);
        setResizable(true);
        setTitle("LVGL Widgets Demo");

        lvgl.setup();
    }

protected:
    void onDisplay() override
    {
        const GraphicsContext& context(getGraphicsContext());
        const double scaleFactor = getScaleFactor();
        const uint margin = kMarginBase * scaleFactor;
        const uint width  = getWidth();
        const uint height = getHeight();

        Color::fromHTML("#5680c2").setFor(context);
        Rectangle<uint>(0, 0, width, margin).draw(context);
        Rectangle<uint>(0, 0, margin, height).draw(context);
        Rectangle<uint>(0, height-margin, width, height).draw(context);
        Rectangle<uint>(width-margin, 0, width, height).draw(context);

        Color::fromHTML("#727272").setFor(context);
        Rectangle<uint>(margin, margin, width-margin*2, height-margin*2).draw(context);
    }
};

int main()
{
    Application app;
    DemoWindow win(app);
    win.show();
    app.exec();
    return 0;
}

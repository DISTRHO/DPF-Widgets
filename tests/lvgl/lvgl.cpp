
#include "Application.hpp"
#include "StandaloneWindow.hpp"

#include "../../generic/ResizeHandle.hpp"
#include "../../opengl/LVGL.hpp"

class DemoWindow : public StandaloneWindow
{
    static const uint kMarginBase    = 8;
    static const uint kMarginContent = kMarginBase + 2;

    static const uint kMinWindowWidth  = 1000;
    static const uint kMinWindowHeight = 600;

    // ResizeHandle resizeHandle;
    LVGLWidget lvgl;

public:
    DemoWindow(Application& app)
        : StandaloneWindow(app),
        //   resizeHandle(this),
          lvgl(this)
    {
        const double scaleFactor = getScaleFactor();
        const uint margin = kMarginContent * scaleFactor;

        setGeometryConstraints(kMinWindowWidth * scaleFactor, kMinWindowHeight * scaleFactor, false, false);
        setSize(kMinWindowWidth * scaleFactor, kMinWindowHeight * scaleFactor);
        setResizable(true);
        setTitle("LVGL Widgets Demo");
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

/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2022 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// Elements is quite large, build it separately
#define ELEMENTS_SKIP_IMPLEMENTATION

#include "../cairo/Elements.cpp"

START_NAMESPACE_DGL

std::string const text1 =
   "一千条路…\n"
   "仁、义、礼、智、信…\n"
   "开放包容、视野宽广\n\n"
;

std::string const text2 =
   "To traverse the quest is to become one with it.\n\n"

   "You and I are adventurers of the quantum cycle. The goal of expanding wave "
   "functions is to plant the seeds of non-locality rather than pain. "
   "The complexity of the present time seems to demand a redefining of our "
   "bodies if we are going to survive. "
   "We are at a crossroads of will and greed. Humankind has nothing to lose. "
   "Our conversations with other storytellers have led to an evolving of "
   "hyper-sentient consciousness. "
   "If you have never experienced this flow on a cosmic scale, it can be "
   "difficult to self-actualize. Although you may not realize it, you are "
   "ancient. Have you found your vision quest?\n\n"

   "Imagine a deepening of what could be. We are being called to explore the "
   "galaxy itself as an interface between nature and transformation. This "
   "circuit never ends. Entity, look within and recreate yourself. "
   "Eons from now, we warriors will exist like never before as we are reborn "
   "by the universe. We must change ourselves and empower others. The "
   "wellspring of sharing is now happening worldwide. "
   "You will soon be awakened by a power deep within yourself - a power "
   "that is ethereal, dynamic. Astral projection may be the solution to "
   "what's holding you back from an ecstatic oasis of divinity. As you "
   "reflect, you will enter into infinite freedom that transcends understanding.\n\n"
;

std::string const text3 =
   "We are in the midst of an intergalatic condensing of beauty that will "
   "clear a path toward the planet itself. The quantum leap of rebirth is "
   "now happening worldwide. It is time to take healing to the next level. "
   "Soon there will be a deepening of chi the likes of which the infinite "
   "has never seen. The universe is approaching a tipping point. This "
   "vision quest never ends. Imagine a condensing of what could be. "
   "We can no longer afford to live with stagnation. Suffering is born "
   "in the gap where stardust has been excluded. You must take a stand "
   "against discontinuity.\n\n"

   "Without complexity, one cannot dream. Stagnation is the antithesis of "
   "life-force. Only a seeker of the galaxy may engender this wellspring of hope."
   "Yes, it is possible to eliminate the things that can destroy us, but not "
   "without wellbeing on our side. Where there is delusion, faith cannot thrive. "
   "You may be ruled by desire without realizing it. Do not let it eliminate "
   "the growth of your journey.\n\n"
;

class ElementsDemo : public ElementsStandaloneWindow
{
public:
    ElementsDemo(Application& app)
        : ElementsStandaloneWindow(app)
    {
        using namespace cycfi::elements;

        // set the font of text box to 文泉驿 open source font: http://wenq.org/wqy2/index.cgi
        char const* font_family = "文泉驿微米黑, \"WenQuanYi Micro Hei\"";
        auto text = text1+text2+text3;

        content(
            button("Momentary Button"),
            scroller(
                margin(
                    { 20, 20, 20, 20 },
                    align_left_top(hsize(800,
                    basic_text_box(text, font_descr{ font_family }
                    )))
                )
            ),
            port(image{ "/Users/falktx/Source/DISTRHO/DPF-Widgets/cairo/elements-git-src/examples/text_edit/resources/dark-bkd.jpg" })
        );
    }
};

END_NAMESPACE_DGL

int main(int, char**)
{
    USE_NAMESPACE_DGL;

    Application app;
    ElementsDemo win(app);
    win.setSize(400*win.getScaleFactor(), 400*win.getScaleFactor());
    win.setResizable(true);
    win.setTitle("Elements");
    win.show();
    app.exec();

    return 0;
}

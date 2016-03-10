#include "common.h"

using namespace details;

go_bandit([] {

    describe("draw::Font:", [] {

        static auto kFontFilePath = "cour.ttf";
        static auto kFontLetterSize = 12;

        using namespace draw;

        RendererPtr renderer;

        before_each([&] {

            mockGL();
            renderer = makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
        });

        it("should be created", [&] {

            auto ptr = renderer->makeFont(kFontFilePath, kFontLetterSize);
            
            AssertThat(ptr, Is().Not().EqualTo(FontPtr()));
            AssertThat(ptr->filePath(), Is().EqualTo(kFontFilePath));
            AssertThat(ptr->letterSize(), Is().EqualTo(kFontLetterSize));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });

        it("should throw InvalidArgument if filePath is invalid", [&] {

            auto ptr = renderer->makeFont(nullptr, kFontLetterSize);

            AssertThat(ptr, Is().EqualTo(FontPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if letterSize is less than Font::kMinLetterSize", [&] {

            auto ptr = renderer->makeFont(kFontFilePath, Font::kMinLetterSize - 1);

            AssertThat(ptr, Is().EqualTo(FontPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidFontFile if font file is not exist or corrupted", [&] {

            auto ptr = renderer->makeFont("none.ttf", kFontLetterSize);

            AssertThat(ptr, Is().EqualTo(FontPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidFontFile));
        });

        /*it("should throw IncompleteFontFile if some alphabet's glyphs is not contained", [&] {

            auto ptr = renderer->makeFont("absent_glyph.ttf", kFontLetterSize);

            AssertThat(ptr, Is().EqualTo(FontPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::IncompleteFontFile));
        });*/
    });
});
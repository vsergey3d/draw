#pragma once
#include <draw.h>

namespace draw {

using Code = Error::Code;

#ifndef DRAW_NO_EXCEPTIONS

class ErrorImpl final : public Error {

public:
    ErrorImpl(Code code) :
        code_(code) {
        ASSERT(code != Code::NoError);
    }
    virtual ~ErrorImpl() = default;

    virtual Code code() const final { return code_; } //noexcept

    virtual const char* what() const NOEXCEPT final {

        switch (code_) {
        case Code::OpenGLAbsentFeature: return "opengl absent feature";
        case Code::OpenGLOutOfMemory: return "opengl out of memory";
        case Code::InvalidArgument: return "invalid argument";
        case Code::InvalidFontFile: return "invalid font file";
        case Code::AbsentFontGlyph: return "absent font glyph";
        default:
            ASSERT(!"one of the error code cases is not handled");
        }
        return "";
    } //noexcept

private:
    Code code_;
};

inline bool error(Code code) {

    ASSERT(code != Code::NoError);
    throw ErrorImpl(code);
    //return false;
}

#else

static Code gLastError = Code::NoError;
Code getLastError() { return gLastError; }

inline bool error(Code code) {

    ASSERT(code != Code::NoError);
    gLastError = code;
    return false;
}

#endif

} // namespace draw

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

    virtual Code code() const final { return code_; }

    virtual const char* what() const NOEXCEPT final {

        switch (code_) {
        case Code::NoError: return "";
        case Code::OpenGLAbsentFeature: return "opengl absent feature";
        case Code::OutOfMemory: return "out of memory";
        case Code::InvalidArgument: return "invalid argument";
        case Code::InvalidFontFile: return "invalid font file";
        case Code::AbsentFontGlyph: return "absent font glyph";
        }
        return "";
    }

private:
    Code code_;
};

inline void setError(Error::Code code) {

    ASSERT(code != Code::NoError);
    throw ErrorImpl(code);
}

#define TRY try
#define CATCH catch(...)

#else

static Code gLastError = Code::NoError;
static Code getLastError() { return gLastError; }

inline void setError(Error::Code code) {

    ASSERT(code != Code::NoError);
    gLastError = code;
}

#define TRY
#define CATCH if (false)

#endif

} // namespace draw

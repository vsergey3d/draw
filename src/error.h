#pragma once
#include <draw.h>
#include <common.h>

namespace draw {

#ifndef DRAW_NO_EXCEPTIONS

class ErrorImpl final : public Error {

public:
    ErrorImpl(ErrorCode code) :
        code_(code) {
        ASSERT(code != NoError);
    }
    virtual ~ErrorImpl() = default;

    virtual ErrorCode code() const final { return code_; }

    virtual const char* what() const NOEXCEPT final {

        switch (code_) {
        case NoError: return "";
        case OpenGLAbsentFeature: return "opengl absent feature";
        case OpenGLOutOfMemory: return "opengl out of memory";
        case InvalidArgument: return "invalid argument";
        case InvalidFontFile: return "invalid font file";
        case IncompleteFontFile: return "incomplete font file";
        }
        return "";
    }
private:
    ErrorCode code_;
};

inline void setError(ErrorCode code) {

    ASSERT(code != NoError);
    throw ErrorImpl(code);
}

#else

static ErrorCode gLastError = NoError;
inline ErrorCode getLastError() { return gLastError; }

inline void setError(ErrorCode code) {

    ASSERT(code != NoError);
    gLastError = code;
}

#endif

} // namespace draw

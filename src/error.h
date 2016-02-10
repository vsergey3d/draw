#pragma once
#include <draw.h>

namespace draw {

#ifndef DRAW_NO_EXCEPTIONS

#if defined(__clang__)
#if __has_feature(cxx_noexcept)
#define HAS_NOEXCEPT
#endif
#elif !defined(__clang__) && \
    defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46 || \
    defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190023026
#define HAS_NOEXCEPT
#endif
#ifdef HAS_NOEXCEPT
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

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
        case AbsentFontGlyph: return "absent font glyph";
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

#pragma once
#include <draw.h>

namespace draw {

class ErrorImpl final : public Error {

public:
    ErrorImpl(Code code) : code_(code) {} //noexcept
    ErrorImpl(Code code, const char* desc) : code_(code), desc_(desc) {} //noexcept
    virtual ~ErrorImpl() = default;

    virtual const char* desc() const final { return desc_.c_str(); } //noexcept
    virtual Code code() const final { return code_; } //noexcept

    virtual const char* what() const noexcept final { return desc_.c_str(); } //noexcept

private:
    Code code_;
    std::string desc_;
};

using Code = Error::Code;

inline ErrorImpl error(Code code) {

    return ErrorImpl(code);
}

inline ErrorImpl error(Code code, const char* desc) {

    return ErrorImpl(code, desc);
}

inline ErrorImpl error(Code code, const std::string& desc) {

    return ErrorImpl(code, desc.c_str());
}

} // namespace draw

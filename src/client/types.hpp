#pragma once

#include <memory>

namespace c2m { namespace client {

struct NullCallback {};

template<class CallbackArgT, class CallbackClassT = NullCallback>
class FunctionPointer {
public:
    // Non-member callback
    FunctionPointer(void (*callback)(CallbackArgT)) : target(nullptr), memberFP(nullptr), fp(callback) {};

    // Member callback
    FunctionPointer(void (CallbackClassT::* callback)(CallbackArgT), std::shared_ptr<CallbackClassT> target) : target(target), memberFP(callback), fp(nullptr) {};

    void sendCallback(const CallbackArgT& callback) {
        if (target != nullptr) {
            (target.get()->*memberFP)(callback);
        }
        else {
            (*fp)(callback);
        }
    };

private:
    std::shared_ptr<CallbackClassT> target;
    void (CallbackClassT::* memberFP)(CallbackArgT);
    void (*fp)(CallbackArgT);
};

}}
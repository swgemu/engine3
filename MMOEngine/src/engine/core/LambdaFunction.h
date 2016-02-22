//
// Created by Victor Popovici on 22/2/16.
//

#ifndef ENGINE3_LAMBDAFUNCTION_H
#define ENGINE3_LAMBDAFUNCTION_H

#include <functional>

#include "Task.h"

namespace engine {
    namespace core {
        template<class L>
        class LambdaFunction {
        protected:
            L lambda;
            const char *name = nullptr;

        public:
            template<class F>
            LambdaFunction(F&& f, const char* funcName) : lambda(std::forward<F>(f)),
                                                          name(funcName) {
            }

            LambdaFunction() = delete;

            virtual ~LambdaFunction() {
            }

            inline const char *getName() {
                return name;
            }

            inline L& getLambda() {
                return lambda;
            }
        };

        class LambdaTask : public Task {
            LambdaFunction<std::function<void()>> function;

        public:
            template<class L>
            LambdaTask(L&& l, const char* name) : function(std::forward<L>(l), name) {

            }

            void run() {
                auto& lambda = function.getLambda();

                lambda();
            }

            const char* getTaskName() {
                return function.getName();
            }
        };
    }
}

using namespace engine::core;

#endif //ENGINE3_LAMBDAFUNCTION_H

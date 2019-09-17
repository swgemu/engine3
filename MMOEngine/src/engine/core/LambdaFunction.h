/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 22/2/16.
//

#ifndef ENGINE3_LAMBDAFUNCTION_H
#define ENGINE3_LAMBDAFUNCTION_H

#include "system/lang/Function.h"

#include "Task.h"

namespace engine {
    namespace core {
        template<class L>
        class LambdaFunction {
        protected:
            L lambda;

        public:
            template<class F>
            LambdaFunction(F&& f) : lambda(std::forward<F>(f)) {
            }

            LambdaFunction() = delete;

            inline L& getLambda() {
                return lambda;
            }
        };

        class LambdaTask : public Task {
            LambdaFunction<Function<void()>> function;

        public:
            template<class L>
            LambdaTask(L&& l, const char* name) : function(std::forward<L>(l)) {
		setTaskName(name);
            }

            void run() final {
                auto& lambda = function.getLambda();

                lambda();
            }
        };
    }
}

using namespace engine::core;

#endif //ENGINE3_LAMBDAFUNCTION_H

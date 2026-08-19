#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace base {
        template <class T>
        class SingletonNode {
        private:
            inline static T* s_inst = nullptr;

        public:
            static T* get() noexcept { return s_inst; };
            static void set(T* ptr) noexcept { s_inst = ptr; };
        };

        template <class T, class... Args>
        class Node : public virtual cocos2d::CCNode {
        protected:
            Node() = default;
            virtual ~Node() = default;

            virtual bool init(Args... args) = 0;

            void onExit() override {
                if constexpr (std::is_base_of_v<SingletonNode<T>, T>) SingletonNode<T>::s_inst = nullptr;
                CCNode::onExit();
            };

        public:
            static T* create(Args... args) {
                auto ret = new T();
                if (ret->init(std::forward<Args>(args)...)) {
                    ret->autorelease();

                    if constexpr (std::is_base_of_v<SingletonNode<T>, T>) SingletonNode<T>::set(ret);

                    return ret;
                };

                delete ret;
                return nullptr;
            };
        };

        template <class T, class... Args>
        class PopupNode : public virtual geode::Popup {
        protected:
            PopupNode() = default;
            virtual ~PopupNode() = default;

            virtual bool init(Args... args) = 0;

            void onExit() {
                if constexpr (std::is_base_of_v<SingletonNode<T>, T>) SingletonNode<T>::s_inst = nullptr;
                CCNode::onExit();
            };

        public:
            static T* create(Args... args) {
                auto ret = new T();
                if (ret->init(std::forward<Args>(args)...)) {
                    ret->autorelease();

                    if constexpr (std::is_base_of_v<SingletonNode<T>, T>) SingletonNode<T>::set(ret);

                    return ret;
                };

                delete ret;
                return nullptr;
            };
        };
    };
};
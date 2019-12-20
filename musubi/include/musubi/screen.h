//
// Created by stuhlmeier on 12/19/19.
//

#ifndef MUSUBI_SCREEN_H
#define MUSUBI_SCREEN_H

namespace musubi {
    class window;

    class screen {
    public:
        virtual ~screen();

        virtual void on_attached(window *window) = 0;

        virtual void on_update(float dt) = 0;

        virtual void on_detached(window *window) = 0;
    };

    class basic_screen : public screen {
    public:
        ~basic_screen() override;

        void on_attached(window *window) override;

        void on_update(float dt) override;

        void on_detached(window *window) override;

    protected:
        [[nodiscard]] window *get_current_window() const;

    private:
        window *currentWindow;
    };
}

#endif //MUSUBI_SCREEN_H

#pragma once

namespace obj
{
    class App {
          
      private:

      public:
        App() {};
        ~App() {};

        App(const App &) = delete;
        App &operator=(const App &) = delete;

        void run();
    };
}
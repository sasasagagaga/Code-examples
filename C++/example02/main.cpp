#include <thread>

template<class F, class G, class... Params>
void work(F &&f, G &&g, Params &&...params)
{
    g(f(std::forward<Params>(params)...));
}

template<class F, class G, class... Params>
void async_launch(F &&f, G &&g, Params &&...params)
{
    std::thread t(work<F, G, Params...>, std::forward<F>(f), std::forward<G>(g), std::forward<Params>(params)...);
    t.detach();
}
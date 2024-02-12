#ifndef DOUBLE_BUFFER_ATOMIC_HPP_T4PFY34R
#define DOUBLE_BUFFER_ATOMIC_HPP_T4PFY34R

#include <array>
#include <atomic>
#include <cstdint>

template <typename Buffer>
class DoubleBufferAtomic
{
public:
    explicit DoubleBufferAtomic(Buffer startState = {})
        : m_buffers{ startState, startState }
    {
        m_index = { .m_front = BufferIndex::First, .m_back = BufferIndex::Second };
    }

    const Buffer& swapBuffers()
    {
        if (m_info != BufferUpdateStatus::Done) {
            return getFront();
        }

        auto            index   = m_index.load();
        BufferIndexPair swapped = { .m_front = index.m_back, .m_back = index.m_front };
        m_index                 = swapped;

        m_info = BufferUpdateStatus::Idle;
        return getBuffer(swapped.m_front);
    }

    void updateBuffer(std::invocable<Buffer&> auto&& update)
    {
        if (m_info != BufferUpdateStatus::Idle) {
            return;
        }
        m_info = BufferUpdateStatus::Updating;

        update(getBuffer(m_index.load().m_back));

        m_info = BufferUpdateStatus::Done;
    }

    const Buffer& getFront() const { return getBuffer(m_index.load().m_front); }

private:
    enum class BufferIndex : std::uint8_t
    {
        First  = 0,
        Second = 1,
    };

    enum class BufferUpdateStatus : std::uint16_t
    {
        Idle     = 0,
        Updating = 1,
        Done     = 2,
    };

    struct BufferIndexPair
    {
        BufferIndex m_front = BufferIndex::First;
        BufferIndex m_back  = BufferIndex::Second;
    };

    static std::size_t toSize(BufferIndex index) { return static_cast<std::size_t>(index); }

    const Buffer& getBuffer(BufferIndex index) const { return m_buffers[static_cast<std::size_t>(index)]; }
    Buffer&       getBuffer(BufferIndex index) { return m_buffers[static_cast<std::size_t>(index)]; }

    std::atomic<BufferUpdateStatus> m_info = BufferUpdateStatus::Idle;
    std::atomic<BufferIndexPair>    m_index;
    std::array<Buffer, 2>           m_buffers{};
};

#endif /* end of include guard: DOUBLE_BUFFER_ATOMIC_HPP_T4PFY34R */

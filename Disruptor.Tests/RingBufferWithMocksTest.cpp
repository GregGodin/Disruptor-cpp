#include "stdafx.h"

#include "Disruptor/RingBuffer.h"
#include "SequencerMock.h"
#include "StubEvent.h"

namespace Disruptor
{
namespace Tests
{

    struct RingBufferWithMocksFixture : public ::testing::Test
    {
        RingBufferWithMocksFixture()
        {
            m_sequencerMock = std::make_shared< testing::NiceMock< SequencerMock< StubEvent > > >();
            m_sequencer = m_sequencerMock;

            ON_CALL(*m_sequencerMock, bufferSize()).WillByDefault(testing::Return(16));
            m_ringBuffer = std::make_shared< RingBuffer< StubEvent > >(StubEvent::eventFactory(), m_sequencer);
        }
        
        std::shared_ptr< RingBuffer< StubEvent > > m_ringBuffer;
        std::shared_ptr< ISequencer< StubEvent > > m_sequencer;
        std::shared_ptr< SequencerMock< StubEvent > > m_sequencerMock;
    };

} // namespace Tests
} // namespace Disruptor


using namespace Disruptor;
using namespace Disruptor::Tests;

TEST_F(RingBufferWithMocksFixture, ShouldDelgateNextAndPublish)
{
    EXPECT_CALL(*m_sequencerMock, next()).WillOnce(testing::Return(34L));
    EXPECT_CALL(*m_sequencerMock, publish(34L)).Times(1);

    m_ringBuffer->publish(m_ringBuffer->next());
}

TEST_F(RingBufferWithMocksFixture, ShouldDelgateTryNextAndPublish)
{
    EXPECT_CALL(*m_sequencerMock, tryNext()).WillOnce(testing::Return(34L));
    EXPECT_CALL(*m_sequencerMock, publish(34L)).Times(1);

    m_ringBuffer->publish(m_ringBuffer->tryNext());
}

TEST_F(RingBufferWithMocksFixture, ShouldDelgateNextNAndPublish)
{
    EXPECT_CALL(*m_sequencerMock, next(10)).WillOnce(testing::Return(34L));
    EXPECT_CALL(*m_sequencerMock, publish(25L, 34L)).Times(1);

    auto hi = m_ringBuffer->next(10);
    m_ringBuffer->publish(hi - 9, hi);
}

TEST_F(RingBufferWithMocksFixture, ShouldDelgateTryNextNAndPublish)
{
    EXPECT_CALL(*m_sequencerMock, tryNext(10)).WillOnce(testing::Return(34L));
    EXPECT_CALL(*m_sequencerMock, publish(25L, 34L)).Times(1);

    auto hi = m_ringBuffer->tryNext(10);
    m_ringBuffer->publish(hi - 9, hi);
}

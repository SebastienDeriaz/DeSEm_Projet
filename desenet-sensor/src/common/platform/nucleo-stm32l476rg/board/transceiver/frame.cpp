#include <assert.h>
#include <new>
#include "pool/ObjectPool"
#include "frame.h"

static const size_t MaximalFramePayloadSize = 32;	// The maximal frame payload for the PHY.

namespace phy {

// Hidden data.
class Frame::Private
{
public:
	FrameAddress address;							// The address of the frame.
	uint8_t payload[MaximalFramePayloadSize];		// The payload.
	size_t size;									// The actual size of the frame.

	static uint8_t dummyByte;						// Dummy byte used to access on emergency.

	static ObjectPool<Private> * framePool;			// The frame pool.
};
uint8_t Frame::Private::dummyByte;
ObjectPool<Frame::Private> * Frame::Private::framePool;


Frame::Frame() : _pImpl( NULL )
{
	// Fail if the frame pool has not been initialized.
	assert( Private::framePool );
	if ( Private::framePool )
	{
		// Allocate a new frame.
		_pImpl = Private::framePool->allocate();

		// Fail if we could not create a new frame.
		assert( _pImpl );
		if ( _pImpl ) _pImpl->size = 0;
	}
}

Frame::Frame( const Frame::FrameAddress & address , size_t size ) : _pImpl( NULL )
{
	// Fail if the frame pool has not been initialized.
	assert( Private::framePool );
	if ( Private::framePool )
	{
		// Allocate a new frame.
		_pImpl = Private::framePool->allocate();

		// Fail if we could not create a new frame.
		assert( _pImpl );
		if ( _pImpl )
		{
			// Setup address and size.
			_pImpl->address = address;
			_pImpl->size = std::min( size , MaximalFramePayloadSize );
		}
	}
}

Frame::Frame( const Frame::FrameAddress & address , const void * const payload , size_t size  ) : _pImpl( NULL )
{
	// Fail if the frame pool has not been initialized.
	assert( Private::framePool );
	if ( Private::framePool )
	{
		// Allocate a new frame.
		_pImpl = Private::framePool->allocate();

		// Fail if we could not create a new frame.
		assert( _pImpl );
		if ( _pImpl )
		{
			// Setup address and size and copy payload from the memory region.
			_pImpl->address = address;
			_pImpl->size = std::min( size , MaximalFramePayloadSize );

			if ( payload && size ) memcpy( _pImpl->payload , payload , _pImpl->size );
		}
	}
}

Frame::Frame( const Frame::FrameAddress & address , const std::vector<uint8_t> & payload ) : _pImpl( NULL )
{
	// Fail if the frame pool has not been initialized.
	assert( Private::framePool );
	if ( Private::framePool )
	{
		// Allocate a new frame.
		_pImpl = Private::framePool->allocate();

		// Fail if we could not create a new frame.
		assert( _pImpl );
		if ( _pImpl )
		{
			// Setup address and size and copy payload from the vector.
			_pImpl->address = address;
			_pImpl->size = std::min( payload.size() , MaximalFramePayloadSize );
			if ( payload.size() ) memcpy( _pImpl->payload , payload.data() , _pImpl->size );
		}
	}
}

Frame::~Frame()
{
	// Release the frame.
	if ( Private::framePool )
		Private::framePool->release( _pImpl );
}

Frame::Frame( const Frame & other ) : _pImpl( NULL )
{
	// Release current frame.
	if ( _pImpl )
		Private::framePool->release( _pImpl );

	// Copy the pointer to the other frame.
	_pImpl = other._pImpl;

	// Retain (increment reference count) for the given frame.
	if ( _pImpl )
		Private::framePool->retain( _pImpl );
}

const Frame & Frame::operator=( const Frame & other )
{
	// I will not copy myself into myself!
	if ( this != &other )
	{
		// Release current frame.
		if ( _pImpl )
			Private::framePool->release( _pImpl );

		// Copy the pointer to the other frame.
		_pImpl = other._pImpl;

		// Retain (increment reference count) for the given frame.
		if ( _pImpl )
			Private::framePool->retain( _pImpl );
	}

	return *this;
}

Frame Frame::deepCopy()
{
	// Create a new frame that will be the deep copy of me.
	Frame copy;

	// Was there enough space left to create the deep copy?
	assert( _pImpl && copy._pImpl );
	if ( _pImpl && copy._pImpl )
	{
		// Copy all attributes.
		copy._pImpl->address = _pImpl->address;
		memcpy( copy._pImpl->payload , _pImpl->payload , MaximalFramePayloadSize );
		copy._pImpl->size = _pImpl->size;
	}

	// Finally return the deep copy.
	return copy;
}

Frame::FrameAddress Frame::address() const
{
	assert( _pImpl );
	if ( _pImpl )
		return _pImpl->address;
	else
		return FrameAddress();
}

void Frame::setAddress( const Frame::FrameAddress & address )
{
	assert( _pImpl );
	if ( _pImpl )
		_pImpl->address = address;
}

size_t Frame::payloadSize() const
{
	assert( _pImpl );
	if ( _pImpl )
		return _pImpl->size;
	else
		return static_cast<size_t>( -1 );
}

size_t Frame::maximalFramePayloadSize()
{
	return MaximalFramePayloadSize;
}

void Frame::setPayloadSize( size_t size )
{
	assert( _pImpl );
	if ( _pImpl )
		_pImpl->size = size;
}

uint8_t & Frame::operator[]( size_t index )
{
	assert( _pImpl && index < MaximalFramePayloadSize );
	if ( _pImpl && index < MaximalFramePayloadSize )
	{
		_pImpl->size = std::max( _pImpl->size , index + 1 );
		return _pImpl->payload[index];
	}
	else
		return Private::dummyByte;
}

const uint8_t & Frame::operator[]( size_t index ) const
{
	assert( _pImpl && index < MaximalFramePayloadSize );
	if ( _pImpl && index < MaximalFramePayloadSize )
	{
		return _pImpl->payload[index];
	}
	else
		return Private::dummyByte;
}

uint8_t * Frame::payloadBytes()
{
	assert( _pImpl );
	if ( _pImpl )
		return _pImpl->payload;
	else
		return NULL;
}

const uint8_t * Frame::payloadBytes() const
{
	assert( _pImpl );
	if ( _pImpl )
		return _pImpl->payload;
	else
		return NULL;
}

const char * Frame::toString() const
{
	static char str[3 * MaximalFramePayloadSize];

	assert( _pImpl );
	if ( _pImpl )
	{
		for (size_t i = 0; i < payloadSize(); i++)
		{
			sprintf(str + (i * 3), "%02x ", _pImpl->payload[i]);
		}
		// Finish the string
		str[payloadSize() * 3] = '\0';

		return str;
	}
	return NULL;
}

bool Frame::initializeFramePool( size_t numberOfFrames , uint8_t * * memory )
{
	// If we got a pointer to the memory region to use, take the memory from there.
	if ( memory )
	{
		uint8_t * poolPtr = *memory;
		*memory += sizeof( ObjectPool<Private> );

		Private::framePool = new( (void *)poolPtr ) ObjectPool<Private>( numberOfFrames , memory );
	}
	// Otherwise use malloc() (new()).
	else
	{
		Private::framePool = new ObjectPool<Private>( numberOfFrames );
	}

	// Check that the allocation of the pool worked!
	assert( Private::framePool != NULL && Private::framePool->objectCount() == numberOfFrames );
	return ( Private::framePool != NULL && Private::framePool->objectCount() == numberOfFrames );
}
} // namespace phy

#pragma once
#include <cstring>
#include <stdint.h>
#include <algorithm>
#include <string>
#include <assert.h>
#include <cstdio>

#ifndef DESENET_TESTBENCH
#   include "trace/Trace"
#endif

namespace phy {
/**
 * @brief Template class to use as a base for PHY/MAC/Network addresses or any other multi-byte identification purpose.
 */
template<size_t size>
class Address
{
public:
	/**
	 * @brief Creates an invalid (all bytes to 0 is considered invalid) address.
	 */
	explicit Address()
	{
		// Initialize address to default value of all bytes equal 0.
		memset( _address , 0 , size );
	}

	/**
	 * @brief Creates an address with the data taken from the memory region pointed by from.
	 *
	 * Note that it is your responsibility to ensure that there are enough bytes for the address size.
	 *
	 * @param from Pointer to the memory region holding the address.
	 */
	explicit Address( const uint8_t * const from )
	{
		copyFrom( from );
	}

	/**
	 * @brief Initializer-list constructor
	 *
	 * Allows to initializes an address with an array initializer.
	 */
	Address(std::initializer_list<uint8_t> from)
	{
		assert(from.size() == size);
		std::copy(from.begin(), from.end(), _address);
	}

	/**
	 * @brief Destructor, actually it does nothing @ all.
	 */
	virtual ~Address()
	{}

	/**
	 * @brief Returns true if the address is considered as valid (An address is invalid if all bytes are 0).
	 *
	 * @return True if the address is valid, false otherwise.
	 */
	bool isValid() const
	{
		// An address is considered valid, if it is not the equal the default address.
		for ( size_t i = 0 ; i < size ; ++i )
			if ( _address[i] )
				return true;

		return false;
	}

	/**
	 * @brief Returns actual with of address in byte (default ist 4).
	 *
	 * @return Address size in byte
	 */
	static size_t addressSize()
	{
		return size;
	}

	/**
	 * @brief Index operator to access the different bytes of the address.
	 *
	 * @param index 	Index of the byte to get the reference to.
	 * @return 			Reference to the byte of the address at the given index.
	 */
	uint8_t & operator[]( size_t index )
	{
		assert( index < size );
		return _address[std::min( index , size - 1 )];
	}

	/**
	 * @brief Index operator to access the different bytes of the address as constants.
	 *
	 * @param index 	Index of the byte to get the reference to.
	 * @return 			Read only reference to the byte of the address at the given index.
	 */
	const uint8_t & operator[]( size_t index ) const
	{
		assert( index < size );
		return _address[std::min( index , size - 1 )];
	}

	/**
	 * @brief Copies the different bytes of the address from the given memory region pointed by from.
	 *
	 * Note that you are responsible to ensure that the memory region is big enough for the address.
	 *
	 * @param from Pointer to the memory location to copy the address from.
	 */
	void copyFrom( const uint8_t * const from )
	{
		assert( from );
		if ( from )
			memcpy( _address , from , size );
	}

	/**
	 * @brief Copies the different bytes of the address to the given memory region pointed by from.
	 *
	 * Note that you are responsible to ensure that the memory region is big enough to hold the address.
	 *
	 * @param to Pointer to the memory location to copy the address to.
	 */
	void copyTo( uint8_t * const to ) const
	{
		assert( to );
		if ( to )
			memcpy( reinterpret_cast<void *>( to ) ,
					reinterpret_cast<const void *>( _address ) , size );
	}

	/**
	 * @brief Compares the given address with him self and returns true, if they are equal.
	 *
	 * @param other 	Reference to the address to compare with.
	 * @return 			True if the addresses are equal, false otherwise.
	 */
	bool operator==( const Address & other ) const
	{
		return memcmp( _address , other._address , size ) == 0;
	}

	/**
	 * @brief Compares the given address with him self and returns true, if they are equal.
	 *
	 * @param other 	Reference to the address to compare with.
	 * @return 			True if the addresses are equal, false otherwise.
	 */
	bool operator!=( const Address & other ) const
	{
		return !operator==( other );
	}

	/**
	 * @brief Compares the addresses for sorting.
	 *
	 * @param other 	Reference to the address to compare with.
	 * @return 			True if the other is bigger, false otherwise.
	 */
	bool operator<( const Address & other ) const
	{
		for ( unsigned int i = 0 ; i < size ; ++i )
			if ( _address[i] != other._address[i] )
				return _address[i] < other._address[i];

		return false;
	}

	/**
	 * @brief Compares the addresses for sorting.
	 *
	 * @param other 	Reference to the address to compare with.
	 * @return 			True if the other is smaller, false otherwise.
	 */
	bool operator>( const Address & other ) const
	{
		for ( unsigned int i = 0 ; i < size ; ++i )
			if ( _address[i] != other._address[i] )
				return _address[i] > other._address[i];

		return false;
	}

	/**
	 * @brief Converts the address into a hexadecimal string suitable for output.
	 *
	 * @return String containing the hexadecimal representation of the address.
	 */
	const std::string toHexString() const
	{
		char hex[2 * size + 1];

		// Print each byte into the temporary string.
		for ( size_t i = 0 ; i < size ; ++i )
#                   ifndef DESENET_TESTBENCH
			Trace::outToString( hex + 2 * i , 3 , "%02X" , _address[i] );
#                   else
                        snprintf(hex + 2 * i, 2, "%02X", _address[i] );
#                   endif

		return hex;
	}

	/**
	 * @brief Creates an address with the given hexadecimal string.
	 *
	 * Note that the size of the string must be exactly twice the size of the address in bytes.
	 *
	 * @param hex 	A string containing the hex representation of the address.
	 * @return 		An address for the given hex representation.
	 */
	static Address fromHexString( const std::string hex )
	{
		Address address;

		// The size of the hex string MUST match the size of the address.
		assert( hex.size() == 2 * size );
		if ( hex.size() == 2 * size )
		{
			char byteString[] = { 0 , 0 , 0 };
			for ( size_t i = 0 ; i < size ; ++i )
			{
				// Parse byte per byte.
				memcpy( byteString , hex.data() + ( 2 * i ) , 2 );
				address[i] = std::strtol( byteString , NULL , 16 );
			}
		}

		return address;
	}

	/**
	 * @brief Creates and returns a random address.
	 *
	 * Note that the system function rand() will be used to generate the different bytes of the address.
	 *
	 * @return A random address.
	 */
	static Address randomAddress()
	{
		Address address;

		for ( size_t i = 0 ; i < size ; ++i )
			address[i] = rand() % 255;

		return address;
	}

	inline const uint8_t * data() const { return _address; }	///< @brief Direct access to address.

private:
	uint8_t _address[size];		// The actual address array.
};

} // namespace phy

/** @} */

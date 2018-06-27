
#include "system.h"
#include "diskcacheimpl.h"

#include "mm.h"

using namespace drv;

enum DiskCacheBufferStatus
{
	DCBPresent,
	DCBNotPresent,
};

class DiskCacheBuffer
	:	public itf::IDiskCacheBuffer
{
	itf::IBlockDevice*	_pDevice;
	unsigned int		_first_block;
	unsigned int		_block_count;
	unsigned int		_length;

	unsigned int		_lock_count;

	void*				_buffer;	

	DiskCacheBufferStatus	_status;

public:
	DiskCacheBuffer( itf::IBlockDevice* pDevice, unsigned int first_block, unsigned int block_count, bool load )
		:	_pDevice( pDevice ),
			_first_block( first_block ),
			_block_count( block_count ),
			_length( block_count * pDevice->block_size() ),
			_lock_count( 0 ),			
			_buffer( 0 ),
			_status( DCBNotPresent )
	{
		if( load )
		{
			_buffer = mm::alloc_pages( mm::calc_required_page_count( _length ) );
			if( _buffer && _pDevice->read( _first_block, _block_count, _buffer ) )
			{
				_status = DCBPresent;
			}
		}
	}

	DiskCacheBuffer( const DiskCacheBuffer&& ) = delete;
	DiskCacheBuffer( const DiskCacheBuffer& ) = delete;
	void operator=( const DiskCacheBuffer& ) = delete;

	virtual size_t	block_count()
	{
		return _block_count;
	}

	virtual	size_t	length()
	{
		return _length;
	}

	virtual void*	lock()
	{
		switch( _status )
		{
		case DCBPresent:
			++_lock_count;
			return _buffer;

		case DCBNotPresent:
			if( !_buffer )
				_buffer = mm::alloc_pages( mm::calc_required_page_count( _length ) );
			
			if( _buffer && _pDevice->read( _first_block, _block_count, _buffer ) )
			{
				_status = DCBPresent;
				return _buffer;
			}

			return 0;

		default:
			return 0;
		}		
	}

	virtual void	unlock()
	{
		--_lock_count;
	}

	virtual bool	locked()
	{
		return _lock_count > 0;
	}

	virtual bool	free()
	{
		if( locked() )
			return false;

		mm::free_pages( _buffer );
		_buffer = 0;
		_status = DCBNotPresent;

		return true;
	}
};



DiskCache::DiskCache( itf::IBlockDevice* pDevice )
	:	_pDevice( pDevice )
{
}

const char*	drv::DiskCache::description()
{	
	return "Disk Cache";
}

void		drv::DiskCache::init()
{
}

unsigned int		drv::DiskCache::device_block_count()
{
	return _pDevice->block_count();
}

unsigned int		drv::DiskCache::device_block_size()
{
	return _pDevice->block_size();
}

drv::itf::IDiskCacheBuffer*	drv::DiskCache::prepare( unsigned int first_block, unsigned int block_count )
{
	auto it = _cache.find( first_block );
	if( it != _cache.end() )
	{
		auto buffer = it->second;

		if( buffer->block_count() <= block_count )
		{
			buffer->lock();

			return buffer;
		}
	}

	return new DiskCacheBuffer( _pDevice, first_block, block_count, false );
}

drv::itf::IDiskCacheBuffer*	drv::DiskCache::cache( unsigned int first_block, unsigned int block_count )
{
	auto it = _cache.find( first_block );
	if( it != _cache.end() )
	{
		auto buffer = it->second;

		if( buffer->block_count() <= block_count )
		{
			buffer->lock();

			return buffer;
		}
	}

	return new DiskCacheBuffer( _pDevice, first_block, block_count, true );
}

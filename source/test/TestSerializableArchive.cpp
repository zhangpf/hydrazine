/*!

	\file TestSerializableArchive.cpp
	
	\author Gregory Diamos
	
	\date Sunday July 20, 2008
	
	\brief Source file for the TestSerializableArchive class. 
	
*/

#ifndef TEST_SERIALIZABLE_ARCHIVE_CPP_INCLUDED
#define TEST_SERIALIZABLE_ARCHIVE_CPP_INCLUDED

#include "TestSerializableArchive.h"
#include <cstring>

namespace test
{

	////////////////////////////////////////////////////////////////////////////
	// SimpleSerializableAllocator
	common::Serializable* TestSerializableArchive::SimpleSerializable::SimpleSerializableAllocator::allocate() const
	{
	
		return new TestSerializableArchive::SimpleSerializable;
	
	}
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	// SimpleSerializable
	TestSerializableArchive::SimpleSerializable::SimpleSerializable()
	{
	
		_size = 0;
	
	}
					
	TestSerializableArchive::SimpleSerializable::~SimpleSerializable()
	{
	
		if( _size > 0 )
		{
		
			delete[] _data;
		
		}
	
	}
	
	unsigned int TestSerializableArchive::SimpleSerializable::size() const
	{
	
		return _size;
	
	}
	
	void TestSerializableArchive::SimpleSerializable::resize( unsigned int size )
	{
	
		if( size != _size )
		{
		
			if( _size > 0 )
			{
			
				delete[] _data;
			
			}
			
			_size = size;
			
			if( _size > 0 )
			{
			
				_data = new char[ _size ];
			
			}
		
		}
	
	}
	
	void* TestSerializableArchive::SimpleSerializable::data()
	{
	
		return _data;
	
	}
	
	common::Serializable::Id TestSerializableArchive::SimpleSerializable::id() const
	{
	
		return 0;
	
	}
	
	void TestSerializableArchive::SimpleSerializable::serialize( common::SerializationBuffer& b ) const
	{
	
		b.write( &_size, sizeof( unsigned int ) );
		b.write( _data, _size );
	
	}
	
	void TestSerializableArchive::SimpleSerializable::deserialize( common::SerializationBuffer& b )
	{
	
		unsigned int size;
		b.read( &size, sizeof( unsigned int ) );
		resize( size );
		b.read( _data, _size );
	
	}
	
	common::Serializable::Allocator* TestSerializableArchive::SimpleSerializable::allocator() const
	{
	
		return new TestSerializableArchive::SimpleSerializable::SimpleSerializableAllocator;
	
	}
	
	bool TestSerializableArchive::SimpleSerializable::operator!=( const TestSerializableArchive::SimpleSerializable& object ) const
	{
	
		if( _size != object._size )
		{
		
			return true;
		
		}
		else
		{
		
			return memcmp( object._data, _data, _size ) != 0;
		
		}
	
	}
	////////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////////
	// TestSerializableArchive
	bool TestSerializableArchive::testSaveLoad( std::string& status ) const
	{
	
		bool pass;
		
		pass = true;
	
		std::stringstream stream;
		
		common::SerializableArchive archive;
		
		TestSerializableArchive::SimpleSerializable objectIn;
		TestSerializableArchive::SimpleSerializable objectOut;
		
		archive.registerClass( &objectIn );
		
		objectIn.resize( size );
		
		for( unsigned int i = 0; i < objectIn.size(); i++ )
		{
		
			static_cast<char*>( objectIn.data() )[i] = i;
		
		}
		
		// test correctness
		archive.save( &objectIn );
		archive.load( &objectOut );
		
		if( objectIn != objectOut )
		{
		
			stream << "Correctness test failed for SerializableArchive load to existing object.\n";
			
			pass = false;
		
		}
		
		TestSerializableArchive::SimpleSerializable* objectClone;
		
		archive.save( &objectIn );
		objectClone = static_cast< TestSerializableArchive::SimpleSerializable* >( archive.load() );
		
		if( objectIn != *objectClone )
		{
		
			stream << "Correctness test failed for SerializableArchive load to new object.\n";
			
			pass = false;
		
		}
		
		delete objectClone;
		
		common::Timer Timer;
		
		// benchmark reference
		Timer.start();
		
		for( unsigned int i = 0; i < iterations; i++ )
		{
		
			archive.save( &objectIn );
			archive.load( &objectOut );
		
		}
		
		Timer.stop();
		
		stream << "SerializableArchive average save/load time for " << iterations << " iterations: " << (Timer.seconds() / iterations) << " seconds.\n";

		#ifdef HAVE_BOOST_SERIALIZATION
	
		// boost correctness
		objectOut.resize( 0 );
		
		{
		
			std::ofstream boostArchiveFile( "_temp.archive" );
		
			boost::archive::text_oarchive boostArchive( boostArchiveFile );
		
			boostArchive << const_cast< const TestSerializableArchive::SimpleSerializable& >( objectIn );
		
		}
		
		{
		
			std::ifstream boostArchiveFile( "_temp.archive", std::ios::binary );
		
			boost::archive::text_iarchive boostArchive( boostArchiveFile );
		
			boostArchive >> objectOut;
		
		}
		
		if( objectIn != objectOut )
		{
		
			stream << "Correctness test failed for BoostArchive load to existing object.\n";
			
			pass = false;
		
		}
		
		// benchmark boost
		Timer.start();
		
		for( unsigned int i = 0; i < iterations; i++ )
		{
		
			{
		
				std::ofstream boostArchiveFile( "_temp.archive" );
		
				boost::archive::text_oarchive boostArchive( boostArchiveFile );
		
				boostArchive << static_cast< const TestSerializableArchive::SimpleSerializable& >( objectIn );
		
			}
		
			{
		
				std::ifstream boostArchiveFile( "_temp.archive", std::ios::binary );
		
				boost::archive::text_iarchive boostArchive( boostArchiveFile );
		
				boostArchive >> objectOut;
		
			}		

		}
		
		Timer.stop();
		
		stream << "BoostArchive average save/load time for " << iterations << " iterations: " << (Timer.seconds() / iterations) << " seconds.\n";

		#endif
		
		status += stream.str();
		
		return pass;
		
	}
	
	bool TestSerializableArchive::testDisk( std::string& status ) const
	{

		bool pass;
		
		pass = true;
	
		std::stringstream stream;
		
		common::SerializableArchive archive;
		
		TestSerializableArchive::SimpleSerializable objectIn;
		TestSerializableArchive::SimpleSerializable objectOut;
		
		archive.registerClass( &objectIn );
		
		objectIn.resize( size );
		
		for( unsigned int i = 0; i < objectIn.size(); i++ )
		{
		
			static_cast<char*>( objectIn.data() )[i] = i;
		
		}
		
		// test correctness
		archive.save( &objectIn );
		archive.saveToFile( "_temp.archive" );
		archive.loadFromFile( "_temp.archive" );
		archive.load( &objectOut );
		
		if( objectIn != objectOut )
		{
		
			stream << "Correctness test failed for SerializableArchive load to existing object.\n";
			
			pass = false;
		
		}
		
		TestSerializableArchive::SimpleSerializable* objectClone;
		
		archive.save( &objectIn );
		objectClone = static_cast< TestSerializableArchive::SimpleSerializable* >( archive.load() );
		
		if( objectIn != *objectClone )
		{
		
			stream << "Correctness test failed for SerializableArchive load to new object.\n";
			
			pass = false;
		
		}
		
		delete objectClone;
		
		common::Timer Timer;
		
		// benchmark reference
		Timer.start();
		
		for( unsigned int i = 0; i < iterations; i++ )
		{
		
			archive.save( &objectIn );
			archive.saveToFile( "_temp.archive" );
			archive.loadFromFile( "_temp.archive" );
			archive.load( &objectOut );
		
		}
		
		Timer.stop();
		
		stream << "SerializableArchive average save/load time for " << iterations << " iterations: " << (Timer.seconds() / iterations) << " seconds.\n";
	
		status += stream.str();
	
		return pass;
	
	
	}
	
	#ifdef HAVE_MPI	
	
	bool TestSerializableArchive::testMpi( std::string& status ) const
	{
		
		bool pass;
		
		pass = true;
	
		std::stringstream stream;
		
		common::SerializableArchive archive;
		
		TestSerializableArchive::SimpleSerializable object;
		
		archive.registerClass( &object );
		
		object.resize( size );
		
		for( unsigned int i = 0; i < object.size(); i++ )
		{
		
			static_cast<char*>( object.data() )[i] = i;
		
		}
		
		switch( rank )
		{
		
			case 0:
			{
			
				common::Timer Timer;
		
				// benchmark reference
				Timer.start();
		
				for( unsigned int i = 0; i < iterations; i++ )
				{
		
					archive.save( &objectIn );					
					MPI_Ssend( archive.buffer().ptr(), archive.buffer().size(), MPI_CHAR, 1, 0, MPI_COMM_WORLD );
					archive.clear();
		
				}
		
				Timer.stop();
		
				stream << "SerializableArchive average save/mpiSsend time for " << iterations << " iterations: " << (Timer.seconds() / iterations) << " seconds.\n";
				
				break;
				
			}
			
			case 1:
			{
			
				for( unsigned int i = 0; i < iteratios; i++ )
				{
				
					MPI_Status status;
					
					TestSerializableArchive::SimpleSerializable objectOut;

					MPI_Probe( 0, 0, MPI_COMM_WORLD, &status );
					
					unsigned int size;
					
					MPI_Get_count( &status, MPI_CHAR, &size );
				
					archive.buffer().clear();
					archive.buffer().resize( size );
					archive.buffer().initializeWritePointer( size );
					
					MPI_Recv( archive.buffer().ptr(), size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status ); 
					
					archive.load( &objectOut );
					
					assert( ! ( objectOut != object ) );
					
				}
						
			}

		}

		#ifdef HAVE_BOOST_SERIALIZATION
	
		switch( rank )
		{
		
			case 0:
			{
			
				common::Timer Timer;
		
				// benchmark boost
				Timer.start();
		
				for( unsigned int i = 0; i < iterations; i++ )
				{
		
					{
		
						std::ofstream boostArchiveFile( "_temp.archive" );
		
						boost::archive::text_oarchive boostArchive( boostArchiveFile );
		
						boostArchive << static_cast< const TestSerializableArchive::SimpleSerializable& >( objectIn );
		
					}
		
					{
		
						std::ifstream boostArchiveFile( "_temp.archive", std::ios::binary );
		
						unsigned int size;
		
						// get size of file
						file.seekg( 0, std::ifstream::end );
		
						size = file.tellg();
		
						file.seekg( 0 );
						
						char* buffer = new char[ size ];
						
						boostArchiveFile.read( buffer, size );
						
						MPI_Ssend( buffer, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD );
						
						delete[] buffer;
						
					}		

				}
		
				Timer.stop();
		
				stream << "Boost Serialization average save/mpiSsend time for " << iterations << " iterations: " << (Timer.seconds() / iterations) << " seconds.\n";
				
				break;
				
			}
			
			case 1:
			{
			
				for( unsigned int i = 0; i < iteratios; i++ )
				{
				
					MPI_Status status;
					
					TestSerializableArchive::SimpleSerializable objectOut;

					MPI_Probe( 0, 0, MPI_COMM_WORLD, &status );
					
					unsigned int size;
					
					MPI_Get_count( &status, MPI_CHAR, &size );
				
					char* buffer = new char[ size ];
					
					MPI_Recv( buffer, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status ); 
					
					{
					
						std::ofstream boostArchiveFile( "_temp_recv.archive", std::ofstream::binary );
					
						boostArchiveFile.write( buffer, size );
					
					}
					
					{
		
						std::ifstream boostArchiveFile( "_temp_recv.archive", std::ios::binary );
		
						boost::archive::text_iarchive boostArchive( boostArchiveFile );
		
						boostArchive >> objectOut;
		
					}		
										
					assert( ! ( objectOut != object ) );
					
				}
						
			}

		}


		#endif
		
		status += stream.str();
		
		return pass;	

	}
	
	#endif	
	
	bool TestSerializableArchive::doTest( std::string& status )
	{
	
		bool pass;
		
		pass = true;
		
		if( rank == 0 )
		{
		
			pass &= testSaveLoad( status );
			pass &= testDisk( status );

		}

		#ifdef HAVE_MPI		
		
		if( ranks >= 2 )
		{

			pass &= testMpi( status );

		}
			
		#endif

		return pass;
	
	}
	
	TestSerializableArchive::TestSerializableArchive()
	{
	
		Name = "TestSerializableArchive";
		
		Description += "Make sure that the implementation of a Serializable\n";
		Description += "archive works as intended.\n";  
	
		Description += "Make sure that the performance is also reasonable.  Compare\n";
		Description += "to the boost serialization library.  This should test and benchmark saving to\n";
		Description += "a buffer then immediately restoring, saving to and restoring from disk,\n";
		Description += "and saving to an archive, sending the archive over mpi, and restoring at the\n";
		Description += "other end.";
	
	}
	////////////////////////////////////////////////////////////////////////////
	
}

int main( int argc, char** argv )
{

	common::ArgumentParser parser( argc, argv );
	
	test::TestSerializableArchive test;

	#ifdef HAVE_MPI
	
	MPI_Init( &argc, &argv );

	MPI_Comm_size( MPI_COMM_WORLD, &test.ranks );
	
	MPI_Comm_rank( MPI_COMM_WORLD, &test.rank );
	
	#else
	
	test.ranks = 0;
	
	test.rank = 0;	
	
	#endif
	
	parser.description( test.description() );
	
	bool help;
	
	parser.parse( "-h", help, "Print this help message." );
	
	parser.parse( "-v", test.verbose, "Print out status message when the test is over." );
	parser.parse( "-i", test.iterations, 1000, "How many times to perform the test before computing the avarege time?" );
	parser.parse( "-s", test.size, 100, "How many bytes does the object being serialized contain?" );

	if( help && test.rank == 0 )
	{
	
		std::cout << parser.help();
	
		return false;
	
	}

	test.test();

	if( test.verbose && test.rank == 0 )
	{

		std::cout << test.toString();
	
	}

	#ifdef HAVE_MPI

	MPI_Finalize();

	#endif

	return test.passed();

}

#endif

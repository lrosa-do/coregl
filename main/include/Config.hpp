#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#define CORE_WINDOWS
#define CORE_WINDOWS_API
#define SYSTEM_DESKTOP
#endif

#if defined(__linux__) || defined(__linux) || defined(linux)
#define CORE_LINUX
#define CORE_LINUX_API
#define SYSTEM_DESKTOP
#endif

#ifdef CORE_WINDOWS_API

#ifndef CORE_STATIC_LIB_
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // CORE_EXPORT
#else
#define CORE_API
#endif // CORE_STATIC_LIB_

#if defined(_STDCALL_SUPPORTED)
#define CORECALLCONV __stdcall
#else
#define CORECALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else // CORE_WINDOWS_API

// Force symbol export in shared libraries built with gcc.
#if (__GNUC__ >= 4) && !defined(CORE_STATIC_LIB_) && defined(CORE_EXPORTS)
#define CORE_API __attribute__((visibility("default")))
#else
#define CORE_API
#endif

#define CORECALLCONV

#endif // CORE_WINDOWS_API

typedef unsigned char u8;
typedef signed char s8;
typedef char c8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef long long s64;
typedef float f32;
typedef double f64;


#if defined(_DEBUG)
	#include <assert.h>
	#define DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#else
	#define DEBUG_BREAK_IF( _CONDITION_ )
#endif


class IDrop
	{
	public:

	IDrop()
			: DebugName(0), ReferenceCounter(1)
		{
		}


		virtual ~IDrop()
		{
           
		}

		void Grab() const { ++ReferenceCounter; }

		bool Drop() const
		{
		

			--ReferenceCounter;
			if (!ReferenceCounter)
			{
				delete this;
				return true;
			}

			return false;
		}

	
		s32 getReferenceCount() const
		{
			return ReferenceCounter;
		}

		const c8* getDebugName() const
		{
			return DebugName;
		}

	protected:

	
		void setDebugName(const c8* newName)
		{
			DebugName = newName;
		}

	private:

		const c8* DebugName;
		mutable s32 ReferenceCounter;
	};
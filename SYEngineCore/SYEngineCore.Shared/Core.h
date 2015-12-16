#pragma once

#include "pch.h"

namespace SYEngineCore
{
	public ref class Core sealed
	{
	public:
		static void Initialize();
		static void Uninitialize();
		
	private:
		Core() { }
	};
}
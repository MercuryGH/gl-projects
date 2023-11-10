#pragma once

namespace brep {

template<typename T>
struct SelfLinked {
	T* prev{ nullptr };
	T* next{ nullptr };
};

}
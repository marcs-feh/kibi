#pragma once
#include "core.hpp"
#include "memory.hpp"

namespace core {
//// Dynamic Array
template<typename T>
struct DynamicArray {
	template<typename U>
	void append(U&& e){
		if(this->len_ >= this->cap_){
			bool ok = this->resize(max(isize(16), this->len_ * 2));
			if(!ok){ return; }
		}
		void* loc = (void*)(this->data_ + this->len_);
		new (loc) T(core::forward<U>(e));
		this->len_ += 1;
	}

	void pop(){
		if(this->len_ <= 0){ return; }

		len_ -= 1;
		if constexpr(!TriviallyDestructible<T>){
			data_[len_].~T();
		}
	}

	void remove(isize idx){
		if(idx < 0 || idx >= len_){ return; }
		if constexpr(!TriviallyDestructible<T>){
			data_[idx].~T();
		}
		mem_copy(&data_[idx], &data_[idx+1], (len_ - idx) * sizeof(T));
		len_ -= 1;
	}

	template<typename U>
	void insert(isize idx, U&& val){
		if(idx < 0 || idx > len_){ return; }

		if(this->len_ >= this->cap_){
			bool ok = this->resize(max(isize(16), this->len_ * 2));
			if(!ok){ return; }
		}

		mem_copy(&data_[idx+1], &data_[idx], (len_ - idx) * sizeof(T));
		new (data_ + idx) T(core::forward<U>(val));
		len_ += 1;
	}

	bool resize(isize new_cap){
		T* new_data = (T*)this->allocator_->realloc(this->data_, this->cap_ * sizeof(T), new_cap * sizeof(T), alignof(T));
		if(new_data == nullptr){ return false; }
		this->data_ = new_data;

		// Delete trailing objects when shrinking
		isize delta = this->cap_ - new_cap;
		if(delta > 0){
			if constexpr(!TriviallyDestructible<T>){
				for(isize i = delta; i < this->cap_; i += 1){
					this->data_[i].~T();
				}
			}
		}

		this->len_ = min(this->len_, new_cap);
		this->cap_ = new_cap;
		return true;
	}
	
	void shrink(){
		resize(len_);
	}

	Slice<T> get_owned_slice(){
		shrink();
		Slice<T> s = Slice<T>(data_, len_);
		data_ = nullptr;
		len_ = 0;
		cap_ = 0;
		return s;
	}

	DynamicArray copy(Allocator* allocator)
		requires CopyConstructible<T>
	{
		auto arr = DynamicArray<T>::create(allocator, this->len_);
		if(arr.cap_ != this->len_){ return {}; }

		if constexpr(TriviallyCopyable<T>){
			mem_copy_no_overlap(arr.data_, this->data_, arr.len_ * sizeof(T));
		} else {
			for(isize i = 0; i < this->len_; i += 1){
				new (arr.data_ + i) T(this->data_[i]);
			}
			arr->len_ = this->len_;
		}

		return arr;
	}

	T& operator[](isize idx){
		ensure_bounds_check(idx >= 0 && idx < len_, "Index to dynamic array is out of bounds");
		return data_[idx];
	}

	T const& operator[](isize idx) const{
		ensure_bounds_check(idx >= 0 && idx < len_, "Index to dynamic array is out of bounds");
		return data_[idx];
	}

	void clear(){
		if constexpr(!TriviallyDestructible<T>){
			for(isize i = 0; i < len_; i++){
				data_[i].~T();
			}
		}
		this->len_ = 0;
	}

	Slice<T> slice(){
		return Slice(data_, len_);
	}

	DynamicArray* drop(){
		clear();
		if(data_){
			allocator_->free(this->data_, sizeof(T) * this->cap_, alignof(T));
			this->data_ = nullptr;
			this->cap_ = 0;
		}
		return this;
	}

	[[nodiscard]]
	static DynamicArray create(Allocator* allocator, isize cap){
		DynamicArray<T> arr;
		arr.data_ = (T*)allocator->alloc(cap * sizeof(T), alignof(T));
		arr.len_ = 0;
		arr.cap_ = arr.data_ ? cap : 0;
		arr.allocator_ = allocator;
		return arr;
	}

	DynamicArray() : data_{0}, len_{0}, cap_{0}, allocator_{0} {}
	DynamicArray(DynamicArray const&) = delete;

	DynamicArray(DynamicArray&& arr)
		: data_{core::exchange(arr.data_, nullptr)}
		, len_{core::exchange(arr.len_, 0)}
		, cap_{core::exchange(arr.cap_, 0)}
		, allocator_{core::exchange(arr.allocator_, nullptr)} {}

	DynamicArray& operator=(DynamicArray&& arr){
		return *new (this->drop()) DynamicArray { core::move(arr) };
	}

	~DynamicArray(){ drop(); }

	[[nodiscard]] constexpr forceinline auto len() const { return len_; }
	[[nodiscard]] constexpr forceinline auto cap() const { return cap_; }
	[[nodiscard]] constexpr forceinline auto data() const { return data_; }
	[[nodiscard]] constexpr forceinline auto allocator() const { return allocator_; }

	// C++ Iterator insanity
	constexpr auto begin() { return LinearIterator{ data_ }; }
	constexpr auto end() { return LinearIterator{ data_ + len_}; }

private:
	T* data_;
	isize len_;
	isize cap_;
	Allocator* allocator_;
};
} /* Universal namespace */

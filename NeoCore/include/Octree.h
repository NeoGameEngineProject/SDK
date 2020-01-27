#ifndef NEO_OCTREE_H
#define NEO_OCTREE_H

#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include <cassert>
#include <mutex>
#include <type_traits>

#include <Vector3.h>

#include <Log.h>
#include "Maths.h"
#include <Box3D.h>

namespace Neo 
{

// Based on the octant layout mechanic used here:
// https://github.com/brandonpelfrey/SimpleOctree/blob/master/Octree.h
template<typename T, unsigned short NODE_CAPACITY = 8>
class Octree
{
public:
	Octree(const Vector3& position, const Vector3& halfSize):
		position(position),
		halfSize(halfSize) {}
	
	Octree(const Octree& octree) = delete;
	Octree(Octree&&) = default;
	
	~Octree()
	{
		for(unsigned short i = 0; i < 8; i++)
			delete children[i];
	}
	
	///< (position, halfSize, object)
	typedef std::tuple<Vector3, Vector3, T> Position;

	bool isLeaf() const
	{
		bool value = false;
		for(unsigned short i = 0; i < 8; i++)
			value |= !children[i];
		return value;
	}
	
	bool isFitting(const Vector3& sz) const
	{
		return sz.x <= halfSize.x && sz.y <= halfSize.y && sz.z <= halfSize.z;
	}
	
	bool contains(const Vector3& point) const
	{
		const Vector3 boxMin = position - halfSize;
		const Vector3 boxMax = position + halfSize;
	
		return isPointInBox(point, boxMin, boxMax);
	}
	
	Box3D makeBox(const Vector3& position, const Vector3& halfSize) const
	{
		Box3D box;
		box.min = position - halfSize;
		box.max = position + halfSize;
		return box;
	}
	
	template<typename Fn>
	void traverse(const Vector3& boxMin, const Vector3& boxMax, Fn functor)
	{
		for(unsigned short i = 0; i < data.size(); i++)
		{
			const auto& pos = std::get<0>(data[i]);
			const auto aabb = makeBox(pos, std::get<1>(data[i]));
			if(!isPointInBox(pos, boxMin, boxMax) && !isBoxToBoxCollision(boxMin, boxMax, aabb.min, aabb.max))
			{
				continue;
			}
			functor(&data[i]);
		}
		
		if(!isLeaf())
		{
			for(unsigned short i = 0; i < 8; i++)
			{
				const Vector3 childMax = children[i]->position + children[i]->halfSize;
				const Vector3 childMin = children[i]->position - children[i]->halfSize;
				
				if(!isBoxToBoxCollision(childMin, childMax, boxMin, boxMax))
				{
					continue;
				}
				
				children[i]->traverse(boxMin, boxMax, functor);
			}
		}
	}
	
	template<typename Fn>
	void traverse(const Vector3& origin, float radius, Fn functor)
	{
		for(unsigned short i = 0; i < data.size(); i++)
		{
			const auto& pos = std::get<0>(data[i]);
			if((origin - pos).getLength() > radius)
				continue;
			
			functor(&data[i]);
		}
		
		if(!isLeaf())
		{
			for(unsigned short i = 0; i < 8; i++)
			{
				if((origin - children[i]->position).getLength() - children[i]->halfSize.getLength() > radius)
					continue;
				
				children[i]->traverse(origin, radius, functor);
			}
		}
	}
	
	T* getDataSlot(const Vector3& objectPosition, const Vector3& objectHalfSize)
	{
		if(!isFitting(objectHalfSize))
			return nullptr;
		
		Position newPos = Position{objectPosition, objectHalfSize, T()};
		data.push_back(newPos);
		return &std::get<2>(data.back());
	}
	
	T* insert(const Vector3& objectPosition, const Vector3& objectHalfSize)
	{
		if(!contains(objectPosition))
			return nullptr;
		
		if(isLeaf())
		{
			// Just return the data entry if we have space.
			if(data.size() < NODE_CAPACITY)
			{
				return getDataSlot(objectPosition, objectHalfSize);
			} 
			else // If our node is full, split
			{
				for(unsigned short i = 0; i < 8; i++)
				{
					Vector3 newPosition = position;
					newPosition.x += halfSize.x * (i&4 ? 0.5f : -0.5f);
					newPosition.y += halfSize.y * (i&2 ? 0.5f : -0.5f);
					newPosition.z += halfSize.z * (i&1 ? 0.5f : -0.5f);
					
					children[i] = new Octree<T, NODE_CAPACITY>(newPosition, halfSize*0.5f);
				}
				
				auto tmpCopy = data;
				data.clear();

				for(const auto& k : tmpCopy)
				{
					if(!children[getOctantContainingPoint(std::get<0>(k))]->insert(std::get<0>(k), std::get<1>(k), std::get<2>(k)))
						insert(std::get<0>(k), std::get<1>(k), std::get<2>(k));
				}
				
				auto* obj = children[getOctantContainingPoint(objectPosition)]->insert(objectPosition, objectHalfSize);
				if(!obj)
				{
					// If we really MUST insert here, do it...
					return getDataSlot(objectPosition, objectHalfSize);
				}
				
				return obj;
			}
		}
		else
		{
			auto* obj = children[getOctantContainingPoint(objectPosition)]->insert(objectPosition, objectHalfSize);
			if(!obj && !isFitting(objectHalfSize))
			{
				return nullptr;
			}
			else if(!obj)
			{
				return getDataSlot(objectPosition, objectHalfSize);
			}
			
			return obj;
		}
		
	}
	
	T* insert(const Vector3& position, const Vector3& halfSize, const T& object)
	{
		if(!isFitting(halfSize) || !contains(position))
		{
			return nullptr;
		}
		
		T* newObject = insert(position, halfSize);
		*newObject = object;
		return newObject;
	}
	
	void remove(const Vector3& position, const T& object)
	{
		auto* node = this;
		do
		{
			auto removed = std::remove_if(node->data.begin(), node->data.end(), [&object](const Position& pos) {
				return std::get<2>(pos) == object;
			});
			
			if(removed != node->data.end())
			{
				node->data.erase(removed);
				return;
			}
			
			node = node->findDirection(position);
		}
		while(node);
	}
	
	T* update(const Vector3& position, const Vector3& newPosition, const Vector3& newHalfSize, const T& object)
	{
		remove(position, object);
		return insert(newPosition, newHalfSize, object);
	}
	
	Octree<T, NODE_CAPACITY>* findDirection(const Vector3& position)
	{
		return children[getOctantContainingPoint(position)];
	}
	
	void grow()
	{
#if 0
		auto newChild = new Octree<T, NODE_CAPACITY>(std::move(*this));
		position = Vector3(newChild->position) - newChild->halfSize;
		halfSize = newChild->halfSize*2.0f;
		
		children[getOctantContainingPoint(newChild->position)] = newChild;
		
		for(unsigned short i = 0; i < 8; i++)
		{
			Vector3 newPosition = position;
			newPosition.x += halfSize.x * (i&4 ? 0.5f : -0.5f);
			newPosition.y += halfSize.y * (i&2 ? 0.5f : -0.5f);
			newPosition.z += halfSize.z * (i&1 ? 0.5f : -0.5f);
			
			children[i] = new Octree<T, NODE_CAPACITY>(newPosition, halfSize*0.5f);
		}
#endif
	}
	
	void clear()
	{
		for(unsigned short i = 0; i < 8; i++)
		{
			delete children[i];
			children[i] = nullptr;
		}
		
		data.clear();
	}
	
private:
	std::vector<Position> data;
	
	Vector3 position = Vector3(0, 0, 0);
	Vector3 halfSize = Vector3(0, 0, 0);
	
	Octree* parent = nullptr;
	Octree* children[8] = {nullptr};
	
	int getOctantContainingPoint(const Vector3& point) const 
	{
		int oct = 0;
		if(point.x >= position.x) oct |= 4;
		if(point.y >= position.y) oct |= 2;
		if(point.z >= position.z) oct |= 1;
		return oct;
	}
};

}

#endif // NEO_OCTREE_H

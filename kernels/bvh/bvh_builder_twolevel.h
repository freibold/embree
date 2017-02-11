// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "bvh.h"
#include "../common/primref.h"
#include "../builders/priminfo.h"

namespace embree
{
  namespace isa
  {
    template<int N, typename Mesh>
    class BVHNBuilderTwoLevel : public Builder
    {
      ALIGNED_CLASS;

      typedef BVHN<N> BVH;
      typedef typename BVH::AlignedNode AlignedNode;
      typedef typename BVH::NodeRef NodeRef;

    public:

      typedef void (*createMeshAccelTy)(Mesh* mesh, AccelData*& accel, Builder*& builder);

      struct __aligned(32) BuildRef : public PrimRef
      {
      public:
        __forceinline BuildRef () {}

        __forceinline BuildRef (const BBox3fa& bounds, NodeRef node, const unsigned int geomID = 0, const unsigned int numPrimitives = 0)
          : PrimRef(bounds,geomID), node(node), geomID(geomID), numPrimitives(numPrimitives)
        {
          if (node.isLeaf())
            lower.w = 0.0f;
          else
            lower.w = area(this->bounds());
        }

        friend bool operator< (const BuildRef& a, const BuildRef& b) {
          return a.lower.w < b.lower.w;
        }

      public:
        NodeRef node;
        unsigned int geomID;
        unsigned numPrimitives;
      };
      
      /*! Constructor. */
      BVHNBuilderTwoLevel (BVH* bvh, Scene* scene, const createMeshAccelTy createMeshAcce, const size_t singleThreadThreshold = DEFAULT_SINGLE_THREAD_THRESHOLD);
      
      /*! Destructor */
      ~BVHNBuilderTwoLevel ();
      
      /*! builder entry point */
      void build(size_t threadIndex, size_t threadCount);
      void deleteGeometry(size_t geomID);
      void clear();

      void open_sequential(size_t numPrimitives);
      void open_merge_build_sequential(const mvector<BuildRef> &buildRefs, const PrimInfo &pinfo);
      
    public:
      BVH* bvh;
      std::vector<BVH*>& objects;
      std::vector<Builder*> builders;
      
    public:
      Scene* scene;
      createMeshAccelTy createMeshAccel;
      
      mvector<BuildRef> refs;
      mvector<PrimRef> prims;
      std::atomic<int> nextRef;
      const size_t singleThreadThreshold;

      typedef mvector<BuildRef> bvector;

    };
  }
}

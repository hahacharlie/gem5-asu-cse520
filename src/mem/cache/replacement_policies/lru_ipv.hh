/**
 * @file
 * Declaration of an LRU-IPV (Insertion Position Vector) replacement policy.
 * The victim is chosen using position-based LRU with configurable insertion
 * and promotion positions defined by the IPV vector.
 */

#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LRU_IPV_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LRU_IPV_HH__

#include <cstdint>
#include <memory>
#include <vector>

#include "mem/cache/replacement_policies/base.hh"

struct LRUIPVRPParams;

namespace ReplacementPolicy {

class LRUIPVRP : public Base
{
  private:
    /**
     * Position vector type. This vector tracks the LRU position of each
     * block in a cache set. Position 0 is MRU, position (numWays-1) is LRU.
     */
    typedef std::vector<int> PositionVector;

    /**
     * Number of ways (associativity) in the cache set.
     * For a 16-way cache, this will be 16.
     */
    const uint64_t numWays;

    /**
     * Count of the number of blocks instantiated. Used to determine
     * when to create a new shared position vector for each cache set.
     */
    uint64_t count;

    /**
     * Pointer to the current position vector instance being populated.
     * All blocks in the same set share this vector.
     */
    PositionVector* currentPositionVector;

    /**
     * Insertion Position Vector (IPV) defining insertion and promotion behavior.
     * - Indices 0 to (numWays-1): promotion positions for blocks at each position
     * - Index numWays: insertion position for newly inserted blocks
     * For 16-way cache: IPV has 17 elements (indices 0-16)
     */
    const std::vector<int> IPV;

  protected:
    /**
     * LRU-IPV-specific implementation of replacement data.
     * Each block stores its way index and shares a position vector with
     * all other blocks in the same cache set.
     */
    struct IPVReplData : ReplacementData
    {
        /**
         * The way index of this block within its cache set (0 to numWays-1).
         * This is used to index into the shared position vector.
         */
        const uint64_t wayIndex;

        /**
         * Shared position vector pointer. All blocks in the same set share
         * this vector, which tracks the LRU position of each way.
         * positions[wayIndex] gives the current LRU position of this block.
         */
        std::shared_ptr<PositionVector> positions;

        /**
         * Constructor.
         *
         * @param way_index The way index of this block in its set.
         * @param pos_vec Shared pointer to the position vector for this set.
         */
        IPVReplData(const uint64_t way_index, 
                    std::shared_ptr<PositionVector> pos_vec);
    };

  public:
    typedef LRUIPVRPParams Params;
    LRUIPVRP(const Params &p);
    ~LRUIPVRP() = default;

    /**
     * Invalidate replacement data to set it as the next probable victim.
     * This function sets the block's position to numWays (maximum position),
     * effectively marking it as the least recently used block that should
     * be evicted next. This is called when a block is invalidated.
     *
     * @param replacement_data Replacement data to be invalidated.
     */
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data) 
                    const override;

    /**
     * Touch an entry to update its replacement data on a cache hit.
     * This function promotes the block to a new position based on the IPV.
     * It shifts other blocks' positions as needed to maintain the LRU order,
     * implementing the promotion policy defined by the IPV vector.
     *
     * @param replacement_data Replacement data to be touched.
     */
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) 
               const override;

    /**
     * Reset replacement data. Used when a new block is inserted into cache.
     * This function inserts the block at the position defined by IPV[numWays].
     * It shifts other blocks' positions as needed to make room for the new
     * block, implementing the insertion policy defined by the IPV vector.
     *
     * @param replacement_data Replacement data to be reset.
     */
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) 
               const override;

    /**
     * Find replacement victim using LRU-IPV positions.
     * Searches through all candidates to find the block with the highest
     * position value (least recently used). That block is selected as the
     * victim for eviction.
     *
     * @param candidates Replacement candidates, selected by indexing policy.
     * @return Replacement entry to be replaced.
     */
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) 
                                const override;

    /**
     * Instantiate a replacement data entry.
     * Creates a new IPVReplData for each block. When count % numWays == 0,
     * a new shared position vector is created for the next cache set.
     * All blocks within the same set share the same position vector.
     *
     * @return A shared pointer to the new replacement data.
     */
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

} // namespace ReplacementPolicy

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LRU_IPV_HH__

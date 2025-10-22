/**
 * Implementation of LRU-IPV (Insertion Position Vector) replacement policy.
 * 
 * This policy maintains explicit position values for each block in a cache set,
 * where position 0 is the MRU (Most Recently Used) and position (numWays-1) is
 * the LRU (Least Recently Used). The IPV vector defines where blocks are inserted
 * and how they are promoted on cache hits.
 */

#include "mem/cache/replacement_policies/lru_ipv.hh"

#include <cassert>
#include <memory>

#include "params/LRUIPVRP.hh"
#include "sim/core.hh"

namespace ReplacementPolicy {

/**
 * Constructor: Initialize the LRU-IPV replacement policy.
 * Sets up the number of ways, initializes the count to 0, and defines
 * the IPV vector with the specified insertion and promotion positions.
 * The IPV vector has numWays+1 elements: indices 0 to numWays-1 define
 * promotion positions, and index numWays defines the insertion position.
 */
LRUIPVRP::LRUIPVRP(const Params &p)
  : Base(p), 
    numWays(p.numWays),
    count(0),
    currentPositionVector(nullptr),
    IPV{0, 0, 1, 0, 3, 0, 3, 2, 1, 0, 5, 1, 0, 0, 4, 11, 8}
{
  // Verify that IPV has the correct size (numWays + 1)
  // For a 16-way cache, IPV should have 17 elements
//   assert(IPV.size() == numWays + 1);
}

/**
 * Constructor for IPVReplData: Initialize replacement data for a single block.
 * Each block stores its way index within the set and a shared pointer to the
 * position vector that is shared among all blocks in the same cache set.
 * 
 * @param way_index The way index of this block (0 to numWays-1)
 * @param pos_vec Shared pointer to the position vector for this set
 */
LRUIPVRP::IPVReplData::IPVReplData(const uint64_t way_index, 
                                   std::shared_ptr<PositionVector> pos_vec)
                                   : wayIndex(way_index), positions(pos_vec)
{}

/**
 * invalidate(): Mark a block as the next victim for eviction.
 * 
 * This function is called when a cache block is invalidated (e.g., due to
 * coherence). It sets the block's position to numWays (the maximum position),
 * which marks it as the least recently used block. The next call to getVictim()
 * will select this block for eviction. No shifting of other blocks is needed
 * because we're only updating this block's position to be worse than all others.
 *
 * @param replacement_data The replacement data of the block to invalidate
 */
void LRUIPVRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Cast the replacement data to our IPV-specific type
    std::shared_ptr<IPVReplData> data = 
        std::static_pointer_cast<IPVReplData>(replacement_data);
    
    // Get the shared position vector for this cache set
    std::shared_ptr<PositionVector> positions = data->positions;
    
    // Get this block's way index
    uint64_t way_idx = data->wayIndex;
    
    // Set this block's position to numWays (maximum), marking it as LRU
    // This makes it the next victim for eviction
    (*positions)[way_idx] = numWays;
    
    // printf("invalidate: block at way %lu set to position %lu (LRU)\n", way_idx, numWays);
}

/**
 * touch(): Update replacement data when a block is accessed (cache hit).
 * 
 * This function implements the promotion policy defined by the IPV vector.
 * When a block at position curr_pos is hit, it is promoted to position
 * IPV[curr_pos]. All blocks at positions between IPV[curr_pos] and curr_pos
 * are shifted down (their positions increase by 1) to make room for the
 * promoted block. This maintains the LRU ordering while implementing the
 * specific promotion behavior defined by IPV.
 *
 * @param replacement_data The replacement data of the block that was hit
 */
void
LRUIPVRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Cast the replacement data to our IPV-specific type
    std::shared_ptr<IPVReplData> data = 
        std::static_pointer_cast<IPVReplData>(replacement_data);
    
    // Get the shared position vector for this cache set
    std::shared_ptr<PositionVector> positions = data->positions;
    
    // Get this block's way index and current position in the LRU stack
    uint64_t way_idx = data->wayIndex;
    int curr_pos = (*positions)[way_idx];
    
    // Look up the promotion position from the IPV vector
    // IPV[curr_pos] tells us where to promote this block
    int promote_pos = IPV[curr_pos];
    
    // Debug output
    // printf("touch: block at way %lu, current position %d, promoting to position %d\n", way_idx, curr_pos, promote_pos);
    
    // Shift blocks to make room for the promotion
    // All blocks at positions >= promote_pos and < curr_pos need to be
    // pushed toward LRU (their positions increase by 1)
    for (uint64_t i = 0; i < numWays; i++) {
        int pos = (*positions)[i];
        if (pos >= promote_pos && pos < curr_pos) {
            (*positions)[i] = pos + 1;
        }
    }
    
    // Place the accessed block at its new promoted position
    (*positions)[way_idx] = promote_pos;
}

/**
 * reset(): Initialize replacement data when a new block is inserted.
 * 
 * This function implements the insertion policy defined by the IPV vector.
 * New blocks are inserted at position IPV[numWays] rather than at the MRU
 * position. All blocks at positions >= IPV[numWays] and < numWays are shifted
 * toward LRU (their positions increase by 1) to make room for the new block.
 * This implements the insertion policy that can prevent cache pollution by
 * not immediately promoting new blocks to MRU.
 *
 * @param replacement_data The replacement data of the newly inserted block
 */
void
LRUIPVRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Cast the replacement data to our IPV-specific type
    std::shared_ptr<IPVReplData> data = 
        std::static_pointer_cast<IPVReplData>(replacement_data);
    
    // Get the insertion position from the IPV vector
    // IPV[numWays] defines where new blocks should be inserted
    int insert_pos = IPV[numWays];
    
    // Get this block's way index
    uint64_t way_idx = data->wayIndex;
    
    // Get the shared position vector for this cache set
    std::shared_ptr<PositionVector> positions = data->positions;
    
    // printf("reset: inserting block at way %lu at position %d\n", way_idx, insert_pos);
    
    // Shift blocks to make room for the insertion
    // All blocks at positions >= insert_pos and < numWays need to be
    // pushed toward LRU (their positions increase by 1)
    for (uint64_t i = 0; i < numWays; i++) {
        int pos = (*positions)[i];
        if (pos >= insert_pos && pos < static_cast<int>(numWays)) {
            (*positions)[i] = pos + 1;
        }
    }
    
    // Place the new block at the insertion position
    (*positions)[way_idx] = insert_pos;
}

/**
 * getVictim(): Find the replacement victim among the candidates.
 * 
 * This function searches through all replacement candidates to find the block
 * with the highest position value. The block with the highest position is the
 * least recently used (LRU) block and is selected as the victim for eviction.
 * In the position-based LRU scheme, higher position values indicate blocks
 * that are less recently used and thus better candidates for eviction.
 *
 * @param candidates The set of replacement candidates to choose from
 * @return Pointer to the victim entry to be evicted
 */
ReplaceableEntry*
LRUIPVRP::getVictim(const ReplacementCandidates& candidates) const
{
    // There must be at least one replacement candidate
    assert(candidates.size() > 0);
    
    // Start with the first candidate as the initial victim
    ReplaceableEntry* victim = candidates[0];
    
    // Get the initial victim's position
    std::shared_ptr<IPVReplData> victim_data = 
        std::static_pointer_cast<IPVReplData>(victim->replacementData);
    int max_position = (*victim_data->positions)[victim_data->wayIndex];
    
    // printf("getVictim: searching among %zu candidates\n", candidates.size());
    
    // Search through all candidates to find the one with maximum position
    for (const auto& candidate : candidates) {
        // Get this candidate's replacement data
        std::shared_ptr<IPVReplData> data = 
            std::static_pointer_cast<IPVReplData>(candidate->replacementData);
        
        // Get this candidate's current position
        int pos = (*data->positions)[data->wayIndex];
        
        // If this candidate has a higher position (more LRU), select it
        if (pos > max_position) {
            victim = candidate;
            max_position = pos;
        }
    }
    
    // Debug output
    std::shared_ptr<IPVReplData> final_victim_data = 
        std::static_pointer_cast<IPVReplData>(victim->replacementData);
    // printf("getVictim: selected way %lu with position %d\n", final_victim_data->wayIndex, max_position);
    
    return victim;
}

/**
 * instantiateEntry(): Create a new replacement data entry.
 * 
 * This function creates replacement data for each cache block. It implements
 * the shared state pattern where all blocks in the same cache set share a
 * single position vector. Every numWays calls (i.e., when starting a new set),
 * a new position vector is created and initialized with all positions set to
 * numWays (indicating all blocks are invalid initially). Each block gets a
 * unique way index (count % numWays) that identifies its position within the set.
 *
 * @return Shared pointer to the newly created replacement data
 */
std::shared_ptr<ReplacementData>
LRUIPVRP::instantiateEntry()
{
    // When starting a new cache set (count % numWays == 0), create a new
    // shared position vector for all blocks in this set
    if (count % numWays == 0) {
        // Create a new position vector with all positions initialized to numWays
        // This indicates all blocks are initially invalid/LRU
        currentPositionVector = new PositionVector(numWays, numWays);
        
        // printf("instantiateEntry: created new position vector for set %lu\n", count / numWays);
    }
    
    // Calculate the way index for this block within its set
    uint64_t way_idx = count % numWays;
    
    // Create the replacement data with the way index and shared position vector
    IPVReplData* data = new IPVReplData(way_idx, 
                                        std::shared_ptr<PositionVector>(currentPositionVector));
    
    // Increment the counter for the next block
    count++;
    
    // printf("instantiateEntry: created entry %lu (way %lu)\n", count - 1, way_idx);
    
    return std::shared_ptr<ReplacementData>(data);
}

} // namespace ReplacementPolicy

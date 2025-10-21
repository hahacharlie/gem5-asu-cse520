#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LRU_IPV_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LRU_IPV_HH__

#include "mem/cache/replacement_policies/base.hh"

struct LRUIPVRPParams; //Check


namespace ReplacementPolicy {

class LRUIPVRP : public Base
{
  protected:
    /** implementing the replacement data for IPV based LRU. */
    const int way_num;
    mutable int index; //counting the number of blocks
    
    typedef std::vector<int>set;  
    /*typedef struct set
    {
    	std::vector<int>vec;
    	int insert;
    	set(){insert=0};  
    }set;*/  
    //mutable std::vector<set*>cache;
    set*vec;
    
    struct IPVReplData : ReplacementData
    {
        /** Tick on which the entry was last touched. */
        Tick lastTouchTick;
	mutable int curr_pos;
	std::shared_ptr<set> set_id; 
	/**
         * Default constructor. Invalidate data.
         */
        IPVReplData(int index);
    };    
    
    const std::vector<int>IPV{0,0,1,0,3,0,1,2,1,0,5,1,0,0,1,11,13};
  public:
    typedef LRUIPVRPParams Params;
    LRUIPVRP(const Params &p);
    ~LRUIPVRP() = default;

    /**
     * Invalidate replacement data to set it as the next probable victim.
     * Sets its last touch tick as the starting tick.
     *
     * @param replacement_data Replacement data to be invalidated.
     */
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
                                                              const override;

    /**
     * Touch an entry to update its replacement data.
     * Sets its last touch tick as the current tick.
     *
     * @param replacement_data Replacement data to be touched.
     */
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Reset replacement data. Used when an entry is inserted.
     * Sets its last touch tick as the current tick.
     *
     * @param replacement_data Replacement data to be reset.
     */
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Find replacement victim using LRU timestamps.
     *
     * @param candidates Replacement candidates, selected by indexing policy.
     * @return Replacement entry to be replaced.
     */
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
                                                                     override;

    /**
     * Instantiate a replacement data entry.
     *
     * @return A shared pointer to the new replacement data.
     */
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

} // namespace ReplacementPolicy

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LRU_RP_HH__

#include "mem/cache/replacement_policies/lru_ipv.hh"

#include <cassert>
#include <memory>

#include "params/LRUIPVRP.hh"
#include "sim/core.hh"

namespace ReplacementPolicy {

LRUIPVRP::LRUIPVRP(const Params &p)
  : Base(p), index(0), way_num(p.numWays){}

LRUIPVRP::IPVReplData::IPVReplData(int index):curr_pos(index), set_id(nullptr){}

void
LRUIPVRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
const
{

    std::shared_ptr<IPVReplData> data =  std::static_pointer_cast<IPVReplData>(replacement_data);
    printf("invalidate: block %d", data->curr_pos);
    //set* id = data->set_id.get();
    //id->at(data->curr_pos)=way_num;
    
}

void
LRUIPVRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Update last touch timestamp
    std::shared_ptr<IPVReplData> data =  std::static_pointer_cast<IPVReplData>(replacement_data);
    
    set* id = data->set_id.get();
    int curr_pos=data->curr_pos;
    printf("touch: block %d", curr_pos);  
    int promote_pos=IPV[curr_pos];
    for(int i=0;i<id->size();i++){
      int pos=id->at(i);
      if((pos>=promote_pos)&&(pos<curr_pos)){
      	id->at(i)++;
      }
    }
    id->at(curr_pos) = promote_pos; 
    
}

void
LRUIPVRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    std::shared_ptr<IPVReplData>data = std::static_pointer_cast<IPVReplData>(replacement_data);
    int insert = IPV[way_num];
    int curr_pos = data->curr_pos; 
    printf("reset %d\n", data->curr_pos);   
    set* id = data->set_id.get();
    for(int i=0;i<id->size();i++){  
        int pos=id->at(i);  
    	if(pos>=insert&&pos<way_num){
    	   id->at(i)++;
    	}    	
    }
    //data->curr_pos=curr_pos%16;
    //data->set_id=std::shared_ptr<set>(id);
    id->at(curr_pos)=insert;
}

ReplaceableEntry*
LRUIPVRP::getVictim(const ReplacementCandidates& candidates) const
{
    // There must be at least one replacement candidate
    assert(candidates.size() > 0);

    // Visit all candidates to find victim
    //std::shared_ptr<IPVReplData>data = std::static_pointer_cast<IPVReplData>(candidate->replacementData);
    
    /*set* id = data->set_id.get();
    int victim_id=0;
    int max=0;
    for (int i=0;i<id->size();i++) {
        // Update victim entry if necessary
        if (max<id->at(i))){
            max = id->at(i);
            victim_id = i;
            break;
        }
    }
    printf("get victim %d\n", victim_id);
    return candidate[victim_id];*/
    printf("number of candidates:%d",candidates.size());
    ReplaceableEntry* victim = candidates[0];
    int victim_id=0;
    int max=0;
    int cnt=0;
    for (const auto& candidate : candidates) {
        // Update victim entry if necessary
        std::shared_ptr<IPVReplData>data = std::static_pointer_cast<IPVReplData>(candidate->replacementData);
        set*id = data->set_id.get();
        int curr_pos=data->curr_pos;
        if (max<id->at(cnt)) {
            victim = candidate;
            victim_id=cnt;
            max=id->at(cnt);
        }
        cnt++;
    }
    printf("get victim %d\n", victim_id);
    return victim;
}

std::shared_ptr<ReplacementData>
LRUIPVRP::instantiateEntry()
{
    //set*vec=nullptr;
    if(index%16==0){
    	vec = new set(way_num, way_num);
    	//cache.push_back(vec);
    }
    
    IPVReplData* data = new IPVReplData(index%way_num);
    data->set_id=std::shared_ptr<set>(vec);
    index=index+1;
    printf("instantiate entry %d\n", index);
    return std::shared_ptr<ReplacementData>(data);
}

} // namespace ReplacementPolicy

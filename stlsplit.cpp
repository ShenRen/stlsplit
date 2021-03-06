/*
    Copyright 2014 Alessandro Ranellucci, Miro Hrončok
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <set>
#include <queue>
#include <deque>
#include "stlsplit.h"

std::vector<stl_file*> stl_split(stl_file* stl_in) {
  // We need neighbors and filled holes
  stl_repair(stl_in, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /* fill_holes_flag */, 0, 0, 0, 0);

  std::set<int> seen_facets;
  std::vector<stl_file*> parts;
  
  // loop while we have remaining facets
  for (;;) {
    // get the first facet
    std::queue<int> facet_queue;
    std::deque<int> facets;
    for (int facet_idx = 0; facet_idx < stl_in->stats.number_of_facets; facet_idx++) {
      if (seen_facets.find(facet_idx) == seen_facets.end()) {
        // if facet was not seen put it into queue and start searching
        facet_queue.push(facet_idx);
        break;
      }
    }
    if (facet_queue.empty()) {
      return parts;
    }
    
    while (!facet_queue.empty()) {
      int facet_idx = facet_queue.front();
      facet_queue.pop();
      if (seen_facets.find(facet_idx) != seen_facets.end()) continue;
      facets.push_back(facet_idx);
      for (int j = 0; j <= 2; j++) {
        facet_queue.push(stl_in->neighbors_start[facet_idx].neighbor[j]);
      }
      seen_facets.insert(facet_idx);
    }
    
    stl_file *stl_out = new stl_file;
    stl_initialize(stl_out);
    stl_out->stats.type = inmemory;
    stl_out->stats.number_of_facets = facets.size();
    stl_out->stats.original_num_facets = stl_out->stats.number_of_facets;
    stl_allocate(stl_out);
    
    
    int first = 1;
    for (std::deque<int>::const_iterator facet = facets.begin(); facet != facets.end(); facet++) {
      stl_out->facet_start[facet - facets.begin()] = stl_in->facet_start[*facet];
      stl_facet_stats(stl_out, stl_in->facet_start[*facet], first);
      first = 0;
    }
    
    parts.push_back(stl_out);
  }
}

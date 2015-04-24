#include "model_space.h"

#if DEBUG
# include <stdio.h>
#endif
#include <set>
#include <vector>
#include "geom_3d.h"

using namespace std;

namespace ModelSpace {

// Since the definitions in the header file were extern-ed, we have to do the
// regular declarations and initializations here, lest client code get
// "undefined symbol" errors when linking.  Client code should assign these
// variables nonzero values at least once.
UIVec box_size(0,0,0);
UIVec size(0,0,0);
UIVec size_rounded_to_boxes(0,0,0);
UIVec num_boxes(0,0,0);

void SetSize(UIVec const & true_size) {
  size = true_size;
  num_boxes = true_size.CeilDivide(box_size);
  size_rounded_to_boxes = num_boxes * box_size;
}

UIVec BoxContaining(Point const & x) {
  return (UIVec(x) / box_size) * box_size;
}

void MinMaxBoxes(set<UIVec> const & known_boxes,
                 UIVec & min_box, UIVec & max_box) {
#if DEBUG
  if(known_boxes.empty()) {
    printf("ModelMap::MinMaxBoxes() called with an empty box list!\n");
    return;
  }
#endif
  set<UIVec>::iterator it = known_boxes.begin();
  min_box = *it;
  max_box = *(it++);
  while(it != known_boxes.end()) {
    if(it->x < min_box.x) { min_box.x = it->x; }
    if(it->x > max_box.x) { max_box.x = it->x; }
    if(it->y < min_box.y) { min_box.y = it->y; }
    if(it->y > max_box.y) { max_box.y = it->y; }
    if(it->z < min_box.z) { min_box.z = it->z; }
    if(it->z > max_box.z) { max_box.z = it->z; }
    ++it;
  }
}

void BoxesInBoundingBox(set<UIVec> const & known_boxes,
                        vector<UIVec> & all_boxes) {
  UIVec min_box, max_box;
  MinMaxBoxes(known_boxes, min_box, max_box);
  all_boxes.reserve(all_boxes.size() +
                    ((max_box-min_box)/box_size+UIVec(1,1,1)).Product());
  UIVec box;
  for(box.z = min_box.z; box.z <= max_box.z; box.z += box_size.z) {
    for(box.y = min_box.y; box.y <= max_box.y; box.y += box_size.y) {
      for(box.x = min_box.x; box.x <= max_box.x; box.x += box_size.x) {
        all_boxes.push_back(box);
      }
    }
  }
}

}

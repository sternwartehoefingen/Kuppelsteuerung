#ifndef _AzMounts_H_
#define _AzMounts_H_

#define MountHWidth 5

// relation to next monting position MountPosition[]
typedef enum {POS_none, POS_vicinity, POS_center} mountPosAttr_t;

void Mounts_showState(bool details);

#endif // _AzMounts_H_


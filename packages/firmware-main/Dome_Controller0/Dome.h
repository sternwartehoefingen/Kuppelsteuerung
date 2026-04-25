#ifndef Dome_H
#define Dome_H

#define MOUNTPOSMAX (2 * 1226)
#define MOUNTOFFSET 802
#define MOUNTCOUNT 32  // Number of physical mount positions

// nach Messung vom 2016-01-09

const int16_t MountPosition[] = {
    51,  123,  209,  282,  354,  439,  512,  584,  669,  741, 
   815,  900,  973, 1061, 1146, 1214, 1280, 1353, 1427, 1511, 
  1583, 1656, 1740, 1813, 1886, 1972, 2044, 2115, 2201, 2274, 
  2346, 2430, 51 + MOUNTPOSMAX
};
#endif

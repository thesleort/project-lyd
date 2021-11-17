#ifndef PACKET_H
#define PACKET_H

#define DATATYPE_MOVE   0b10000000

#define MOVE_STOP       0b00000000
#define MOVE_FORWARD    0b00000001
#define MOVE_BACKWARDS  0b00000010
#define MOVE_RIGHT90    0b00000011
#define MOVE_LEFT90     0b00000100

// Ved DATATYPE_MOVE skal den 'or' med en af MOVE_ definitionerne

#endif

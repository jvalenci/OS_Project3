#ifndef _CUBE_H
#define _CUBE_H

#define SOLUTION

struct cube;

struct wizard {
  int x;
  int y;
  int id;
  char team;
  int status; /* 0: alive, 1: frozen */
  struct cube *cube; 

  /* Fill in as required */
};
  
struct room {
  int x;
  int y;
  struct wizard *wizards[2];

  /* Fill in as required */
};

struct cube {
  int size;
  int teamA_size;
  int teamB_size;
  struct wizard **teamA_wizards;
  struct wizard **teamB_wizards;
  int game_status;

  /* Pointer to a two-dimensional array of rooms */
  struct room ***rooms;

  /* Fill in as required */
};

extern void print_wizard(struct wizard *);
extern void kill_wizards(struct wizard *);
extern void print_cube(struct cube *);
extern int check_winner(struct cube *);

extern void dostuff();
extern struct room * choose_room(struct wizard*);
extern int try_room(struct wizard *, struct room *, struct room* );
extern void switch_rooms(struct wizard *, struct room *, struct room* );
extern struct wizard * find_opponent(struct wizard*, struct room *);
extern int fight_wizard(struct wizard *, struct wizard *, struct room *);
extern int free_wizard(struct wizard *, struct wizard *, struct room *);

#endif

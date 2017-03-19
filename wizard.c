#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "cube.h"
#include "wizard.h"

void *
wizard_func(void *wizard_descr)
{
	struct cube* cube;
	struct room *newroom;
	struct room *oldroom;
	struct wizard* self;
	struct wizard* other;

	self = (struct wizard*)wizard_descr;
	assert(self);
	cube = self->cube;
	assert(cube);

	/* Sets starting room */
	oldroom = cube->rooms[self->x][self->y];
	assert(oldroom);

	/* Chooses the new room */
	newroom = choose_room(self);

	/* Infinite loop */
	while (1)
	{
		sem_wait(&singleStepMove);

		//kills the wizard
		if (cube->game_status == 1) {
			pthread_exit(NULL);
		}

		//checks if wizard is frozen,
		if (self->status == 1)
		{
			// if this wizard is frozen, allow another wizard to take the move
			sem_post(&singleStepMove);
			dostuff();
			continue;
		}


		printf("Wizard %c%d in room (%d,%d) wants to go to room (%d,%d)\n",
			self->team, self->id, oldroom->x, oldroom->y, newroom->x, newroom->y);

		//locks until if find a room to be in so multi wizards are not getting into the same room
		pthread_mutex_lock(&mutexRoom);

		if (try_room(self, oldroom, newroom)) // room is full
		{
			/* Waits a random amount of time */
			dostuff();

			/* Chooses the new room */
			newroom = choose_room(self);

			printf("Request denied, room locked!\n");
			pthread_mutex_unlock(&mutexRoom);
			sem_post(&commandLineCurser);

			/* Goes back to the initial state and try again */
			continue;
		}

		// room is not full

		printf("Wizard %c%d in room (%d,%d) moves to room (%d,%d)\n",
			self->team, self->id,
			oldroom->x, oldroom->y, newroom->x, newroom->y);

		/* Self is active and has control over both rooms */
		switch_rooms(self, oldroom, newroom);

		other = find_opponent(self, newroom);

		/* If there is not another wizard does nothing */
		if (other == NULL)
		{
			printf("Wizard %c%d in room (%d,%d) finds nobody around\n",
				self->team, self->id, newroom->x, newroom->y);
		}
		else
		{
			/* Other is from opposite team */
			if (tolower(other->team) != tolower(self->team))
			{
				/* Checks if the opponent is active */
				if (other->status == 0)
				{
					printf("Wizard %c%d in room (%d,%d) finds active enemy\n",
						self->team, self->id, newroom->x, newroom->y);

					fight_wizard(self, other, newroom);
				}
				else
				{
					printf("Wizard %c%d in room (%d,%d) finds enemy already frozen\n",
						self->team, self->id, newroom->x, newroom->y);
				}
			}
			/* Other is from same team */
			else
			{
				/* Checks if the friend is frozen */
				if (other->status == 1)
				{
					free_wizard(self, other, newroom);
				}
			}
		}

		// unlocks once a wizard is in a room and has taken action
		// (don't want other wizards moving around while you're trying
		// to fight or heal)
		pthread_mutex_unlock(&mutexRoom);

		/* Thinks about what to do next */
		dostuff();

		oldroom = newroom;
		newroom = choose_room(self);
		sem_post(&commandLineCurser);
	}

	return NULL;
}

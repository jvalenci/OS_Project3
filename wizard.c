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
		//sem_wait(&commandLineCurser);
		sem_wait(&singleStepMove);

		if (cube->game_status == 1) {
			pthread_exit(NULL);
		}

		//checks if wizard is frozen, 
		if (self->status == 1)
		{
			sem_post(&commandLineCurser);
			continue;
		}

		/* Loops until he's able to get a hold on both the old and new rooms */
		while (1)
		{
			
			printf("Wizard %c%d in room (%d,%d) wants to go to room (%d,%d)\n",
				self->team, self->id, oldroom->x, oldroom->y, newroom->x, newroom->y);

			//locks until if find a room to be in so multi wizards are not getting into the same room
			pthread_mutex_lock(&mutexRoom);

			if (try_room(self, oldroom, newroom))
			{
				/* Waits a random amount of time */
				dostuff();

				/* Chooses the new room */
				newroom = choose_room(self);

				printf("Request denied, room locked!\n");
				pthread_mutex_unlock(&mutexRoom);
				sem_post(&commandLineCurser);
				sem_wait(&singleStepMove);

				/* Goes back to the initial state and try again */
				continue;
			}
			else
			{
				break;
			}
		}

		printf("Wizard %c%d in room (%d,%d) moves to room (%d,%d)\n",
			self->team, self->id,
			oldroom->x, oldroom->y, newroom->x, newroom->y);

		/* Fill in */

		/* Self is active and has control over both rooms */
		switch_rooms(self, oldroom, newroom);

		//unlocks once a wizard is in a roomm
		pthread_mutex_unlock(&mutexRoom);

		other = find_opponent(self, newroom);

		/* If there is not another wizard does nothing */
		if (other == NULL)
		{
			printf("Wizard %c%d in room (%d,%d) finds nobody around\n",
				self->team, self->id, newroom->x, newroom->y);
			/* Fill in */
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

			/* Fill in */
		}

		/* Thinks about what to do next */
		dostuff();

		oldroom = newroom;
		newroom = choose_room(self);
		sem_post(&commandLineCurser);
	}

	return NULL;
}
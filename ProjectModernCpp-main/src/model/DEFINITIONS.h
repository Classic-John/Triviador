#pragma once
#include <stdint.h>

/*
	This file contains definitions to specific game parameters. These might include Number of Choices, Number of Rounds, Maximum Number of Players and more.
	Creating a separate header file for all the definitions makes it easy to adjust different parameters without the need of searching through the Class Hierarchy
*/

#define NUMBER_CHOICES 4
#define QUESTION_FIRST_THRESHOLD 70
#define QUESTION_SECOND_THRESHOLD 90
#define POSSIBLE_ANSWERS 4
#define NEGATIVE_ERROR 10
#define POSITIVE_ERROR 10
#define MIN_PROBABILITY_OF_CORRECT_ANSWER 30

#define BASE_SCORE 300
#define TERRITORY_SCORE 100

#define NR_DUELING_ROUNDS 5
typedef uint32_t numericAnswer;
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lab_6_my_string.h"
#include "lab_7_generic_vector.h"
#include "lab_8_binary_search_tree.h"

void read_dictionary(GENERIC_VECTOR set_of_words[]);
void play_evil_hangman(GENERIC_VECTOR set_of_words[]);
int get_number_of_letters(GENERIC_VECTOR set_of_words[]);
Boolean no_words_of_length(int number_of_letters, GENERIC_VECTOR set_of_words[]);
int get_number_of_guesses(void);
char get_guess(int guesses_so_far[]);
GENERIC_VECTOR get_largest_word_bin(GENERIC_VECTOR working_words, MY_STRING display_string, char guess);
Status get_word_key_value(MY_STRING current_word_family, MY_STRING new_key, MY_STRING word, char guess);

int count_letters(MY_STRING display_string, char c);
void display_words_and_key(MY_STRING key, GENERIC_VECTOR working_words, int index);

int user_wishes_to_continue(void);
void clear_keyboard_buffer(void);

int main(int argc, char* argv[])
{
	//Step 1- Make the handle and set it to NULL
	GENERIC_VECTOR set_of_words[30];
	int i;

	for (i = 0; i < 30; i++)
	{
		set_of_words[i] = NULL;
	}

	//Step 2- Init the handle(s)
	for (i = 0; i < 30; i++)
	{
		set_of_words[i] = generic_vector_init_default(my_string_assignment, my_string_destroy);
		if (set_of_words[i] == NULL)
		{
			printf("Unable to create a generic vector\n");
			exit(1);
		}
	}

	//Step 3- Do stuff
	read_dictionary(set_of_words);
	
	//CHECKPOINT 1 - LAB 7
	/*
	for (i = 0; i < 30; i++)
	{
		printf("[%d]: %d\n", i, generic_vector_get_size(set_of_words[i]));
	}
	*/
	do
	{
		play_evil_hangman(set_of_words);
	} while (user_wishes_to_continue());

	//Step 4- Destroy the object(s)
	for (i = 0; i < 30; i++)
	{
		generic_vector_destroy(&set_of_words[i]);
	}

	return 0;
}

void read_dictionary(GENERIC_VECTOR set_of_words[])
{
	MY_STRING hString = NULL;
	FILE* fp;
	//int i;

	hString = my_string_init_default();
	if (hString == NULL)
	{
		printf("Couldn't init the string handle\n");
		exit(1);
	}

	fp = fopen("dictionary.txt", "r");
	if (fp == NULL)
	{
		printf("Couldn't open dictionary.txt for reading.\n");
		exit(1);
	}
	
	while (my_string_extraction(hString, fp))
	{
		generic_vector_push_back(set_of_words[my_string_get_size(hString)], hString);
	}
	
	/*//Printing 
	for (i = 0; i < generic_vector_get_size(set_of_words[6]); i++)
	{
		my_string_insertion(generic_vector_at(set_of_words[6], i),stdout);
		printf("\n");
	}
	*/
	fclose(fp);
	my_string_destroy(&hString);
}

void play_evil_hangman(GENERIC_VECTOR set_of_words[])
{
	int number_of_letters;
	int number_of_guesses;
	int i;
	int index;
	char current_guess;
	int guesses_so_far[26] = { 0 };
	Boolean game_is_over = FALSE;
	GENERIC_VECTOR working_words = NULL;
	MY_STRING display_string = NULL;
	MY_STRING old_display_string = NULL;

	//Step 1 of the game: get the amount of letters of the word
	number_of_letters = get_number_of_letters(set_of_words);

	//Step 2 of the game: Make a complete independent copy of that set of words
	//Create the generic vector working_words (init)
	working_words = generic_vector_init_default(my_string_assignment, my_string_destroy);
	if (working_words == NULL)
	{
		printf("Couldn't initialize the generic vector.\n");
		exit(1);
	}
	//Create the string display_string (init)
	display_string = my_string_init_default();
	if (display_string == NULL)
	{
		printf("Couldn't initialize the string object.\n");
		exit(1);
	}

	old_display_string = my_string_init_default();
	if (old_display_string == NULL)
	{
		printf("Couldn't initialize the string object.\n");
		exit(1);
	}

	//Walk thorugh the number of letters of the ith element of the array vector
	for (i = 0; i < generic_vector_get_size(set_of_words[number_of_letters]); i++)
	{
		//Push it into the vector working_words
		generic_vector_push_back(working_words, generic_vector_at(set_of_words[number_of_letters], i));
	}

	for (i = 0; i < number_of_letters; i++)
	{
		my_string_push_back(display_string, '-');
	}

	//Step 3 of the game: get the amount of guesses from the user
	number_of_guesses = get_number_of_guesses();

	//Step 4 of the game: Playing the game
	while (!game_is_over)
	{
		my_string_insertion(display_string, stdout);
		printf("\n");
		my_string_assignment(&old_display_string, display_string);
		printf("There are %d number of words left\n", generic_vector_get_size(working_words)); //Comment out later
		current_guess = get_guess(guesses_so_far);
		working_words = get_largest_word_bin(working_words, display_string, current_guess);

		if (my_string_compare(old_display_string, display_string) == 0)
		{
			printf("I'm sorry, but there were no %c's in the word\n", current_guess);
			number_of_guesses--;
			if (number_of_guesses <= 0)
			{
				game_is_over = TRUE;
			}
		}
		else
		{
			printf("Yes, there were %c's in the word\n", current_guess);
			if (count_letters(display_string, '-') == 0)
			{
				game_is_over = TRUE;
			}
		}
	}

	if (count_letters(display_string, '-') == 0)
	{
		printf("Congratulations! You've won the game! The word was: ");
		my_string_insertion(generic_vector_at(working_words, 0), stdout);
		printf("\n");
	}
	else
	{
		index = rand() % generic_vector_get_size(working_words);
		printf("I'm sorry, but the word I was looking for was: ");
		my_string_insertion(generic_vector_at(working_words, index), stdout);
		printf("\n");
	}

	my_string_destroy(&old_display_string);
	my_string_destroy(&display_string);
	generic_vector_destroy(&working_words);
}

int get_number_of_letters(GENERIC_VECTOR set_of_words[])
{
	int number_of_letters = 0;

	printf("How many letters do you want in the word? : ");
	scanf(" %d", &number_of_letters);
	clear_keyboard_buffer();

	while (no_words_of_length(number_of_letters, set_of_words))
	{
		printf("Sorry, there are no words of that length in the dictionary.\n");
		printf("How many letters do you want in the word? : ");
		scanf(" %d", &number_of_letters);
		clear_keyboard_buffer();
	}

	return number_of_letters;
}

Boolean no_words_of_length(int number_of_letters, GENERIC_VECTOR set_of_words[])
{
	//Start by saying there are words of that length
	//Boolean no_words_of_this_length = TRUE;

	//check to see if the value given is in bound
	if (number_of_letters < 0 || number_of_letters > 29)
	{
		return TRUE;
	}
	//If it's in bound, there is a generic vector
	if (generic_vector_get_size(set_of_words[number_of_letters]) > 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int get_number_of_guesses(void)
{
	int number_of_guesses = 0;

	while (number_of_guesses < 1)
	{
		printf("Please enter how many guesses you would like to have: ");
		scanf(" %d", &number_of_guesses);
		clear_keyboard_buffer();
	}

	return number_of_guesses;
}

char get_guess(int guesses_so_far[])
{
	char c;
	int i;
	int j;

	printf("Here is a list of the letters that you have already selected:\n");
	for (i = 0; i < 26; i++)
	{
		if (guesses_so_far[i] == 1)
		{
			printf("%c, ", i + 'a');
		}
	}
	printf("\n");

	printf("What is your next guess? : ");
	scanf(" %c", &c);
	c = tolower(c);
	clear_keyboard_buffer();
	j = c - 'a';

	while (!isalpha(c) || guesses_so_far[j] != 0)
	{
		printf("The value is either invalid or has already been selected.\n");
		printf("What is your next guess? : ");
		scanf(" %c", &c);
		c = tolower(c);
		clear_keyboard_buffer();
		j = c - 'a';
	}

	guesses_so_far[j] = 1;
	return c;
}

GENERIC_VECTOR get_largest_word_bin(GENERIC_VECTOR working_words, 
	  MY_STRING display_string, char guess)
{
	MY_STRING key = NULL;
	BS_TREE word_bins = NULL;
	int i;
	Node* node_with_largest_bin;
	MY_STRING largest_bin_key = NULL;

	/***INITIALIZING THE OBJECTS***/
	word_bins = binary_search_tree_init_default();
	if (word_bins == NULL)
	{
		printf("Couldn't allocate space for node.\n");
		exit(1);
	}

	key = my_string_init_default();
	if (key == NULL)
	{
		printf("Couldn't allocate space for key string.\n");
		exit(1);
	}

	/***DOING STUFF WITH THE OBJECTS***/
	for (i = 0; i < generic_vector_get_size(working_words); i++)
	{
		//Generate a key for every word in working_words
		get_word_key_value(display_string, key, generic_vector_at(working_words, i), guess);
		//display_words_and_key(key, working_words, i);
	    //Once I have a key, put it in the tree. It's a Key value pair
		//Given the key and value, the tree can sort it.
		binary_search_tree_insert(word_bins, generic_vector_at(working_words, i), key);
		//Re-use the same key
		while(!my_string_empty(key))
		{
			my_string_pop_back(key);
		}
	}

	//binary_search_tree_display_bins(word_bins);
	printf("\n");

	//Find whatever the node of the largest is
	//Assume the node of the largest is the very first thing.
	//If the node is bigger, it will remember the pointer to the other node 
	node_with_largest_bin = binary_search_tree_find_max(word_bins);

	largest_bin_key = binary_search_tree_node_key(node_with_largest_bin);
	
	//Once it rememebers which one is the largest, It does a swap between
	  //the working_words and node inside the tree.
	binary_search_tree_swap(&node_with_largest_bin, &working_words);

	//CHANGE THE DISPLAY STRING TO THE STRING OF THE LARGEST BIN
	//USE STRING_ASSIGN
	my_string_assignment(&display_string, largest_bin_key);

	/***DESTROYING THE OBJECTS***/
	binary_search_tree_destroy(&word_bins);
	my_string_destroy(&key);
	
	return working_words;
}

Status get_word_key_value(MY_STRING current_word_family, MY_STRING new_key, MY_STRING word, char guess)
{
	int i;

    while (my_string_get_size(current_word_family) >= my_string_get_capacity(new_key))
	{
		if (my_string_resize(new_key) == FAILURE)
		{
			return FAILURE;
		}
	}

	for (i = 0; i < my_string_get_size(word); i++)
	{
		if (*my_string_at(word, i) == guess)
		{
			my_string_push_back(new_key, guess);
		}
		else
		{
			my_string_push_back(new_key, *my_string_at(current_word_family, i));
		}
	}

	return SUCCESS;
}

int count_letters(MY_STRING display_string, char c)
{
	int i;
	int count = 0;

	for (i = 0; i < my_string_get_size(display_string); i++)
	{
		if (*my_string_at(display_string, i) == c)
		{
			count++;
		}
	}

	return count;
}

void display_words_and_key(MY_STRING key, GENERIC_VECTOR working_words, int index)
{
	my_string_insertion(generic_vector_at(working_words, index), stdout);
	printf(" : ");
	my_string_insertion(key, stdout);
	printf("\n");
}

//Saved program "User_Wishes_to_Continue()"
// Will prompt the user if he wants to continue running the program
int user_wishes_to_continue(void)
{
	char answer;
	
	//-------------------------------------------------------//
	printf("Do you wish to continue? (y/n): ");
	scanf(" %c", &answer);
	clear_keyboard_buffer();

	while (answer != 'y' && answer != 'Y' &&
		   answer != 'n' && answer != 'N')
	{
		printf("I'm sorry but that input is not valid\n");
		printf("Do you wish to continue? (y/n): ");
		scanf(" %c", &answer);
		clear_keyboard_buffer();
	}
	// only returns if the character y or Y were inputted
	return answer == 'y' || answer == 'Y';
}

//Saved program "Clear_keyboard_buffer"
//Will clear (eat up) everything up to the "new line" character 
void clear_keyboard_buffer(void)
{
	char c;

	//-------------------------------
	scanf("%c", &c);

	while (c != '\n')
	{
		scanf("%c", &c);
	}

}
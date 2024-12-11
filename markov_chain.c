#include <string.h>
#include "markov_chain.h"

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */

int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
    Node *current = markov_chain->database->first;

    //Performs linear search for the word in the database
    for (int i = 0; i < markov_chain->database->size; i++)
    {
        //If match, return the node
        if (strcmp(current->data->data, data_ptr) == 0)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

//Adds new node (word) to the database
Node* add_to_database(MarkovChain *markov_chain, char *data_ptr)
{
    //Checks if the word exists in the database already
    Node *target = get_node_from_database(markov_chain, data_ptr);

    if (target) //if exists, returns it
    {
        return target;
    }
    
    //otherwise, allocates memeory for the new node:
    if (add(markov_chain->database, (MarkovNode*)malloc(sizeof(MarkovNode))) == 1)
    {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }

    //Copy the word string data to the node
    markov_chain->database->last->data->data = malloc(strlen(data_ptr) + 1);

    if (markov_chain->database->last->data->data == NULL)
    {
        
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }
    else
    {
        memcpy(markov_chain->database->last->data->data, data_ptr, strlen(data_ptr) + 1); // Including null terminator
    }

    //Initializes the frequency list with size 0
    markov_chain->database->last->data->frequency_list = (MarkovNodeFrequency*)calloc(0, sizeof(MarkovNodeFrequency));
    
    if (markov_chain->database->last->data->frequency_list == NULL)
    {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }

    //Initializes the rest of the fields
    markov_chain->database->last->data->array_size = 0;
    markov_chain->database->last->data->total_frequencies = 0;
    
    return markov_chain->database->last;
}

int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node)
{
    if (first_node->array_size > 0)
    {
        //Checks if node already exists in freq list
        for (int i = 0; i < first_node->array_size; i++)
        {
            if (second_node == first_node->frequency_list[i].markov_node)
            {
                //Found, so increase frequency
                first_node->frequency_list[i].frequency++;
                first_node->total_frequencies++;
                return EXIT_SUCCESS;
            }
        }
    }
    
    //so we realloc to update the memory and allocate space for the next node
    first_node->frequency_list = (MarkovNodeFrequency*)realloc(first_node->frequency_list, (first_node->array_size + 1) * sizeof(MarkovNodeFrequency));

    if (first_node->frequency_list == NULL)
    {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }

    //create the new node:
    first_node->frequency_list[first_node->array_size].markov_node = second_node;
    first_node->frequency_list[first_node->array_size].frequency = 1;

    first_node->array_size++;
    first_node->total_frequencies++;

    return EXIT_SUCCESS;
}

void free_database(MarkovChain **ptr_chain)
{
    if (ptr_chain && *ptr_chain)
    {
        MarkovChain *chain = *ptr_chain;

        //free the linked list
        if (chain->database)
        {
            Node *current = chain->database->first;
            Node *next_node;

            //free each every node
            while (current)
            {
                next_node = current->next;

                //Free MarkovNode inside the Node
                if (current->data)
                {
                    if (current->data->data)
                    {
                        free(current->data->data);  //Free the string in MarkovNode
                    }

                    //free the frequency list
                    if (current->data->frequency_list)
                    {
                        free(current->data->frequency_list);  //free the frequency list
                    }

                    free(current->data);  //Fre the MarkovNode itself
                }

                free(current);  // Free the Node
                current = next_node;
            }

            free(chain->database);  // free the LinkedList
        }

        free(chain);  //Free the MarkovChain structure
        *ptr_chain = NULL;
    }
}

//Chooses a random word from the whole markov chain database
MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
    int n = -1;
    Node *current = NULL;

    do
    {
        //Choose random word
        n = get_random_number(markov_chain->database->size);
        current = markov_chain->database->first;

        //Reach the random word
        for (int i = 0; i < n; i++)
        {
            current = current->next;
        }
    }
    //condition to choose words that aren't "sentence-ending"
    while (current->data->data[strlen(current->data->data) - 1] == '.');

    return current->data;
}

//Chooses the next word based on probability (frequency)
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
    //Get random number
    int rand_num = get_random_number(cur_markov_node->total_frequencies) + 1;

    //Iterates over the possible words based on their frequency
    for (int i = 0; i < cur_markov_node->array_size; i++)
    {
        //If the random generated number raches a number less than the
        //frequency of the current node, that means it's the node we want.
        if (rand_num <= cur_markov_node->frequency_list[i].frequency)
        {
            MarkovNode* chosen = cur_markov_node->frequency_list[i].markov_node;

            return chosen;
        }
        else //otherwise we continue to the next node:
        {
            rand_num -= cur_markov_node->frequency_list[i].frequency;
        }
    }
}

//Generates a tweet of maximum length max_length
void generate_tweet(MarkovNode *first_node, int max_length)
{
    //Minimum is 2 words.
    if (max_length < 2) max_length = 2;

    //Generates one word per iteration
    for (int i = 0; i < max_length; i++)
    {
        printf("%s", first_node->data);

        if (first_node->array_size == 0 || 
            first_node->data[strlen(first_node->data) - 1] == '.')
        {
            break;
        }

        printf(" ");
        
        first_node = get_next_random_node(first_node); //next word
    }

    printf("\n"); //newline
}

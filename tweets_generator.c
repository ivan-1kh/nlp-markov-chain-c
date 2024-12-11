#include <stdio.h>
#include "linked_list.h"
#include "markov_chain.h"
#include <stdlib.h>
#include <string.h>

#define FILE_PATH_ERROR "Error: incorrect file path\n"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments\n"

#define DELIMITERS " \n\t\r"

int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain);

int main(int argc, char const *argv[])
{
    //Validate argument count
    if (argc < 4 || argc > 5)
    {
        fprintf(stderr, NUM_ARGS_ERROR);
        exit(EXIT_FAILURE);
    }

    //Send the seed to randomize numbers
    srand(atoi(argv[1]));
    
    //Open file
    FILE *fp = fopen(argv[3], "r");
    
    if (!fp)
    {
        fprintf(stderr, FILE_PATH_ERROR);
        exit(EXIT_FAILURE);
    }

    //Parse number of words to read (optional argument)
    int words_to_read = -1;  // Default: read all words
    if (argc == 5)
    {
        words_to_read = atoi(argv[4]);

        if (words_to_read <= 0)
        {
            fprintf(stderr, "Error: invalid number of words to read\n");
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }

    //Initialize Markov Chain
    MarkovChain *markov_chain = (MarkovChain *)malloc(sizeof(MarkovChain));

    if (!markov_chain)
    {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    //Initialize LinkedList
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));

    if (!list)
    {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE);
        free(markov_chain);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    markov_chain->database = list;

    //Fill database
    if (fill_database(fp, words_to_read, markov_chain) == 1)
    {
        free_database(&markov_chain);
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    
    fclose(fp);

    for (int i = 0; i < atoi(argv[2]); i++)
    {
        printf("Tweet %d: ", i + 1);

        //Generate a tweet
        generate_tweet(get_first_random_node(markov_chain), 20);
    }

    //Free memory
    free_database(&markov_chain);
    return EXIT_SUCCESS;
}

int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
    //Helper variables
    char line[1024];
    char *token;
    char *prev_word = NULL;

    int total_words_read = 0;

    //Reads the corpus file line-by-line
    while (fgets(line, sizeof(line), fp))
    {
        prev_word = NULL;
        token = strtok(line, DELIMITERS);

        while (token)
        {
            total_words_read++;

            //Add current word to the database
            Node *current_node = add_to_database(markov_chain, token);

            //Link the current word to the previous one
            if (prev_word)
            {
                Node *prev_node = get_node_from_database(markov_chain, prev_word);
                
                if (add_node_to_frequency_list(prev_node->data, current_node->data) == 1)
                {
                    fprintf(stderr, "Adding to frequencies list failed\n");
                    return EXIT_FAILURE;
                }
            }

            //Check if we need to stop reading
            if (words_to_read > 0 &&
                token[strlen(token) - 1] == '.' && total_words_read >= words_to_read)
            {
                return EXIT_SUCCESS;
            }

            if (token[strlen(token) - 1] == '.')
            {
                prev_word = NULL;
            }
            else
            {
                prev_word = token;
            }

            token = strtok(NULL, DELIMITERS);
        }
    }

    //check that the word count is valid
    if (words_to_read > 0 && total_words_read < words_to_read)
    {
        fprintf(stderr, "not enough words in the corpus file\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

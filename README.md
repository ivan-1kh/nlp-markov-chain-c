# Tweet Generator 🤖

GitHub Repository: https://github.com/ivan-1kh/nlp-markov-chain-c

## Overview
An NLP-powered tweet generator using Markov Chains and efficient memory management in C.

## Files
- `linked_list.c` / `linked_list.h`
- `markov_chain.c` / `markov_chain.h`
- `tweets_generator.c`

## Compilation
```bash
gcc *.c -W -Wextra -Wvla -std=c99 -o tweets_generator
```
Flags explained:
- `-W -Wextra`: Advanced warning levels
- `-Wvla`: Variable-length array warnings
- `-std=c99`: C99 language standard

## Usage
```bash
./tweets_generator <random_seed> <tweets_count> <corpus_file> <(optional) words_to_read>
```

### Example
```bash
./tweets_generator 45 10 justdoit_tweets.txt 123
```
*note: justdoit_tweets.txt file included in the project is just a test corpus file, and the user can feed it any corpus file they want and simply change the parameter when running the program.

## Author
Ivan Khuri

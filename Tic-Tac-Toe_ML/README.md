# Tic Tac Toe with Machine Learning #

This project is an implementation of the classic game of Tic Tac Toe, with a twist - it uses Machine Learning (ML) to create an AI opponent that learns how to play the game over time.

## Overview ##

The AI opponent is implemented using a technique called Q-Learning, a type of Reinforcement Learning (RL). In Q-Learning, an agent learns to perform actions in an environment so as to maximize some notion of cumulative reward. In the context of Tic Tac Toe, the environment is the game board, the actions are the moves the AI can make, and the reward is winning the game.

The AI starts with no knowledge of the game and learns by playing many games and updating its strategy based on the outcomes of these games. Over time, the AI learns to make moves that increase its chances of winning.

## Features ##

- Play Tic Tac Toe against an AI opponent that learns from each game.
- The AI uses Q-Learning to improve its strategy over time.
- The AI's strategy is stored in a Q-Table, which can be saved to a file and loaded from a file.
- Load new q tables to make the agent play as you want

## Approach ##

The problem of teaching an AI to play Tic Tac Toe is approached as a Reinforcement Learning problem. The AI is modeled as an agent that interacts with an environment (the Tic Tac Toe board), taking actions (making moves) and receiving rewards (winning or losing the game).

The agent's strategy is represented as a Q-Table, a data structure that stores the expected reward for each action in each state. The agent starts with a Q-Table that assigns an equal expected reward to each action in each state, and updates this Q-Table as it plays games and receives actual rewards.

The agent uses the Q-Table to decide which action to take in each state. Specifically, it uses a policy called epsilon-greedy, which means it usually takes the action with the highest expected reward, but occasionally takes a random action. This allows the agent to explore the game and discover new strategies.

The agent's Q-Table can be saved to a file after each game, allowing the agent to continue learning from where it left off the next time it plays. The Q-Table can also be loaded from a file at the start of each game.
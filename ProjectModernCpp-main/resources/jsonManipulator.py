import json
import requests
import html
import random
import numpy as np
import os

def difficulty_to_numeric(difficulty:str) -> int:
    return {
        'easy': 0,
        'medium': 1,
        'hard': 2
    }[difficulty]

def shuffle_answers(answers:list):
    """Function to shuffle answers, returns tuple with first element being the shuffled list, the second being the position of the correct answer

    Args:
        answers (list): _description_

    Returns:
        tuple: _description_
    """
    indices = np.arange(len(answers))
    random.shuffle(indices)
    correct_index = 0
    shuffled = []
    for i in range(len(indices)):
        if indices[i] == len(indices)-1:
            correct_index = i
        shuffled.append(answers[indices[i]])
    return (shuffled, correct_index)

def get_questions(api_url):
    response = requests.get(api_url)
    data = response.text
    replaced_data = data.replace("&quot;", "\\\"")
    categories = json.loads(html.unescape(replaced_data))
    x = categories['results']
    questions = []
    for property in x:
        if property['type'] != 'multiple':
            continue
        possible_answers = property['incorrect_answers']
        possible_answers.append(property['correct_answer'])
        shuffled_answers, correct_index = shuffle_answers(possible_answers)
        questions.append(dict(difficulty = difficulty_to_numeric(property['difficulty']), statement = property['question'], answers = '|'.join(shuffled_answers), correct = correct_index))
    return questions;

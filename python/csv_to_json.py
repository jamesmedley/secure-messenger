# DATASET: https://www.kaggle.com/code/karnikakapoor/spam-or-ham-sms-classifier

import csv
import json

csv_file_path = 'python/dataset/spam.csv'
json_file_path = 'python/dataset/spam.json'

data = []

# Read the CSV file
with open(csv_file_path, mode='r', encoding='iso-8859-1') as csv_file:
    csv_reader = csv.DictReader(csv_file)
    for row in csv_reader:
        row.pop("")
        if row != "":
            data.append(row)

# Write the data to a JSON file
with open(json_file_path, mode='w', encoding='utf-8') as json_file:
    json.dump(data, json_file, indent=4)

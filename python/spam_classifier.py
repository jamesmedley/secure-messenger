import json
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.linear_model import LogisticRegression
from sklearn.svm import SVC
from sklearn.pipeline import Pipeline
from sklearn.metrics import classification_report
import joblib
import onnx
from skl2onnx import convert_sklearn
from skl2onnx.common.data_types import StringTensorType

json_file_path = 'python/dataset/spam.json'

with open(json_file_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

df = pd.DataFrame(data)
X = df['message']
y = df['class']

y = y.map({'ham': 0, 'spam': 1})

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Create a pipeline with TF-IDF vectorization and Logistic Regression
pipeline = Pipeline([
    ('tfidf', TfidfVectorizer(lowercase=False, stop_words='english')),
    ('clf', SVC())
])

pipeline.fit(X_train, y_train) # train

# Predict on the test set
y_pred = pipeline.predict(X_test)

print(classification_report(y_test, y_pred))

joblib.dump(pipeline, 'python/model/spam_classifier.pk1')
joblib.dump(pipeline.named_steps['tfidf'], 'python/model/tfidf_vectorizer.pk1')


# Convert the sklearn model to ONNX
initial_type = [('input', StringTensorType())]
onnx_model = convert_sklearn(pipeline, initial_types=initial_type)

# Save the ONNX model to a file
with open('python/model/spam_classifier.onnx', 'wb') as f:
    f.write(onnx_model.SerializeToString())

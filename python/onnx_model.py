import onnxruntime as ort
from sklearn.feature_extraction.text import TfidfVectorizer
import joblib
from scipy.special import softmax

# Load the ONNX model
session = ort.InferenceSession('python/model/spam_classifier.onnx')

# Load the TF-IDF vectorizer used for training
vectorizer = joblib.load('python/model/tfidf_vectorizer.pk1')

# Define a function to preprocess input data
def prepare_input(texts):
    # Convert texts to a list of lists to match the shape [None, 1]
    return {'input': [[text] for text in texts]}

# Example texts to predict
texts = ["XXXMobileMovieClub: To use your credit, click the WAP link in the next txt message or click here>> http://wap. xxxmobilemovieclub.com?n=QJKGIGHJJGCBL"]

# Prepare inputs for ONNX model
inputs = prepare_input(texts)

# Run inference
outputs = session.run(['label', 'probabilities'], inputs)
print(outputs)
# Print the outputs
labels = outputs[0]
probabilities = outputs[1]
probabilities = softmax(probabilities, axis=1)


print("Labels:", labels)
print("Probabilities:", probabilities)
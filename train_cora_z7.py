import numpy as np
import cv2
import os
import time
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Flatten, Dense
from sklearn.utils import shuffle
import argparse

# Optional: select GPU
os.environ['CUDA_VISIBLE_DEVICES'] = '0'


# Parse command-line arguments

parser = argparse.ArgumentParser(description="Train MNIST 10x10 model")
parser.add_argument("--dataset_dir", type=str, default="MNIST_Dataset_JPG",
                    help="Path to MNIST dataset folder")
args = parser.parse_args()
dataset_dir = args.dataset_dir

dims = (10, 10)
train_images, train_labels = [], []
test_images, test_labels = [], []

# Load dataset

for j, split in enumerate(["MNIST_JPG_training", "MNIST_JPG_testing"]):
    for i in range(10):  # digits 0-9
        folder = os.path.join(dataset_dir, split, str(i))
        for filename in os.listdir(folder):
            img = cv2.imread(os.path.join(folder, filename), cv2.IMREAD_GRAYSCALE)
            if img is None:
                continue
            img = cv2.resize(img, dims, interpolation=cv2.INTER_AREA)
            if j == 0:
                train_images.append(img / 255.0)
                train_labels.append(i)
            else:
                test_images.append(img / 255.0)
                test_labels.append(i)

train_images = np.asarray(train_images, dtype='float32')
test_images = np.asarray(test_images, dtype='float32')
train_labels = np.asarray(train_labels, dtype='uint8')
test_labels = np.asarray(test_labels, dtype='uint8')

train_images, train_labels = shuffle(train_images, train_labels)
test_images, test_labels = shuffle(test_images, test_labels)

-
# Build model

model = Sequential([
    Flatten(input_shape=dims),
    Dense(16, activation='relu', use_bias=False),
    Dense(8, activation='relu', use_bias=False),
    Dense(10, activation='softmax', use_bias=False),
])

model.compile(optimizer='adam',
              loss='sparse_categorical_crossentropy',
              metrics=['accuracy'])

# Train model

model.fit(train_images, train_labels, epochs=20, batch_size=128, validation_split=0.1)
model.summary()

# Evaluate

start_t = time.time()
results = model.evaluate(test_images, test_labels, verbose=0)
total_t = time.time() - start_t
print(f"Inference time for {len(test_images)} test images: {total_t:.4f} s")
print("Test loss, Test accuracy:", results)

# Save weights to files

for w in range(1, len(model.layers)):
    weight_filename = f"layer_{w}_weights.txt"
    with open(weight_filename, 'w') as file:
        file.write('{')
        W = model.layers[w].weights[0].numpy()
        for i in range(W.shape[0]):
            file.write('{')
            for j in range(W.shape[1]):
                file.write(f"{W[i][j]:.6f}")
                if j != W.shape[1]-1:
                    file.write(', ')
            file.write('}')
            if i != W.shape[0]-1:
                file.write(',\n')
        file.write('}')

# Test a single prediction

def main():
    x = np.expand_dims(test_images[0], axis=0)
    print("Test image[0] label:", test_labels[0])
    print("NN Prediction:", np.argmax(model.predict(x)))
    print("Finished.")

if __name__ == "__main__":
    main()

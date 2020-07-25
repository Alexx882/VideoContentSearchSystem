import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Conv2D, Flatten, Dropout, MaxPooling2D
from tensorflow.keras.preprocessing.image import ImageDataGenerator

import os
import numpy as np
import matplotlib.pyplot as plt

######

# Hyperparameters
BATCH_SIZE = 256
N_EPOCHS = 2
IMG_HEIGHT = 256
IMG_WIDTH = 256
IMG_CHANNELS = 3

#####

base_path = './'
if not os.path.exists(base_path):
    raise IOError('Image folder does not exist')

soph_dataset_training_path = os.path.normpath(os.path.join(base_path,'seg_train/seg_train'))
soph_dataset_test_path = os.path.normpath(os.path.join(base_path,'seg_test/seg_test'))

######

train_data_gen = ImageDataGenerator(
    # rescale pixel values [0, 255] to [0, 1]
    rescale=1./255, 
    # augmentation:
    ## allow some rotation 
    rotation_range=20,
    ## allow to shift whole image
    width_shift_range=.05,
    height_shift_range=.05,
    fill_mode="nearest",
    ## allow some zoom
    zoom_range=.1
) \
.flow_from_directory(
    soph_dataset_training_path,
    target_size=(IMG_HEIGHT, IMG_WIDTH),
    class_mode='sparse',
    batch_size=BATCH_SIZE
)
# .flow(x_train, y_train, batch_size=BATCH_SIZE)

test_data_gen = ImageDataGenerator(
    rescale=1./255
    # dont apply augmentation in test set
) \
.flow_from_directory(
    soph_dataset_test_path,
    target_size=(IMG_HEIGHT, IMG_WIDTH),
    class_mode='sparse',
    batch_size=BATCH_SIZE
)
# .flow(x_test, y_test, batch_size=BATCH_SIZE)

######

training_length = train_data_gen.samples
test_length = test_data_gen.samples

######

# build the layers
model = Sequential([
    # convolution over image with 
    ## nr of filters aka depth of the resulting convolution result
    ## kernel size for the convolution
    ## padding if the kernel is placed at the borders
    ## activation function of the result
    ## shape of the input (as it's the first layer)
    Conv2D(16, 3, padding='same', activation='relu', input_shape=(IMG_HEIGHT, IMG_WIDTH, IMG_CHANNELS)),
    # pooling for the results from previous layer by taking max value with pooling size (2, 2)
    MaxPooling2D(),
    Conv2D(32, 3, padding='same', activation='relu'),
    MaxPooling2D(),
    Conv2D(64, 3, padding='same', activation='relu'),
    MaxPooling2D(),
    # flattens the previous result into one dimension (concat of multidimensional results)
    Flatten(),
    # fully connected layer
    Dense(512, activation='relu'),
    # final fully connected layer with 
    ## number of output values = nr of classes
    ## activation function is softmax to create probabilities
    Dense(10, activation='softmax')
])

# compile the architecture
model.compile(
              # optimizer for applying updates
              optimizer='adam',
              # sparse categorical instead of binary as we have more than two classes
              loss='sparse_categorical_crossentropy',
              # the metrics to show while training and on evaluations
              metrics=['accuracy']
              )

# print a summary
model.summary()

model = tf.keras.models.load_model('mixed.h5')

######

history = model.fit(
                    # training data provided by generator
                    train_data_gen,
                    # steps per epoch calculated that all training data is seen once per epoch
                    steps_per_epoch=training_length/BATCH_SIZE,
                    # number of epochs
                    epochs=N_EPOCHS,
                    # test data to visualize results later in plot
                    validation_data=test_data_gen,
                    # steps after each epoch for test data
                    validation_steps=test_length/BATCH_SIZE
                    )

######

model.save('mixed2.h5')

# Video Content Search System
This repository contains self-describing code for a content search system, which classifies frames of videos based on some pretrained convolutional neural network.

## Video Keyframe Extraction in C
Keyframes are extracted by C code contained in the Visual Studio project. `main1.c` is used to extract keyframes for a single video based on the twin-comparison algorithm. <br />
The Visual Studio solution also contains a hierarchical clustering algorithm and an SVM classifier which were not used in the final project.

## Semantic Context Classifier
The CNN used for the project was trained with the Jupyter Notebook `ImageClassifier/classifier.ipynb` based on the [VOC2012 dataset](http://host.robots.ox.ac.uk/pascal/VOC/voc2012/index.html#devkit). 
The dataset was preprocessed to match the folder structure used by Tensorflow's *ImageDataGenerator.flow_from_directory()* method in `VOCdevkit/preprocess_data.py`. The trained model is stored in `ImageClassifier/voc2012.h5`.

## Database for keyframes and predictions
A PostgreSQL database was used to store the following information:
- videos            (id, filepath)
- keyframes         (id, filepath, video_id)
- concepts          (id, name)
- keyframe_concepts (id, keyframe_id, concept_id, confidence)

The tables are created with `database/create_tables.py`. <br />
Videos and keyframes are inserted with `database/insert_keyframes.py` which expects the videos in the folder *videos/v3c1/* and keyframes in the folder *keyframes/v3c1/*. This structure is achieved by executing `main.c` with the files from the project's task definition in the videos folder. <br />
The script `VOCdevkit/preprocess_data.py` automatically adds all concepts based on the preprocessed folders into the database. This happens even before training is started. <br />
Finally, `database/insert_classified_keyframes.py` classifies the keyframes from the task definition with the trained model and inserts the result into the *keyframe_concepts* table.

## Online Search Tool
The web service's sourcecode to search for concepts in the preprocessed keyframes is located in `search_tool`. For easier execution the Flask app and Postgres database were containerized and uploaded to Docker Hub. <br />
In short, the web app loads concepts from the database to show in the search form. After selecting the desired concept and minimum confidence all matching images are displayed. Clicking them opens the corresponding video.